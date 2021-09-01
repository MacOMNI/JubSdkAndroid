//
//  ContextTRX.cpp
//  JubSDK
//
//  Created by Pan Min on 2020/12/13.
//  Copyright © 2020 JuBiter. All rights reserved.
//


#include "utility/util.h"

#include "context/ContextTRX.h"
#include "token/interface/TokenInterface.hpp"
#include "libTRX/libTRX.hpp"
#include "libETH/ERC20Abi.h"
#include <Tron/Signer.h>


namespace jub {


JUB_RV ContextTRX::ActiveSelf() {

    auto token = dynamic_cast<TRXTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
    auto ctoken = dynamic_cast<CommonTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
    JUB_CHECK_NULL(token);
    JUB_CHECK_NULL(ctoken);

    JUB_VERIFY_RV(token->SelectAppletTRX());
    JUB_VERIFY_RV(ctoken->SetTimeout(_timeout));
    JUB_VERIFY_RV(token->SetCoinTypeTRX());

    return JUBR_OK;
}


JUB_RV ContextTRX::GetMainHDNode(const JUB_BYTE format, std::string& xpub) {

    auto token = dynamic_cast<TRXTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
    JUB_CHECK_NULL(token);

    JUB_VERIFY_RV(token->GetHDNodeTRX(format, _mainPath, xpub));

    return JUBR_OK;
}


JUB_RV ContextTRX::GetAddress(const BIP32_Path& path, const JUB_UINT16 tag, std::string& address) {

    auto token = dynamic_cast<TRXTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
    JUB_CHECK_NULL(token);

    std::string strPath = _FullBip32Path(path);
    JUB_VERIFY_RV(token->GetAddressTRX(strPath, tag, address));

    return JUBR_OK;
}


JUB_RV ContextTRX::SetMyAddress(const BIP32_Path& path, std::string& address) {

    auto token = dynamic_cast<TRXTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
    JUB_CHECK_NULL(token);

    std::string strPath = _FullBip32Path(path);
    JUB_VERIFY_RV(token->GetAddressTRX(strPath, 0x02, address));

    return JUBR_OK;
}


JUB_RV ContextTRX::GetHDNode(const JUB_BYTE format, const BIP32_Path& path, std::string& pubkey) {

    auto token = dynamic_cast<TRXTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
    JUB_CHECK_NULL(token);

    std::string strPath = _FullBip32Path(path);
    JUB_VERIFY_RV(token->GetHDNodeTRX(format, strPath, pubkey));

    return JUBR_OK;
}


JUB_RV ContextTRX::SignTransaction(const BIP32_Path& path,
                                   JUB_CHAR_CPTR packedContractInPb,
                                   std::string& rawInJSON) {

    auto token = dynamic_cast<TRXTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
    JUB_CHECK_NULL(token);

    JUB_CHECK_NULL(packedContractInPb);

    std::string strPath = _FullBip32Path(path);
    std::vector<JUB_BYTE> vPath(strPath.begin(), strPath.end());

    uchar_vector vRaw;
    JUB_VERIFY_RV(jub::trx::SerializePreimage(packedContractInPb,
                                              vRaw));

    std::vector<uchar_vector> vSignatureRaw;
    JUB_VERIFY_RV(token->SignTXTRX(vPath,
                                   vRaw,
                                   vSignatureRaw));

    // finish transaction
    try {
        TW::Tron::Transaction tx;
        tx.raw_data.deserialize(vRaw);
        tx.signature = vSignatureRaw[0];

#if defined(DEBUG)
        //verify
        std::string pubkey;
        JUB_VERIFY_RV(token->GetHDNodeTRX(JUB_ENUM_PUB_FORMAT::HEX, strPath, pubkey));

        TW::Tron::Signer signer;
        if (!signer.verify(TW::PublicKey(TW::Data(uchar_vector(pubkey)), TWPublicKeyType::TWPublicKeyTypeSECP256k1),
                           tx)) {
            return JUBR_VERIFY_SIGN_FAILED;
        }
#endif

        rawInJSON = tx.serialize().dump();
    }
    catch (...) {
        return JUBR_ARGUMENTS_BAD;
    }

    return JUBR_OK;
}


JUB_RV ContextTRX::BuildTRC20TransferAbi(JUB_CHAR_CPTR to, JUB_CHAR_CPTR value, std::string& abi) {

    TW::Tron::Address toAddr(to);
    std::vector<JUB_BYTE> vTo = uchar_vector(toAddr.bytes.begin(), toAddr.bytes.end());
    std::vector<JUB_BYTE> vValue = jub::HexStr2CharPtr(DecStringToHexString(std::string(value)));
    uchar_vector vAbi = jub::eth::ERC20Abi::serialize(vTo, vValue);
    abi = vAbi.getHex();

    return JUBR_OK;
}


JUB_RV ContextTRX::SetTRC20Token(JUB_CHAR_CPTR pTokenName,
                                 JUB_UINT16 unitDP,
                                 JUB_CHAR_CPTR pContractAddress) {

    auto token = dynamic_cast<TRXTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
    JUB_CHECK_NULL(token);

    JUB_CHECK_NULL(pTokenName);
    JUB_CHECK_NULL(pContractAddress);

    std::string tokenName = std::string(pTokenName);
    std::string contractAddress = uchar_vector(TW::Tron::Address::toHex(std::string(pContractAddress))).getHex();

    JUB_VERIFY_RV(token->SetTRC20Token(tokenName,
                                       unitDP,
                                       contractAddress));

    return JUBR_OK;
}


JUB_RV ContextTRX::PackTransactionRaw(const JUB_TX_TRX& tx,
                                      std::string& packedContractInPB) {

    auto token = dynamic_cast<TRXTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
    JUB_CHECK_NULL(token);

    try {
        JUB_VERIFY_RV(jub::trx::PackTransactionRaw(tx,
                                                   packedContractInPB));
    }
    catch (...) {
        return JUBR_ARGUMENTS_BAD;
    }

    return JUBR_OK;
}


} // namespace jub end
