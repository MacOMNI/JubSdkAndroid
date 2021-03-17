#include "JUB_SDK_Hcash.h"
#include "context/ContextHC.h"
#include "utility/util.h"
#include "token/interface/TokenInterface.hpp"
#include "token/interface/HCTokenInterface.hpp"
#include "mSIGNA/stdutils/uchar_vector.h"
#include <TrezorCrypto/ecdsa.h>
#include <TrezorCrypto/secp256k1.h>

namespace jub {

JUB_RV ContextHC::ActiveSelf() {

    auto token = dynamic_cast<HCTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
    auto ctoken = dynamic_cast<CommonTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
    JUB_CHECK_NULL(token);
    JUB_CHECK_NULL(ctoken);
    JUB_VERIFY_RV(token->selectApplet_HC());
    JUB_VERIFY_RV(ctoken->SetTimeout(_timeout));

    return JUBR_OK;
}

JUB_RV ContextHC::signTX(const JUB_ENUM_BTC_ADDRESS_FORMAT& addrFmt, const std::vector<INPUT_HC>& vInputs, const std::vector<OUTPUT_HC>& vOutputs, const std::string& unsignedTrans, std::string& raw) {

    auto token = dynamic_cast<HCTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
    JUB_CHECK_NULL(token);

    //deal inputs
    std::vector<JUB_UINT64> vInputAmount;
    std::vector<std::string> vInputPath;
    for (auto input : vInputs) {
        vInputAmount.push_back(input.amount);
        vInputPath.push_back(_FullBip32Path(input.path));
    }

    //deal outputs
    std::vector<JUB_UINT16> vChangeIndex;
    std::vector<std::string> vChangePath;
    for (std::size_t i = 0, e = vOutputs.size(); i != e; ++i) {
        if (vOutputs[i].changeAddress) {
            vChangeIndex.push_back((JUB_UINT16)i);
            vChangePath.push_back(_FullBip32Path(vOutputs[i].path));
        }
    }

    uchar_vector vUnsignedTrans(unsignedTrans);

    uchar_vector vRaw;
    JUB_VERIFY_RV(token->SignTXBTC(JUB_ENUM_BTC_ADDRESS_FORMAT::OWN,
                                   p2pkh,
                                   (JUB_UINT16)vInputs.size(),
                                   vInputAmount,
                                   vInputPath,
                                   vChangeIndex,
                                   vChangePath,
                                   vUnsignedTrans,
                                   vRaw));

    raw = vRaw.getHex();

    return JUBR_OK;
}

JUB_RV ContextMultisigHC::ActiveSelf() {

    auto token = dynamic_cast<HCTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
    auto ctoken = dynamic_cast<CommonTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
    JUB_CHECK_NULL(token);
    JUB_CHECK_NULL(ctoken);
    JUB_VERIFY_RV(token->selectApplet_HC());
    JUB_VERIFY_RV(ctoken->SetTimeout(_timeout));

    return JUBR_OK;
}

JUB_RV ContextMultisigHC::signTX(const JUB_ENUM_BTC_ADDRESS_FORMAT& addrFmt, const std::vector<INPUT_HC>& vInputs, const std::vector<OUTPUT_HC>& vOutputs, const std::string& unsignedTrans, std::string& raw) {

    auto token = dynamic_cast<HCTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
    JUB_CHECK_NULL(token);

    //deal inputs
    std::vector<JUB_UINT64> vInputAmount;
    std::vector<std::string> vInputPath;
    std::vector<uchar_vector> vRedeemScriptTlv;
    std::vector<uchar_vector> vRedeemScript;
    for (auto input : vInputs) {
        vInputAmount.push_back(input.amount);
        vInputPath.push_back(_FullBip32Path(input.path));

        uchar_vector redeemScript;
        JUB_VERIFY_RV(BuildRedeemScript(input.path, redeemScript));
        vRedeemScript.push_back(redeemScript);

        uchar_vector redeemScriptTlv;
        JUB_VERIFY_RV(BuildRedeemScriptTlv(input.path, redeemScriptTlv));
        vRedeemScriptTlv.push_back(redeemScriptTlv);
    }

    //deal outputs
    std::vector<JUB_UINT16> vChangeIndex;
    std::vector<std::string> vChangePath;
    std::vector<uchar_vector> vChangeRedeemScriptTlv;
    for (std::size_t i = 0, e = vOutputs.size(); i != e; ++i) {
        if (vOutputs[i].changeAddress) {
            //if output is changeAddress,so it must be P2SH_MULTISIG in this context
            vChangeIndex.push_back((JUB_UINT16)i);
            vChangePath.push_back(_FullBip32Path(vOutputs[i].path));
            uchar_vector changeRedeemScriptTlv;
            JUB_VERIFY_RV(BuildRedeemScriptTlv(vOutputs[i].path,changeRedeemScriptTlv));
            vChangeRedeemScriptTlv.push_back(changeRedeemScriptTlv);
        }
    }

    //need DeserializeTx here
    JUB_VERIFY_RV(DeserializeTxHC(unsignedTrans));

    //store signatures and throw signature in TX;
    std::vector<uchar_vector> stored_scriptSigs;
    for (size_t i = 0; i < _tx.inputs.size();i++) {
        stored_scriptSigs.push_back(_tx.inputs[i].scriptSig);     
        _tx.inputs[i].scriptSig.clear();
    }

    std::string rawStringTX;
    JUB_VERIFY_RV(SerializeTxHC(rawStringTX));
    uchar_vector vUnsignedTrans = rawStringTX;

    std::vector<uchar_vector> vSignatureRaw;
    //vSignatureRaw has len in begin,so sigs must has len too.
    JUB_VERIFY_RV(token->SignTXMultisigBTC(addrFmt,
                                           _transType,
                                           (JUB_UINT16)vInputs.size(),
                                           vInputAmount, vInputPath, vRedeemScriptTlv,
                                           vChangeIndex, vChangePath, vChangeRedeemScriptTlv,
                                           vUnsignedTrans,
                                           vSignatureRaw));

    //pushAll
    JUB_VERIFY_RV(pushAll(vSignatureRaw));

    //build sigs
    bool isComplete = true;
    for (size_t i = 0; i < _tx.inputs.size();i++) {
        std::vector<uchar_vector> sigs;
        std::vector<uchar_vector> vOrderedPublickey;
        JUB_VERIFY_RV(OrderDHPublicKey(vInputs[i].path,vOrderedPublickey));

        //stored_signatures[i].read_uint8();  Hcash script did not lead with 0x00
        uchar_vector oneSig;
        if (stored_scriptSigs[i].size() != 0) {  //not first signer,get the signatures for order.
            try {
                stored_scriptSigs[i].reset_it();
                while(1) {
                    stored_scriptSigs[i] | oneSig;
                    if (oneSig[0] == 0x30) {
                        oneSig.insert(oneSig.begin(), (JUB_BYTE)oneSig.size());
                        sigs.emplace_back(oneSig);
                    }
                    else {
                        if (oneSig != vRedeemScript[i]) { //error rawTx
                            JUB_VERIFY_RV(JUBR_ARGUMENTS_BAD);
                        }
                        else {
                            if (stored_scriptSigs[i].get_cur_it() == stored_scriptSigs[i].end()) {
                                break;
                            }
                            else {
                                JUB_VERIFY_RV(JUBR_ARGUMENTS_BAD);
                            }
                        }
                    }     
                }
            //stored_scriptSigs[i].read_uint8();  OP_CHECKMULTISIG don`t need here
            }
            catch(...) {
                JUB_VERIFY_RV(JUBR_ARGUMENTS_BAD);
            }
        }

        //add my sig
        sigs.emplace_back(vSignatureRaw[i]);

        //order sigs
        auto result = BOOL_TRUE;
        uchar_vector preimageI;
        jub::hc::SerializePreimage(_tx, i, vRedeemScript[i], preimageI);
        std::sort(sigs.begin(), sigs.end(),
                  [&result, preimageI, vOrderedPublickey] (uchar_vector sig1, uchar_vector sig2) {
            JUB_BYTE sig1InRaw[64] = {0,};
            JUB_BYTE sig2InRaw[64] = {0,};

            if (1 != ecdsa_der_to_sig(&sig1[1], sig1InRaw)) {
                result = BOOL_FALSE;
                return true;
            }
            if (1 != ecdsa_der_to_sig(&sig2[1], sig2InRaw)) {
                result = BOOL_FALSE;
                return true;
            }
            size_t order1 = -1;
            size_t order2 = -1;
            for (size_t i =0 ;i < vOrderedPublickey.size();i++) {
                if (0 == ecdsa_verify(&secp256k1, HASHER_BLAKE,
                                      &vOrderedPublickey[i][0], sig1InRaw,
                                      &preimageI[0], (uint32_t)preimageI.size())) {
                    order1 = i;
                    continue;
                }
                else if (0 == ecdsa_verify(&secp256k1, HASHER_BLAKE,
                                           &vOrderedPublickey[i][0], sig2InRaw,
                                           &preimageI[0], (uint32_t)preimageI.size())) {
                    order2 = i;
                    continue;
                }      
            }
            if (   -1 == order1
                || -1 == order2
               ) {
                result = BOOL_FALSE;  //???? where comes from the signature
            }

            return order1 < order2;
        });

        if (result == BOOL_FALSE) {
            JUB_VERIFY_RV(JUBR_ARGUMENTS_BAD);
        }
        //build scriptSig
        uchar_vector scriptSig;
        for (auto sig :sigs) {
            scriptSig << sig;
        }         
        scriptSig & vRedeemScript[i];
        _tx.inputs[i].scriptSig = scriptSig;
        if (sigs.size() < _m) {
            isComplete = false;
        }
    }

    //need SerializeTx here
    JUB_VERIFY_RV(SerializeTxHC(raw));

    if (isComplete) {
        return JUBR_OK;
    }
    else {
        return JUBR_MULTISIG_OK;
    }
}

} // namespace jub end
