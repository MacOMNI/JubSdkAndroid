//
//  JUB_SDK_DEV_BIO.cpp
//  JubSDK
//
//  Created by Pan Min on 2020/7/3.
//  Copyright © 2020 JuBiter. All rights reserved.
//

#include "JUB_SDK_DEV_BIO.h"

#include "utility/util.h"
#include "context/Context.h"

#include "device/JubiterBLEDevice.hpp"
#include "token/JubiterBioImpl.h"
#ifdef __ANDROID__
#include "utils/logUtils.h"
#endif


JUB_RV _allocMem(JUB_CHAR_PTR_PTR memPtr, const std::string &strBuf);


///*****************************************************************************
// * @function name : JUB_IdentityVerify
// * @in  param : deviceID - device ID
// *           mode - the mode for verify identity, the following values are valid:
// *                   - JUB_ENUM_IDENTITY_VERIFY_MODE::VIA_FPGT
// * @out param : retry - if OK, retry is meaningless value
// * @last change :
// *****************************************************************************/
//JUB_COINCORE_DLL_EXPORT
//JUB_RV JUB_IdentityVerify(IN JUB_UINT16 deviceID,
//                          IN JUB_ENUM_IDENTITY_VERIFY_MODE mode,
//                          OUT JUB_ULONG_PTR pretry) {
//
//    auto token = jub::TokenManager::GetInstance()->GetOne(deviceID);
//    JUB_CHECK_NULL(token);
//
//    JUB_ULONG retry = 0;
//    JUB_RV rv = token->IdentityVerify(mode, retry);
//
//    *pretry = retry;
//
//    return rv;
//}
//
//
/*****************************************************************************
 * @function name : JUB_IdentityVerifyPIN
 * @in  param : deviceID - device ID
 *           mode - the mode for verify identity, the following values are valid:
 *                   - JUB_ENUM_IDENTITY_VERIFY_MODE::VIA_9GRIDS
 *           pinMix: user's PIN
 * @out param : retry - if OK, retry is meaningless value
 * @last change :
 *****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_IdentityVerifyPIN(IN JUB_UINT16 deviceID,
                             IN JUB_ENUM_IDENTITY_VERIFY_MODE mode,
                             IN JUB_CHAR_CPTR pinMix,
                             OUT JUB_ULONG_PTR pretry) {

    auto token = jub::TokenManager::GetInstance()->GetOne(deviceID);
    JUB_CHECK_NULL(token);

    JUB_ULONG retry = 0;
    JUB_RV rv = token->IdentityVerifyPIN(mode, pinMix, retry);

    *pretry = retry;

    return rv;
}


/*****************************************************************************
 * @function name : JUB_IdentityShowNineGrids
 * @in  param : deviceID - device ID
 * @out param :
 * @last change :
 *****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_IdentityShowNineGrids(IN JUB_UINT16 deviceID) {

    auto token = jub::TokenManager::GetInstance()->GetOne(deviceID);
    JUB_CHECK_NULL(token);

    JUB_VERIFY_RV(token->IdentityNineGrids(true));

    return JUBR_OK;
}


/*****************************************************************************
 * @function name : JUB_IdentityCancelNineGrids
 * @in  param : deviceID - device ID
 * @out param :
 * @last change :
 *****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_IdentityCancelNineGrids(IN JUB_UINT16 deviceID) {

    auto token = jub::TokenManager::GetInstance()->GetOne(deviceID);
    JUB_CHECK_NULL(token);

    JUB_VERIFY_RV(token->IdentityNineGrids(false));

    JUB_VERIFY_RV(token->UIShowMain());

    return JUBR_OK;
}


/*****************************************************************************
 * @function name : JUB_EnrollFingerprint
 * @in  param : deviceID - device ID
 * @inout param: fgptTimeout - timeout for fingerprint
 *            fgptIndex - The index of current fingerprint modality.
 *                    If this value is ZERO, indicate enroll a new fingerprint;
 *                      otherwise this value mast be equal to the value in
 *                      response of previous this command.
 * @out param : ptimes -  total number of times that need to enroll for current fingerprint modality.
 *            fgptID - modality ID, assigned by the device
 * @last change :
 *****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_EnrollFingerprint(IN JUB_UINT16 deviceID,
                             IN JUB_UINT16 fgptTimeout,
                             INOUT JUB_BYTE_PTR fgptIndex, OUT JUB_ULONG_PTR ptimes,
                             OUT JUB_BYTE_PTR fgptID) {

    auto token = jub::TokenManager::GetInstance()->GetOne(deviceID);
    JUB_CHECK_NULL(token);

    JUB_VERIFY_RV(token->EnrollFingerprint(fgptTimeout,
                                           fgptIndex, ptimes,
                                           fgptID));

    return JUBR_OK;
}


/*****************************************************************************
 * @function name : JUB_EnumFingerprint
 * @in  param : deviceID - device ID
 * @out param :fgptList - fingerprint list
 * @last change :
 *****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_EnumFingerprint(IN JUB_UINT16 deviceID,
                           OUT JUB_CHAR_PTR_PTR fgptList) {

    auto token = jub::TokenManager::GetInstance()->GetOne(deviceID);
    JUB_CHECK_NULL(token);

    std::string str_fgpts;
    JUB_VERIFY_RV(token->EnumFingerprint(str_fgpts));
    JUB_VERIFY_RV(_allocMem(fgptList, str_fgpts));

    return JUBR_OK;
}


/*****************************************************************************
 * @function name : JUB_EraseFingerprint
 * @in  param : deviceID - device ID
 *           fgptTimeout - timeout for fingerprint
 * @out param :
 * @last change :
 *****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_EraseFingerprint(IN JUB_UINT16 deviceID,
                            IN JUB_UINT16 fgptTimeout) {

    auto token = jub::TokenManager::GetInstance()->GetOne(deviceID);
    JUB_CHECK_NULL(token);

    JUB_VERIFY_RV(token->EraseFingerprint(fgptTimeout));

    return JUBR_OK;
}


/*****************************************************************************
 * @function name : JUB_DeleteFingerprint
 * @in  param : deviceID - device ID
 *           fgptTimeout - timeout for fingerprint
 *           fgptID - the modality ID of a fingerprint.
 * @out param :
 * @last change :
 *****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_DeleteFingerprint(IN JUB_UINT16 deviceID,
                             IN JUB_UINT16 fgptTimeout,
                             IN JUB_BYTE fgptID) {

    auto token = jub::TokenManager::GetInstance()->GetOne(deviceID);
    JUB_CHECK_NULL(token);

    JUB_VERIFY_RV(token->DeleteFingerprint(fgptTimeout,
                                           fgptID));

    return JUBR_OK;
}


/*****************************************************************************
 * @function name : JUB_VerifyFingerprint
 * @in  param : contextID - context ID
 * @out param : retry
 * @last change :
 *****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_VerifyFingerprint(IN JUB_UINT16 contextID,
                             OUT JUB_ULONG_PTR pretry) {

    auto context = jub::ContextManager::GetInstance()->GetOne(contextID);
    JUB_CHECK_NULL(context);

    JUB_VERIFY_RV(context->VerifyFingerprint(*pretry));

    return JUBR_OK;
}
