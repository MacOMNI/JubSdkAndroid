//
//  JUB_SDK_DEV_HID.cpp
//  JubSDK
//
//  Created by Pan Min on 2019/7/17.
//  Copyright © 2019 JuBiter. All rights reserved.
//

#include "JUB_SDK_DEV_HID.h"

#include "utility/util.h"

#include "device/JubiterHidDevice.hpp"
#include "token/JubiterBLDBioImpl.h"


/*****************************************************************************
 * @function name : JUB_ListDeviceHid
 * @in  param :
 * @out param : deviceIDs - device ID list
 * @last change :
 *****************************************************************************/
JUB_RV JUB_ListDeviceHid(OUT JUB_UINT16 deviceIDs[MAX_DEVICE]) {

#ifdef HID_MODE
    auto pid_list = {PID_BLD, PID_BIO};
    for (auto pid : pid_list) {

    auto path_list = jub::JubiterHidDevice::EnumDevice(pid);
    //std::cout <<"** "<< path_list.size() << std::endl;

    //deal removed key
    auto vDeviceIDs = jub::TokenManager::GetInstance()->GetHandleList();
    for (JUB_UINT16 i = 0; i < vDeviceIDs.size(); i++) {
        auto token = (jub::JubiterBLDImpl*)jub::TokenManager::GetInstance()->GetOne(vDeviceIDs[i]);
        if (!token) {
            continue;
        }
        bool bCheckTypeid = PID_BLD == pid ?
        (typeid(jub::JubiterBLDImpl)    == typeid(*token)) :
        (typeid(jub::JubiterBLDBioImpl) == typeid(*token));
        if (std::end(path_list) == std::find(std::begin(path_list), std::end(path_list), token->getPath())
            && bCheckTypeid
            ) {
            //removed key
            jub::TokenManager::GetInstance()->ClearOne(vDeviceIDs[i]);
        }
    }

    //deal inserted key
    auto isInManager = [](const std::string& path, const unsigned short pid)-> bool {
        auto vDeviceIDs = jub::TokenManager::GetInstance()->GetHandleList();
        for (JUB_UINT16 i = 0; i < vDeviceIDs.size(); i++) {
            auto token = (jub::JubiterBLDImpl*)jub::TokenManager::GetInstance()->GetOne(vDeviceIDs[i]);
            if (!token) {
                continue;
            }
            bool bCheckTypeid = PID_BLD == pid ?
            (typeid(jub::JubiterBLDImpl)    == typeid(*token)) :
            (typeid(jub::JubiterBLDBioImpl) == typeid(*token));
            if (   token
                && path == token->getPath()
                && bCheckTypeid
                ) {
                return true;
            }
        }

        return false;
    };

    for (auto path : path_list) {
        if (!isInManager(path, pid)) {
            //new inserted key
            switch (pid) {
            case PID_BLD:
            {
                jub::JubiterBLDImpl* token = new jub::JubiterBLDImpl(path);
                jub::TokenManager::GetInstance()->AddOne(token);
                break;
            }
            case PID_BIO:
            {
                jub::JubiterBLDBioImpl* token = new jub::JubiterBLDBioImpl(path);
                jub::TokenManager::GetInstance()->AddOne(token);
                break;
            }
            default:
                break;
            }
        }
    }
    }   // for (auto pid : pid_list) end

    auto _vDeviceIDs = jub::TokenManager::GetInstance()->GetHandleList();
    for (JUB_UINT16 i = 0 ; i < std::min((size_t)MAX_DEVICE, _vDeviceIDs.size()); i++) {
        deviceIDs[i] = _vDeviceIDs[i];
    }

    return JUBR_OK;
#else
    return JUBR_IMPL_NOT_SUPPORT;
#endif // #ifdef HID_MODE
}


/*****************************************************************************
 * @function name : JUB_ConnetDeviceHid
 * @in  param : deviceID - device ID
 * @out param :
 * @last change :
 *****************************************************************************/
JUB_RV JUB_ConnetDeviceHid(IN JUB_UINT16 deviceID) {

#ifdef HID_MODE
    auto token = jub::TokenManager::GetInstance()->GetOne(deviceID);
    JUB_CHECK_NULL(token);

    JUB_VERIFY_RV(token->ConnectToken());

    return JUBR_OK;
#else
    return JUBR_IMPL_NOT_SUPPORT;
#endif // #ifdef HID_MODE
}


/*****************************************************************************
 * @function name : JUB_DisconnetDeviceHid
 * @in  param : deviceID - device ID
 * @out param :
 * @last change :
 *****************************************************************************/
JUB_RV JUB_DisconnetDeviceHid(IN JUB_UINT16 deviceID) {

#ifdef HID_MODE
    auto token = jub::TokenManager::GetInstance()->GetOne(deviceID);
    JUB_CHECK_NULL(token);

    JUB_VERIFY_RV(token->DisconnectToken());

    return JUBR_OK;
#else
    return JUBR_IMPL_NOT_SUPPORT;
#endif // #ifdef HID_MODE
}
