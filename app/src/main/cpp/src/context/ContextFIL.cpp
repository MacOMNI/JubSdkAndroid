//
//  ContextFIL.cpp
//  JubSDK
//
//  Created by Pan Min on 2020/11/12.
//  Copyright © 2020 JuBiter. All rights reserved.
//


#include "utility/util.h"

#include "context/ContextFIL.h"
#include "token/interface/TokenInterface.hpp"
//#include <TrustWallet/wallet-core/src/Filecoin/Transaction.h>
#include <Filecoin/Signer.h>


namespace jub {


JUB_RV ContextFIL::ActiveSelf() {

    auto token = dynamic_cast<FILTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
    auto ctoken = dynamic_cast<CommonTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
    JUB_CHECK_NULL(token);
    JUB_CHECK_NULL(ctoken);

    JUB_VERIFY_RV(token->SelectAppletFIL());
    JUB_VERIFY_RV(token->GetAppletVersionFIL(_appletVersion));
    JUB_VERIFY_RV(ctoken->SetTimeout(_timeout));
    JUB_VERIFY_RV(token->SetCoinTypeFIL());

    return JUBR_OK;
}


JUB_RV ContextFIL::GetAddress(const BIP32_Path& path, const JUB_UINT16 tag, std::string& address) {

    auto token = dynamic_cast<FILTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
    JUB_CHECK_NULL(token);

    std::string strPath = _FullBip32Path(path);
    JUB_VERIFY_RV(token->GetAddressFIL(strPath, tag, address));

    return JUBR_OK;
}


JUB_RV ContextFIL::GetMainHDNode(const JUB_BYTE format, std::string& xpub) {

    //0x00 for hex, 0x01 for xpub
    if (   JUB_ENUM_PUB_FORMAT::HEX  != format
        && JUB_ENUM_PUB_FORMAT::XPUB != format
        ) {
        return JUBR_ERROR_ARGS;
    }

    auto token = dynamic_cast<FILTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
    JUB_CHECK_NULL(token);

    JUB_VERIFY_RV(token->GetHDNodeFIL(format, _mainPath, xpub));

    return JUBR_OK;
}


JUB_RV ContextFIL::SetMyAddress(const BIP32_Path& path, std::string& address) {

    auto token = dynamic_cast<FILTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
    JUB_CHECK_NULL(token);

    std::string strPath = _FullBip32Path(path);
    JUB_VERIFY_RV(token->GetAddressFIL(strPath, 0x02, address));

    return JUBR_OK;
}


JUB_RV ContextFIL::GetHDNode(const JUB_BYTE& format, const BIP32_Path& path, std::string& pubkey) {

    //0x00 for hex, 0x01 for xpub
    if (   JUB_ENUM_PUB_FORMAT::HEX  != format
        && JUB_ENUM_PUB_FORMAT::XPUB != format
        ) {
        return JUBR_ERROR_ARGS;
    }

    auto token = dynamic_cast<FILTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
    JUB_CHECK_NULL(token);

    std::string strPath = _FullBip32Path(path);
    JUB_VERIFY_RV(token->GetHDNodeFIL(format, strPath, pubkey));

    return JUBR_OK;
}


JUB_RV ContextFIL::SignTransaction(const BIP32_Path& path,
                                   const JUB_UINT64 nonce,
                                   const JUB_UINT64 gasLimit,
                                   JUB_CHAR_CPTR gasFeeCapInAtto,
                                   JUB_CHAR_CPTR gasPremiumInAtto,
                                   JUB_CHAR_CPTR to,
                                   IN JUB_CHAR_CPTR valueInAtto,
                                   IN JUB_CHAR_CPTR input,
                                   OUT std::string& strRaw) {

    auto token = dynamic_cast<FILTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
    JUB_CHECK_NULL(token);

    JUB_CHECK_NULL(gasFeeCapInAtto);
    JUB_CHECK_NULL(gasPremiumInAtto);
    JUB_CHECK_NULL(to);
    JUB_CHECK_NULL(valueInAtto);

    std::string strPath = _FullBip32Path(path);
    std::vector<JUB_BYTE> vPath(strPath.begin(), strPath.end());

    uint256_t gasFeeCap(gasFeeCapInAtto, 10);
    uint256_t gasPremium(gasPremiumInAtto, 10);
    uint256_t value(valueInAtto, 10);

    std::vector<uchar_vector> vSignatureRaw;
    JUB_VERIFY_RV(token->SignTXFIL(nonce,
                                   gasLimit,
                                   gasFeeCap,
                                   gasPremium,
                                   to,
                                   value,
                                   input,
                                   strPath,
                                   vSignatureRaw));

    // finish transaction
    std::string from;
    JUB_VERIFY_RV(GetAddress(path, 0, from));

    TW::Filecoin::Transaction tx(TW::Filecoin::Address(to),
                                 TW::Filecoin::Address(from),
                                 nonce,
                                 value,
                                 gasLimit,
                                 gasFeeCap,
                                 gasPremium);

#if defined(DEBUG)
    // Verify
    std::string pubkey;
    JUB_VERIFY_RV(GetHDNode(JUB_ENUM_PUB_FORMAT::HEX, path, pubkey));

    TW::PublicKey verifyPubk(TW::Data(uchar_vector(pubkey)), TWPublicKeyType::TWPublicKeyTypeSECP256k1);
    TW::Filecoin::Address verifyAddr = TW::Filecoin::Address(verifyPubk);
    if (verifyPubk.isCompressed()) {
        verifyAddr = TW::Filecoin::Address(verifyPubk.extended());
    }

    TW::Filecoin::Address fromAddr = TW::Filecoin::Address(from);
    if (!(fromAddr == verifyAddr)) {
        return JUBR_ARGUMENTS_BAD;
    }

    // verify sign failed for Lite.
    if (!TW::Filecoin::Signer::verify(verifyPubk, tx, vSignatureRaw[0])) {
        return JUBR_VERIFY_SIGN_FAILED;
    }
#endif

    strRaw = tx.serialize(vSignatureRaw[0]);

    return JUBR_OK;
}


} // namespace jub end
