//
//  JUB_SDK_FIL.cpp
//  JubSDK
//
//  Created by Pan Min on 2020/11/12.
//  Copyright © 2020 JuBiter. All rights reserved.
//

#include "JUB_SDK_FIL.h"

#include "utility/util.h"

#include "context/ContextFIL.h"
#include "token/interface/TokenInterface.hpp"


JUB_RV _allocMem(JUB_CHAR_PTR_PTR memPtr, const std::string &strBuf);

/*****************************************************************************
 * @function name : JUB_CreateContextFIL
 * @in  param : cfg
 *          : deviceID - device ID
 * @out param : contextID
 * @last change :
 *****************************************************************************/
JUB_RV JUB_CreateContextFIL(IN CONTEXT_CONFIG_FIL cfg,
                            IN JUB_UINT16 deviceID,
                            OUT JUB_UINT16* contextID) {

    if (nullptr == jub::TokenManager::GetInstance()->GetOne(deviceID)) {
        JUB_VERIFY_RV(JUBR_ARGUMENTS_BAD);
    }

    jub::ContextFIL* context = new jub::ContextFIL(cfg, deviceID);
    *contextID = jub::ContextManager::GetInstance()->AddOne(context);
    context->ActiveSelf();

    return JUBR_OK;
}


/*****************************************************************************
 * @function name : JUB_GetAddressFIL
 * @in  param : contextID - context ID
 *          : path
 *          : bShow
 * @out param : address
 * @last change :
 *****************************************************************************/
JUB_RV JUB_GetAddressFIL(IN JUB_UINT16 contextID,
                         IN BIP32_Path    path,
                         IN JUB_ENUM_BOOL bShow,
                         OUT JUB_CHAR_PTR_PTR address) {

    JUB_CHECK_CONTEXT_FIL(contextID);

    auto context = (jub::ContextFIL*)jub::ContextManager::GetInstance()->GetOne(contextID);
    JUB_CHECK_NULL(context);

    std::string str_address;
    JUB_VERIFY_RV(context->GetAddress(path, bShow, str_address));
    JUB_VERIFY_RV(_allocMem(address, str_address));

    return JUBR_OK;
}


/*****************************************************************************
 * @function name : JUB_SetMyAddressFIL
 * @in  param : contextID - context ID
 *            : path
 * @out param : address
 * @last change :
 *****************************************************************************/
JUB_RV JUB_SetMyAddressFIL(IN JUB_UINT16 contextID,
                           IN BIP32_Path path,
                           OUT JUB_CHAR_PTR_PTR address) {

//    JUB_CHECK_CONTEXT_FIL(contextID);
//
//    auto context = (jub::ContextFIL*)jub::ContextManager::GetInstance()->GetOne(contextID);
//    JUB_CHECK_NULL(context);
//
//    std::string str_address;
//    JUB_VERIFY_RV(context->SetMyAddress(path, str_address));
//    JUB_VERIFY_RV(_allocMem(address, str_address));

    return JUBR_IMPL_NOT_SUPPORT;
}


/*****************************************************************************
 * @function name : JUB_GetHDNodeFIL
 * @in  param : contextID - context ID
 *          : format - JUB_ENUM_PUB_FORMAT::HEX(0x00) for hex;
 *          : path
 * @out param : pubkey
 * @last change :
 *****************************************************************************/
JUB_RV JUB_GetHDNodeFIL(IN JUB_UINT16 contextID,
                        IN JUB_ENUM_PUB_FORMAT format,
                        IN BIP32_Path path,
                        OUT JUB_CHAR_PTR_PTR pubkey) {

    JUB_CHECK_CONTEXT_FIL(contextID);

    auto context = (jub::ContextFIL*)jub::ContextManager::GetInstance()->GetOne(contextID);
    JUB_CHECK_NULL(context);

    std::string str_pubkey;
    JUB_VERIFY_RV(context->GetHDNode(format,path, str_pubkey));
    JUB_VERIFY_RV(_allocMem(pubkey, str_pubkey));

    return JUBR_OK;
}


/*****************************************************************************
 * @function name : JUB_GetMainHDNodeFIL
 * @in  param : contextID - context ID
 *            : format - JUB_ENUM_PUB_FORMAT::HEX (0x00) for hex;
 *                       JUB_ENUM_PUB_FORMAT::XPUB(0x01) for xpub
 * @out param : xpub
 * @last change :
 *****************************************************************************/
JUB_RV JUB_GetMainHDNodeFIL(IN JUB_UINT16 contextID,
                            IN JUB_ENUM_PUB_FORMAT format,
                            OUT JUB_CHAR_PTR_PTR xpub) {

    JUB_CHECK_CONTEXT_FIL(contextID);

    auto context = (jub::ContextFIL*)jub::ContextManager::GetInstance()->GetOne(contextID);
    JUB_CHECK_NULL(context);

    std::string str_xpub;
    JUB_VERIFY_RV(context->GetMainHDNode(format, str_xpub));
    JUB_VERIFY_RV(_allocMem(xpub, str_xpub));

    return JUBR_OK;
}


/*****************************************************************************
 * @function name : JUB_SignTransactionFIL
 * @in  param : contextID - context ID
 *          : path
 *          : nonce - nonce
 *          : gasLimit - gas limit
 *          : gasFeeCapInAtto - gas fee cap in attoFIL
 *          : gasPremiumInAtto - gas premium in attoFIL
 *          : to
 *          : valueInAtto - value in attoFIL
 *          : input
 * @out param : raw
 * @last change :
 *****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_SignTransactionFIL(IN JUB_UINT16 contextID,
                              IN BIP32_Path path,
                              IN JUB_UINT64 nonce,
                              IN JUB_UINT64 gasLimit,
                              IN JUB_CHAR_CPTR gasFeeCapInAtto,
                              IN JUB_CHAR_CPTR gasPremiumInAtto,
                              IN JUB_CHAR_CPTR to,
                              IN JUB_CHAR_CPTR valueInAtto,
                              IN JUB_CHAR_CPTR input,
                              OUT JUB_CHAR_PTR_PTR raw) {

    JUB_CHECK_CONTEXT_FIL(contextID);

    auto context = (jub::ContextFIL*)jub::ContextManager::GetInstance()->GetOne(contextID);
    JUB_CHECK_NULL(context);

    std::string str_raw;
    JUB_VERIFY_RV(context->SignTransaction(path,
                                           nonce,
                                           gasLimit,
                                           gasFeeCapInAtto,
                                           gasPremiumInAtto,
                                           to,
                                           valueInAtto,
                                           input,
                                           str_raw));
    JUB_VERIFY_RV(_allocMem(raw, str_raw));

    return JUBR_OK;
}
