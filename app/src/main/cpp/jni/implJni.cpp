//
// Created by FT on 2018/4/16.
//

#include <jni.h>
#include <logUtils.h>
#include <utils.h>
#include <vector>
#include <json/json.h>
#include <implJni.h>

#ifdef HC

#include <JUB_SDK_Hcash.h>
#include <JUB_SDK_XRP.h>
#include <utility/Debug.hpp>
#include <JUB_SDK_DEV_BIO.h>

#else
#include <JUB_SDK.h>
#endif

// 保存 JavaVM
JavaVM *g_vm = NULL;
int errorCode = 0;

// 是否是多重签名
bool globalMultiSig = false;

JNIEXPORT jint JNICALL native_getErrorCode(JNIEnv *env, jclass obj) {
    return errorCode;
}

//================================= 蓝牙 ================================================

JNIEXPORT jint JNICALL native_initDevice(JNIEnv *env, jclass obj) {
    LOG_ERR(">>>> in native_initDevice");

    DEVICE_INIT_PARAM initParam;

    // 初始化参数转换
    jobjectToInitParam(env, g_vm, &initParam);

    JUB_RV rv = JUB_initDevice(initParam);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_initDevice ret: %08x", rv);
        return rv;
    }
    return rv;
}

JNIEXPORT jint JNICALL native_startScan(JNIEnv *env, jclass obj, jobject scanCallback) {
    jobject objParam = env->NewGlobalRef(scanCallback);
    setScanCallbackObj(g_vm, objParam);

    JUB_RV rv = JUB_enumDevices();
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_enumDevices rv: %08x", rv);
        return rv;
    }
    return rv;
}

JNIEXPORT jint JNICALL native_stopScan(JNIEnv *env, jclass obj) {
    JUB_RV rv = JUB_stopEnumDevices();
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_stopEnumDevices rv: %08x", rv);
        return rv;
    }
    return rv;
}

JNIEXPORT jint JNICALL native_connectDevice(JNIEnv *env, jclass obj, jstring devName, jstring address, jint devType,
                                            jlongArray handle, jint timeout, jobject disCallback) {
    JUB_BYTE_PTR pAddress = (JUB_BYTE_PTR) (env->GetStringUTFChars(address, NULL));
    JUB_BYTE_PTR pDevName = (JUB_BYTE_PTR) (env->GetStringUTFChars(devName, NULL));
    JUB_UINT16 *pHandle = reinterpret_cast<JUB_UINT16 *>(env->GetLongArrayElements(handle, NULL));

    jobject objParam = env->NewGlobalRef(disCallback);
    setDiscCallbackObj(g_vm, objParam);


    JUB_RV rv = JUB_connectDevice(pDevName, pAddress, devType, pHandle, timeout);

    env->ReleaseLongArrayElements(handle, reinterpret_cast<jlong *>(pHandle), 0);
    env->ReleaseStringUTFChars(devName, reinterpret_cast<const char *>(pDevName));
    env->ReleaseStringUTFChars(address, reinterpret_cast<const char *>(pAddress));
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_connectDevice rv: %08x", rv);

        return rv;
    }
    return rv;
}


JNIEXPORT jint JNICALL native_cancelConnect(JNIEnv *env, jobject obj,  jstring devName, jstring address) {
    JUB_BYTE_PTR pAddress = (JUB_BYTE_PTR) env->GetStringUTFChars(address, NULL);
    JUB_BYTE_PTR pDevName = (JUB_BYTE_PTR) env->GetStringUTFChars(devName, NULL);

    JUB_RV rv = JUB_cancelConnect(pDevName,pAddress);

    env->ReleaseStringUTFChars(address, reinterpret_cast<const char *>(pAddress));
    env->ReleaseStringUTFChars(devName, reinterpret_cast<const char *>(pDevName));
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_cancelConnect rv: %08x", rv);
    }
    return rv;
}


JNIEXPORT jint JNICALL native_disconnectDevice(JNIEnv *env, jclass obj, jlong deviceHandle) {
    JUB_RV rv = JUB_disconnectDevice(static_cast<JUB_UINT16>(deviceHandle));
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_disconnectDevice rv: %08x", rv);
    }
    return rv;
}


JNIEXPORT jint JNICALL native_isConnectDevice(JNIEnv *env, jclass obj, jlong deviceHandle) {
    JUB_RV rv = JUB_isDeviceConnect(static_cast<JUB_UINT16>(deviceHandle));
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_isDeviceConnect rv: %08x", rv);
    }
    return rv;
}

//================================= 功能 ================================================

JNIEXPORT jstring JNICALL native_getDeviceType(JNIEnv *env, jclass obj, jlong deviceID) {
    JUB_ENUM_COMMODE pComMode;
    JUB_ENUM_DEVICE pDevice;
    JUB_UINT16 deviceId = deviceID;
    JUB_RV rv = JUB_GetDeviceType(deviceId, &pComMode, &pDevice);
    if (rv != JUBR_OK) {
        errorCode = rv;
        LOG_ERR("JUB_GetDeviceType rv: %08x", rv);
        return NULL;
    }
    Json::FastWriter writer;
    Json::Value root;
    root["COMMODE"] = pComMode;
    root["DEVICE"] = pDevice;
    jstring result = env->NewStringUTF(writer.write(root).c_str());
    return result;
}

JNIEXPORT jint JNICALL native_show(JNIEnv *env, jclass obj, jlong contextID) {
    JUB_RV rv = JUB_ShowVirtualPwd(static_cast<JUB_UINT16>(contextID));
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_showVirtualPwd rv: %08x", rv);
    }
    return rv;
}

JNIEXPORT jint JNICALL native_CancelVirtualPwd(JNIEnv *env, jclass obj, jlong contextID) {
    JUB_RV rv = JUB_CancelVirtualPwd(static_cast<JUB_UINT16>(contextID));
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_CancelVirtualPwd rv: %08x", rv);
    }
    return rv;
}


JNIEXPORT jint JNICALL
native_verifyPIN(JNIEnv *env, jclass obj, jlong contextID, jbyteArray jPin) {
    JUB_CHAR_PTR pPin = (JUB_CHAR_PTR) (env->GetByteArrayElements(jPin, NULL));
    int length = env->GetArrayLength(jPin);

    // java数组没有结束符，jni层需补充
    *(pPin + length) = '\0';

    JUB_ULONG retry;
    JUB_RV ret = JUB_VerifyPIN(static_cast<JUB_UINT16>(contextID), pPin, &retry);
    if (ret != JUBR_OK) {
        LOG_ERR("JUB_VerifyPIN: %08x", ret);
    }
    env->ReleaseByteArrayElements(jPin, (jbyte *) pPin, JNI_ABORT);
    return ret;
}

JNIEXPORT jint JNICALL native_GetDeviceInfo(JNIEnv *env, jclass obj,
                                            jobject deviceInfo, jlong deviceHandle) {

#define SET_SN               "setSn"
#define SET_LABEL            "setLabel"
#define SETPIN_RETRY         "setPin_retry"
#define SETPIN_MAX_RETRY     "setPin_max_retry"
#define SETBLE_VERSION       "setBle_version"
#define SET_FIRMWARE_VERSION "setFirmware_version"

    JUB_DEVICE_INFO info;
    JUB_RV rv = JUB_GetDeviceInfo((JUB_UINT16) deviceHandle, &info);
    if (rv == JUBR_OK) {
        jclass clazz = env->GetObjectClass(deviceInfo);
        jmethodID setSN = env->GetMethodID(clazz, SET_SN, "(Ljava/lang/String;)V");
        env->CallVoidMethod(deviceInfo, setSN, env->NewStringUTF(info.sn));

        jmethodID setLabel = env->GetMethodID(clazz, SET_LABEL, "(Ljava/lang/String;)V");
        env->CallVoidMethod(deviceInfo, setLabel, env->NewStringUTF(info.label));

        jmethodID pin_retry = env->GetMethodID(clazz, SETPIN_RETRY, "(I)V");
        env->CallVoidMethod(deviceInfo, pin_retry, info.pinRetry);

        jmethodID pin_max_retry = env->GetMethodID(clazz, SETPIN_MAX_RETRY, "(I)V");
        env->CallVoidMethod(deviceInfo, pin_max_retry, info.pinMaxRetry);

        char tmpVersion[5] = {0,};
        memcpy(tmpVersion, info.bleVersion, 4);
        jmethodID ble_version = env->GetMethodID(clazz, SETBLE_VERSION, "(Ljava/lang/String;)V");
        jstring version = env->NewStringUTF(tmpVersion);
        env->CallVoidMethod(deviceInfo, ble_version, version);

        char tmpFirewareVersion[5] = {0,};
        memcpy(tmpFirewareVersion, info.firmwareVersion, 4);
        jmethodID firmware_version = env->GetMethodID(clazz, SET_FIRMWARE_VERSION,
                                                      "(Ljava/lang/String;)V");
        jstring firmware_versionStr = env->NewStringUTF(tmpFirewareVersion);
        env->CallVoidMethod(deviceInfo, firmware_version, firmware_versionStr);
    }
    return rv;
};

JNIEXPORT jstring JNICALL native_sendAPDU(JNIEnv *env, jclass obj, jlong deviceID,
                                          jstring jApdu) {
    JUB_CHAR_PTR pApdu = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jApdu, NULL));
    JUB_CHAR_PTR response = nullptr;
    JUB_RV ret = JUB_SendOneApdu(static_cast<JUB_UINT16>(deviceID), pApdu, &response);
    if (ret == JUBR_OK) {
        jstring result = env->NewStringUTF(response);
        JUB_FreeMemory(response);
        return result;
    } else {
        return NULL;
    }
}

JNIEXPORT jstring JNICALL native_GetDeviceCert(JNIEnv *env, jclass obj, jlong deviceHandle) {
    JUB_CHAR_PTR cert = NULL;
    JUB_RV rv = JUB_GetDeviceCert((JUB_UINT16) deviceHandle, &cert);
    if (rv == JUBR_OK) {
        jstring result = env->NewStringUTF(cert);
        JUB_FreeMemory(cert);
        return result;
    } else {
        LOG_ERR("JUB_GetDeviceCert error");
        return NULL;
    }
}

JNIEXPORT jstring JNICALL
native_GetAppletVersion(JNIEnv *env, jclass obj, jlong deviceHandle, jstring appID) {
    JUB_CHAR_PTR pAppID = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(appID, NULL));
    JUB_CHAR_PTR appVersion = NULL;
    JUB_RV rv = JUB_GetAppletVersion((JUB_UINT16) deviceHandle, pAppID, &appVersion);
    if (rv == JUBR_OK) {
        jstring result = env->NewStringUTF(appVersion);
        JUB_FreeMemory(appVersion);
        return result;
    } else {
        LOG_ERR("JUB_GetAppletVersion error");
        return NULL;
    }
}

JNIEXPORT jstring JNICALL native_EnumApplets(JNIEnv *env, jclass obj, jlong deviceHandle) {

    JUB_CHAR_PTR list = NULL;
    JUB_RV rv = JUB_EnumApplets((JUB_UINT16) deviceHandle, &list);
    if (rv == JUBR_OK) {
        jstring result = env->NewStringUTF(list);
        JUB_FreeMemory(list);
        return result;
    } else {
        LOG_ERR("JUB_EnumApplets error");
        return NULL;
    }
}

JNIEXPORT jint JNICALL native_SetTimeOut(JNIEnv *env, jclass obj, jlong contextID, jint jTimeOut) {
    return static_cast<jint>(JUB_SetTimeOut(static_cast<JUB_UINT16>(contextID),
                                            static_cast<JUB_UINT16>(jTimeOut)));
}

//===================================== BTC ============================================

JNIEXPORT jint JNICALL
native_BTCCreateContext(JNIEnv *env, jclass obj, jintArray jContextId, jboolean isMultiSig,
                        jstring jJSON,
                        jlong deviceInfo) {

#define MAIN_PATH      "main_path"
#define P2SH_SEGWIT    "p2sh_segwit"
#define M              "m"
#define N              "n"
#define MASTER_KEY     "cosigner"
#define COIN_TYPE_BTC  "coin_type"

    if (NULL == jJSON) {
        return JUBR_ARGUMENTS_BAD;
    }

    int length = env->GetStringLength(jJSON);
    if (0 == length) {
        errorCode = JUBR_ARGUMENTS_BAD;
        return JUBR_ARGUMENTS_BAD;
    }

    JUB_UINT16 *pContextID = (JUB_UINT16 *) env->GetIntArrayElements(jContextId, NULL);
    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, NULL));

    Json::Reader reader;
    Json::Value root;
    reader.parse(pJSON, root);

    CONTEXT_CONFIG_BTC cfg;
    CONTEXT_CONFIG_MULTISIG_BTC multiCfg;
    cfg.mainPath = (char *) root[MAIN_PATH].asCString();
    int cointype = root[COIN_TYPE_BTC].asInt();
    switch (cointype) {
        case 0x00:
            cfg.coinType = COINBTC;
            break;
        case 0x01:
            cfg.coinType = COINBCH;
            break;
        case 0x02:
            cfg.coinType = COINLTC;
            break;
        case 0x03:
            cfg.coinType = COINUSDT;
            break;
        case 0x04:
            cfg.coinType = COINDASH;
            break;
        case 0x05:
            cfg.coinType = COINQTUM;
            break;
        case 0x06:
            cfg.coinType = COINDOGE;
            break;
        default:
            cfg.coinType = COINBTC;
            break;
    }

    // 缓存是否是多重签名标记
    globalMultiSig = isMultiSig;

    JUB_RV rv = JUBR_OK;
    if (isMultiSig) {
        multiCfg.transType = p2sh_multisig;
        multiCfg.mainPath = cfg.mainPath;
        multiCfg.coinType = cfg.coinType;
        multiCfg.m = root[M].asInt64();
        multiCfg.n = root[N].asInt64();

        int keySize = root[MASTER_KEY].size();
        std::vector<std::string> masterKey;
        for (int i = 0; i < keySize; ++i) {
            std::string key = root[MASTER_KEY][i].asString();
            masterKey.push_back(key);
        }
        multiCfg.vCosignerMainXpub = masterKey;

        rv = JUB_CreateContextBTC(&multiCfg, deviceInfo, pContextID);
    } else {
        if (COINBCH == cfg.coinType) {
            cfg.transType = p2pkh;
        } else {
            if (root[P2SH_SEGWIT].asBool()) {
                cfg.transType = p2sh_p2wpkh;
            } else {
                cfg.transType = p2pkh;
            }
        }
        rv = JUB_CreateContextBTC(&cfg, deviceInfo, pContextID);
    }

    if (rv != JUBR_OK) {
        LOG_ERR("JUB_CreateContextBTC: %08x", rv);
        errorCode = rv;
    } else {
        LOG_INF("contextID: %d", *pContextID);
    }
    env->ReleaseIntArrayElements(jContextId, (jint *) pContextID, 0);
    return rv;
}


JNIEXPORT jint JNICALL native_ClearContext(JNIEnv *env, jclass obj, jlong jContextId) {
    JUB_RV rv = JUB_ClearContext(jContextId);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_ClearContext: %08x", rv);
        errorCode = rv;
    }
    return rv;
}

JNIEXPORT jstring JNICALL native_USDTTransaction(JNIEnv *env, jclass obj,
                                                 jlong contextID, jstring jJSON) {
#define INPUTS       "inputs"
#define PREHASH      "preHash"
#define PREINDEX     "preIndex"
#define BIP32_PATH   "bip32_path"
#define CHANGE       "change"
#define INDEX        "addressIndex"
#define AMOUNT       "amount"

#define OUTPUTS      "outputs"
#define ADDRESS      "address"
#define CHANGE_ADDRESS "change_address"

#define USDT_AMOUNT "USDT_amount"
#define USDT_TO     "USDT_to"


    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, NULL));
    Json::Reader reader;
    Json::Value root;
    reader.parse(pJSON, root);

    std::vector<INPUT_BTC> inputs;
    std::vector<OUTPUT_BTC> outputs;

    int input_number = root[INPUTS].size();
    for (int i = 0; i < input_number; i++) {
        INPUT_BTC input;
        input.preHash = (char *) root[INPUTS][i][PREHASH].asCString();
        input.preIndex = static_cast<JUB_UINT16>(root[INPUTS][i][PREINDEX].asInt());
        input.path.change = (JUB_ENUM_BOOL) root[INPUTS][i][BIP32_PATH][CHANGE].asBool();
        input.path.addressIndex = static_cast<JUB_UINT64>(root[INPUTS][i][BIP32_PATH][INDEX].asInt());
        input.amount = static_cast<JUB_UINT64>(root[INPUTS][i][AMOUNT].asUInt64());
        inputs.push_back(input);
    }

    int output_number = root[OUTPUTS].size();
    for (int i = 0; i < output_number; i++) {
        OUTPUT_BTC output;
        output.stdOutput.address = (char *) root[OUTPUTS][i][ADDRESS].asCString();
        output.stdOutput.amount = static_cast<JUB_UINT64>(root[OUTPUTS][i][AMOUNT].asUInt64());
        output.stdOutput.changeAddress = (JUB_ENUM_BOOL) root[OUTPUTS][i][CHANGE_ADDRESS].asBool();
        if (output.stdOutput.changeAddress) {
            output.stdOutput.path.change = (JUB_ENUM_BOOL) root[OUTPUTS][i][BIP32_PATH][CHANGE].asBool();
            output.stdOutput.path.addressIndex = static_cast<JUB_UINT64>(root[OUTPUTS][i][BIP32_PATH][INDEX].asInt());
        }
        outputs.push_back(output);
    }

    OUTPUT_BTC USDT_outputs[2] = {};

    char *usdt_to = const_cast<char *>(root[USDT_TO].asCString());
    JUB_RV rv = JUB_BuildUSDTOutputs(
            static_cast<JUB_UINT16>(contextID),
            usdt_to,
            root[USDT_AMOUNT].asUInt64(),
            USDT_outputs);
    if (rv != JUBR_OK) {
        errorCode = static_cast<int>(rv);
        return NULL;
    }

    outputs.emplace_back(USDT_outputs[0]);
    outputs.emplace_back(USDT_outputs[1]);

    JUB_SetUnitBTC(static_cast<JUB_UINT16>(contextID), BTC);
    char *raw = NULL;
    rv = JUB_SignTransactionBTC(static_cast<JUB_UINT16>(contextID),
                                JUB_ENUM_BTC_ADDRESS_FORMAT::OWN,
                                &inputs[0],
                                (JUB_UINT16) inputs.size(),
                                &outputs[0], (JUB_UINT16) outputs.size(), 0, &raw);

    // JUBR_MULTISIG_OK 表示多重签名结果不完整，需要后续再次签名
    if (rv != JUBR_OK && rv != JUBR_MULTISIG_OK) {
        errorCode = static_cast<int>(rv);
        return NULL;
    }
    // 用于判断是否多签完成
    errorCode = rv;
    jstring rawString = env->NewStringUTF(raw);
    JUB_FreeMemory(raw);
    return rawString;
}

JNIEXPORT jstring JNICALL native_QRC20Transaction(JNIEnv *env, jclass obj,
                                                  jlong contextID, jstring jJSON) {
#define INPUTS       "inputs"
#define PREHASH      "preHash"
#define PREINDEX     "preIndex"
#define BIP32_PATH   "bip32_path"
#define CHANGE       "change"
#define INDEX        "addressIndex"
#define AMOUNT       "amount"

#define OUTPUTS      "outputs"
#define ADDRESS      "address"
#define CHANGE_ADDRESS "change_address"

#define CONTRACT_ADDRESS "QRC20_contractAddr"
#define DECIMAL "QRC20_decimal"
#define SYMBOL "QRC20_symbol"
#define GAS_LIMIT "gasLimit"
#define GAS_PRICE "gasPrice"
#define TOKEN_TO "QRC20_to"
#define TOKEN_VALUE "QRC20_amount"


    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, NULL));
    Json::Reader reader;
    Json::Value root;
    reader.parse(pJSON, root);

    OUTPUT_BTC QRC20_outputs[1] = {};
    JUB_BuildQRC20Outputs(
            static_cast<JUB_UINT16>(contextID),
            const_cast<JUB_CHAR_PTR>(root[CONTRACT_ADDRESS].asCString()),
            static_cast<JUB_UINT8>(root[DECIMAL].asInt()),
            const_cast<JUB_CHAR_PTR>(root[SYMBOL].asCString()),
            static_cast<JUB_UINT64>(root[GAS_LIMIT].asInt()),
            static_cast<JUB_UINT64>(root[GAS_PRICE].asInt()),
            const_cast<JUB_CHAR_PTR>(root[TOKEN_TO].asCString()),
            const_cast<JUB_CHAR_PTR>(root[TOKEN_VALUE].asCString()),
            QRC20_outputs);

    std::vector<INPUT_BTC> inputs;
    std::vector<OUTPUT_BTC> outputs;

    int input_number = root[INPUTS].size();
    for (int i = 0; i < input_number; i++) {
        INPUT_BTC input;
        input.preHash = (char *) root[INPUTS][i][PREHASH].asCString();
        input.preIndex = static_cast<JUB_UINT16>(root[INPUTS][i][PREINDEX].asInt());
        input.path.change = (JUB_ENUM_BOOL) root[INPUTS][i][BIP32_PATH][CHANGE].asBool();
        input.path.addressIndex = static_cast<JUB_UINT64>(root[INPUTS][i][BIP32_PATH][INDEX].asUInt64());
        input.amount = static_cast<JUB_UINT64>(root[INPUTS][i][AMOUNT].asInt());
        inputs.push_back(input);
    }

    int output_number = root[OUTPUTS].size();
    for (int i = 0; i < output_number; i++) {
        OUTPUT_BTC output;
        output.stdOutput.address = (char *) root[OUTPUTS][i][ADDRESS].asCString();
        output.stdOutput.amount = static_cast<JUB_UINT64>(root[OUTPUTS][i][AMOUNT].asUInt64());
        output.stdOutput.changeAddress = (JUB_ENUM_BOOL) root[OUTPUTS][i][CHANGE_ADDRESS].asBool();
        if (output.stdOutput.changeAddress) {
            output.stdOutput.path.change = (JUB_ENUM_BOOL) root[OUTPUTS][i][BIP32_PATH][CHANGE].asBool();
            output.stdOutput.path.addressIndex = static_cast<JUB_UINT64>(root[OUTPUTS][i][BIP32_PATH][INDEX].asInt());
        }
        outputs.push_back(output);
    }
    outputs.push_back(QRC20_outputs[0]);

    JUB_SetUnitBTC(static_cast<JUB_UINT16>(contextID), BTC);
    char *raw = NULL;
    JUB_RV rv = JUB_SignTransactionBTC(static_cast<JUB_UINT16>(contextID),
                                       JUB_ENUM_BTC_ADDRESS_FORMAT::OWN,
                                       &inputs[0],
                                       (JUB_UINT16) inputs.size(),
                                       &outputs[0], (JUB_UINT16) outputs.size(), 0, &raw);

    // JUBR_MULTISIG_OK 表示多重签名结果不完整，需要后续再次签名
    if (rv != JUBR_OK && rv != JUBR_MULTISIG_OK) {
        errorCode = static_cast<int>(rv);
        return NULL;
    }
    // 用于判断是否多签完成
    errorCode = rv;
    jstring rawString = env->NewStringUTF(raw);
    JUB_FreeMemory(raw);
    return rawString;

}


JNIEXPORT jstring JNICALL
native_BTC_ShowAddress(JNIEnv *env, jclass obj, jlong contextID, jint change, jint index,
                       jboolean useLegacyAddress) {

    jclass clazz = env->FindClass("java/lang/String");
    if (clazz == NULL) {
        LOG_ERR("clazz == NULL");
        return NULL;
    }

    BIP32_Path path;
    path.change = JUB_ENUM_BOOL(change);
    path.addressIndex = static_cast<JUB_UINT64>(index);

    JUB_ENUM_BTC_ADDRESS_FORMAT addrFmt = JUB_ENUM_BTC_ADDRESS_FORMAT::OWN;
    if (useLegacyAddress) {
        addrFmt = JUB_ENUM_BTC_ADDRESS_FORMAT::LEGACY;
    }

    JUB_CHAR_PTR pAddress = NULL;
    JUB_RV rv = JUB_GetAddressBTC(static_cast<JUB_UINT16>(contextID), addrFmt, path, BOOL_TRUE,
                                  &pAddress);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_GetAddressBTC: %08x", rv);
        errorCode = static_cast<int>(rv);
        return NULL;
    }
    jstring address = env->NewStringUTF(pAddress);
    JUB_FreeMemory(pAddress);
    return address;
}

JNIEXPORT jstring JNICALL
native_BTC_SetMyAddress(JNIEnv *env, jclass obj, jlong contextID, jint change, jint index,
                        jboolean useLegacyAddress) {

    jclass clazz = env->FindClass("java/lang/String");
    if (clazz == NULL) {
        LOG_ERR("clazz == NULL");
        return NULL;
    }

    BIP32_Path path;
    path.change = JUB_ENUM_BOOL(change);
    path.addressIndex = static_cast<JUB_UINT64>(index);

    JUB_ENUM_BTC_ADDRESS_FORMAT addrFmt = JUB_ENUM_BTC_ADDRESS_FORMAT::OWN;
    if (useLegacyAddress) {
        addrFmt = JUB_ENUM_BTC_ADDRESS_FORMAT::LEGACY;
    }

    JUB_CHAR_PTR pAddress = NULL;
    JUB_RV rv = JUB_SetMyAddressBTC(static_cast<JUB_UINT16>(contextID), addrFmt, path, &pAddress);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_SetMyAddressBTC: %08x", rv);
        errorCode = static_cast<int>(rv);
        return NULL;
    }
    jstring address = env->NewStringUTF(pAddress);
    JUB_FreeMemory(pAddress);
    return address;
}

JNIEXPORT jobjectArray JNICALL
native_BTCGetAddress(JNIEnv *env, jclass obj, jlong contextID, jboolean useLegacyAddress,
                     jstring jJSON) {

#define BIP32_PATH   "bip32_path"
#define CHANGE       "change"
#define INDEX        "addressIndex"

    jclass clazz = env->FindClass("java/lang/String");
    if (clazz == NULL) {
        LOG_ERR("clazz == NULL");
        return NULL;
    }
    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, FALSE));

    Json::Reader reader;
    Json::Value root;
    reader.parse(pJSON, root);

    int input_number = root[BIP32_PATH].size();
    jobjectArray array = env->NewObjectArray(2 * input_number, clazz, 0);
    for (int i = 0; i < input_number; i++) {
        JUB_CHAR_PTR xpub;

        BIP32_Path path;
        path.change = (JUB_ENUM_BOOL) root[BIP32_PATH][i][CHANGE].asBool();
        path.addressIndex = static_cast<JUB_UINT64>(root[BIP32_PATH][i][INDEX].asInt());

        JUB_RV rv = JUB_GetHDNodeBTC(contextID, path, &xpub);
        if (rv != JUBR_OK) {
            LOG_ERR("JUB_GetHDNodeBTC: %08x", rv);
            env->SetObjectArrayElement(array, 2 * i, NULL);
            env->SetObjectArrayElement(array, 2 * i + 1, NULL);
        } else {
            jstring jsXpub = env->NewStringUTF(xpub);

            JUB_ENUM_BTC_ADDRESS_FORMAT addrFmt = JUB_ENUM_BTC_ADDRESS_FORMAT::OWN;
            if (useLegacyAddress) {
                addrFmt = JUB_ENUM_BTC_ADDRESS_FORMAT::LEGACY;
            }

            JUB_CHAR_PTR pAddress = NULL;
            rv = JUB_GetAddressBTC(contextID, addrFmt, path, BOOL_FALSE, &pAddress);
            if (rv != JUBR_OK) {
                LOG_ERR("JUB_GetAddressBTC: %08x", rv);
                env->SetObjectArrayElement(array, 2 * i, jsXpub);
                env->SetObjectArrayElement(array, 2 * i + 1, NULL);
            } else {
                jstring address = env->NewStringUTF(pAddress);
                env->SetObjectArrayElement(array, 2 * i, jsXpub);
                env->SetObjectArrayElement(array, 2 * i + 1, address);
            }
        }
    }
    return array;
}

JNIEXPORT jstring JNICALL native_BTCGetMainHDNode(JNIEnv *env, jclass obj, jlong contextID) {

    jclass clazz = env->FindClass("java/lang/String");
    if (clazz == NULL) {
        LOG_ERR("clazz == NULL");
        return NULL;
    }

    JUB_CHAR_PTR xpub;
    JUB_RV rv = JUB_GetMainHDNodeBTC(contextID, &xpub);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_GetHDNodeBTC: %08x", rv);
        return NULL;
    }
    jstring mainPub = env->NewStringUTF(xpub);
    JUB_FreeMemory(xpub);
    return mainPub;
}


jstring native_ParseTransactionRaw(JNIEnv *env, jclass obj, jlong contextID, jstring jRaw) {

    JUB_CHAR_PTR pRaw = (JUB_CHAR_PTR) env->GetStringUTFChars(jRaw, NULL);

    JUB_UINT32 lockTime;
    std::vector<INPUT_BTC> btcInputVector;
    std::vector<OUTPUT_BTC> btcOutputVector;
    JUB_RV rv = JUB_ParseTransactionBTC(contextID, pRaw, btcInputVector, btcOutputVector,
                                        &lockTime);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_ParseTransactionBTC: %08x", rv);
        errorCode = rv;
        return NULL;
    }

    Json::Value root;
    Json::Value inputArray;
    for (int i = 0; i < btcInputVector.size(); ++i) {
        INPUT_BTC inputBtc = btcInputVector[i];
        Json::Value input;
        input["multisig"] = inputBtc.type;
        input["preHash"] = inputBtc.preHash;
        input["preIndex"] = inputBtc.preIndex;
#if defined(__aarch64__)
        Json::UInt64 tmstp = inputBtc.amount;
        input["amount"] = tmstp;
#else
        input["amount"] = inputBtc.amount;
#endif
        input["nSequence"] = inputBtc.nSequence;
        inputArray.append(input);
    }
    root["inputs"] = inputArray;

    Json::Value outputArray;
    for (int i = 0; i < btcOutputVector.size(); ++i) {
        OUTPUT_BTC outputBtc = btcOutputVector[i];
        Json::Value output;
        output["multisig"] = outputBtc.type;
#if defined(__aarch64__)
        Json::UInt64 tmoAmount = outputBtc.stdOutput.amount;
        output["amount"] = tmoAmount;
#else
        output["amount"] = outputBtc.stdOutput.amount;
#endif
        output["address"] =
                outputBtc.stdOutput.address == nullptr ? "" : outputBtc.stdOutput.address;
        outputArray.append(output);
    }
    root["output"] = outputArray;
    root["lock_time"] = lockTime;

    std::string rawString = root.toStyledString();
    return env->NewStringUTF(rawString.c_str());
}


JNIEXPORT jstring JNICALL
native_BTCTransaction(JNIEnv *env, jclass obj, jlong contextID, jboolean useLegacyAddress,
                      jstring jJSON) {
#define INPUTS       "inputs"
#define PREHASH      "preHash"
#define PREINDEX     "preIndex"
#define BIP32_PATH   "bip32_path"
#define CHANGE       "change"
#define INDEX        "addressIndex"
#define AMOUNT       "amount"
#define MULTISIG     "multisig"

#define OUTPUTS      "outputs"
#define ADDRESS      "address"
#define CHANGE_ADDRESS "change_address"

    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, NULL));
    Json::Reader reader;
    Json::Value root;
    reader.parse(pJSON, root);

    std::vector<INPUT_BTC> inputs;
    std::vector<OUTPUT_BTC> outputs;

    int input_number = root[INPUTS].size();
    for (int i = 0; i < input_number; i++) {
        INPUT_BTC input;
        // 根据全局变量赋值
        input.type = globalMultiSig ? P2SH_MULTISIG : P2PKH;
//        input.type = SCRIPT_BTC_TYPE(root[INPUTS][i][MULTISIG].asInt());
        input.preHash = (char *) root[INPUTS][i][PREHASH].asCString();
        input.preIndex = static_cast<JUB_UINT16>(root[INPUTS][i][PREINDEX].asInt());
        input.path.change = (JUB_ENUM_BOOL) root[INPUTS][i][BIP32_PATH][CHANGE].asBool();
        input.path.addressIndex = static_cast<JUB_UINT64>(root[INPUTS][i][BIP32_PATH][INDEX].asInt());
        input.amount = static_cast<JUB_UINT64>(root[INPUTS][i][AMOUNT].asUInt64());
        inputs.push_back(input);
    }

    int output_number = root[OUTPUTS].size();
    for (int i = 0; i < output_number; i++) {
        OUTPUT_BTC output;
        // 根据全局变量赋值
        output.type = globalMultiSig ? P2SH_MULTISIG : P2PKH;
//        output.type = SCRIPT_BTC_TYPE(root[OUTPUTS][i][MULTISIG].asInt());
        output.stdOutput.address = (char *) root[OUTPUTS][i][ADDRESS].asCString();
        output.stdOutput.amount = static_cast<JUB_UINT64>(root[OUTPUTS][i][AMOUNT].asUInt64());
        output.stdOutput.changeAddress = (JUB_ENUM_BOOL) root[OUTPUTS][i][CHANGE_ADDRESS].asBool();
        if (output.stdOutput.changeAddress) {
            output.stdOutput.path.change = (JUB_ENUM_BOOL) root[OUTPUTS][i][BIP32_PATH][CHANGE].asBool();
            output.stdOutput.path.addressIndex = static_cast<JUB_UINT64>(root[OUTPUTS][i][BIP32_PATH][INDEX].asInt());
        }
        outputs.push_back(output);
    }

    char *raw = NULL;
    JUB_SetUnitBTC(static_cast<JUB_UINT16>(contextID), BTC);

    JUB_ENUM_BTC_ADDRESS_FORMAT addrFmt = JUB_ENUM_BTC_ADDRESS_FORMAT::OWN;
    if (useLegacyAddress) {
        addrFmt = JUB_ENUM_BTC_ADDRESS_FORMAT::LEGACY;
    }

    JUB_RV rv = JUB_SignTransactionBTC(static_cast<JUB_UINT16>(contextID), addrFmt,
                                       &inputs[0], (JUB_UINT16) inputs.size(),
                                       &outputs[0], (JUB_UINT16) outputs.size(), 0, &raw);

    // JUBR_MULTISIG_OK 表示多重签名结果不完整，需要后续再次签名
    if (rv != JUBR_OK && rv != JUBR_MULTISIG_OK) {
        errorCode = static_cast<int>(rv);
        return NULL;
    }
    // 用于判断是否多签完成
    errorCode = rv;
    jstring rawString = env->NewStringUTF(raw);
    JUB_FreeMemory(raw);
    return rawString;
}


//==================================== ETH ==========================================

JNIEXPORT jint JNICALL
native_ETHCreateContext(JNIEnv *env, jclass obj, jintArray jContextId, jstring jJSON,
                        jlong deviceInfo) {

#define MAIN_PATH  "main_path"
#define CHAIN_ID   "chainID"

    JUB_UINT16 *pContextID = (JUB_UINT16 *) env->GetIntArrayElements(jContextId, NULL);

    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, NULL));

    Json::Reader reader;
    Json::Value root;
    reader.parse(pJSON, root);

    CONTEXT_CONFIG_ETH cfg;
    cfg.mainPath = (char *) root[MAIN_PATH].asCString();
    cfg.chainID = root[CHAIN_ID].asInt();
    int rv = JUB_CreateContextETH(cfg, static_cast<JUB_UINT16>(deviceInfo), pContextID);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_CreateContextETH: %08x", rv);
    } else {
        LOG_INF("JUB_CreateContextETH: %d", *pContextID);
    }
    env->ReleaseIntArrayElements(jContextId, (jint *) pContextID, FALSE);
    return rv;
}


JNIEXPORT jstring JNICALL native_ETH_Transaction(JNIEnv *env, jclass obj, jlong contextID,
                                                 jstring jJSON) {

    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, NULL));

    Json::Reader reader;
    Json::Value root;
    reader.parse(pJSON, root);

    BIP32_Path path;
    path.change = (JUB_ENUM_BOOL) root["ETH"]["bip32_path"]["change"].asBool();
    path.addressIndex = root["ETH"]["bip32_path"]["addressIndex"].asUInt();

    uint32_t nonce = static_cast<uint32_t>(root["ETH"]["nonce"].asDouble());
    uint32_t gasLimit = static_cast<uint32_t>(root["ETH"]["gasLimit"].asDouble());
    char *gasPriceInWei = (char *) root["ETH"]["gasPriceInWei"].asCString();
    char *valueInWei = (char *) root["ETH"]["valueInWei"].asCString();
    char *to = (char *) root["ETH"]["to"].asCString();
    char *data = (char *) root["ETH"]["data"].asCString();

    char *raw = nullptr;
    JUB_RV rv = JUB_SignTransactionETH(static_cast<JUB_UINT16>(contextID), path,
                                       nonce, gasLimit,
                                       gasPriceInWei, to, valueInWei, data, &raw);
    if (rv != JUBR_OK) {
        errorCode = static_cast<int>(rv);
        return NULL;
    }

    jstring rawString = env->NewStringUTF(raw);
    JUB_FreeMemory(raw);
    return rawString;
}

JNIEXPORT jstring JNICALL native_ETH_BuildERC20Abi(JNIEnv *env, jclass obj, jlong contextID,
                                                       jstring jJSON) {
    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, NULL));

    Json::Reader reader;
    Json::Value root;
    reader.parse(pJSON, root);


    char *token_to = (char *) root["ERC20"]["token_to"].asCString();
    char *token_value = (char *) root["ERC20"]["token_value"].asCString();
    char *tokenName = (char *) root["ERC20Token"]["tokenName"].asCString();
    uint16_t unitDP = root["ERC20Token"]["dp"].asDouble();
    char *contractAddress = (char *) root["ERC20Token"]["contract_address"].asCString();


    char *abi = nullptr;
    JUB_RV rv = JUB_BuildERC20AbiETH(contextID, tokenName, unitDP, contractAddress, token_to,
                                     token_value, &abi);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_BuildERC20AbiETH: %08x", rv);
        env->ReleaseStringUTFChars(jJSON, pJSON);
        return NULL;
    }
    jstring rawString = env->NewStringUTF(abi);
    JUB_FreeMemory(abi);
    return rawString;
}


JNIEXPORT jstring JNICALL native_ETH_ERC20_Transaction(JNIEnv *env, jclass obj, jlong contextID,
                                                       jstring jJSON) {

    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, NULL));

    Json::Reader reader;
    Json::Value root;
    reader.parse(pJSON, root);

    uint32_t nonce = root["ERC20"]["nonce"].asDouble();
    uint32_t gasLimit = root["ERC20"]["gasLimit"].asDouble();
    char *gasPriceInWei = (char *) root["ERC20"]["gasPriceInWei"].asCString();
    char *to = (char *) root["ERC20"]["contract_address"].asCString();
    char *token_to = (char *) root["ERC20"]["token_to"].asCString();
    char *token_value = (char *) root["ERC20"]["token_value"].asCString();

    BIP32_Path path;
    path.change = (JUB_ENUM_BOOL) root["ERC20"]["bip32_path"]["change"].asBool();
    path.addressIndex = root["ERC20"]["bip32_path"]["addressIndex"].asUInt();

    char *tokenName = (char *) root["ERC20Token"]["tokenName"].asCString();
    uint16_t unitDP = root["ERC20Token"]["dp"].asDouble();
    char *contractAddress = (char *) root["ERC20Token"]["contract_address"].asCString();


    char *abi = nullptr;
    JUB_RV rv = JUB_BuildERC20AbiETH(contextID, tokenName, unitDP, contractAddress, token_to,
                                     token_value, &abi);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_BuildERC20AbiETH: %08x", rv);
        env->ReleaseStringUTFChars(jJSON, pJSON);
        return NULL;
    }

    char *raw = nullptr;
    rv = JUB_SignTransactionETH(contextID, path, nonce, gasLimit, gasPriceInWei, to, 0, abi, &raw);

    JUB_FreeMemory(abi);
    if (rv != JUBR_OK) {
        errorCode = static_cast<int>(rv);
        return NULL;
    }

    jstring rawString = env->NewStringUTF(raw);
    JUB_FreeMemory(raw);
    return rawString;
}

JNIEXPORT jstring JNICALL native_ETH_SignContract(JNIEnv *env, jclass obj, jlong contextID,
                                                       jstring jJSON) {

    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, NULL));

    Json::Reader reader;
    Json::Value root;
    reader.parse(pJSON, root);

    BIP32_Path path;
    path.change = (JUB_ENUM_BOOL) root["ETH"]["bip32_path"]["change"].asBool();
    path.addressIndex = root["ETH"]["bip32_path"]["addressIndex"].asUInt();

    uint32_t nonce = static_cast<uint32_t>(root["ETH"]["nonce"].asDouble());
    uint32_t gasLimit = static_cast<uint32_t>(root["ETH"]["gasLimit"].asDouble());
    char *gasPriceInWei = (char *) root["ETH"]["gasPriceInWei"].asCString();
    char *valueInWei = nullptr;
    try {
       valueInWei = (char *) root["ETH"]["valueInWei"].asCString();
    } catch (...){}

    char *to = nullptr;
    try {
        to = (char *) root["ETH"]["contract_address"].asCString();
    } catch (...){}

    char *data = (char *) root["ETH"]["data"].asCString();


    char *raw = nullptr;
    JUB_RV rv = JUB_SignContractETH(contextID, path, nonce, gasLimit, gasPriceInWei, to, valueInWei, data, &raw);

    if (rv != JUBR_OK) {
        errorCode = static_cast<int>(rv);
        return NULL;
    }

    jstring rawString = env->NewStringUTF(raw);
    JUB_FreeMemory(raw);
    return rawString;
}


JNIEXPORT jint JNICALL native_ETH_SetContrAddr(JNIEnv *env, jclass obj, jlong contextID,
                                                  jstring jContrAddr) {

    JUB_CHAR_PTR pContrAddr = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jContrAddr, NULL));
    JUB_RV rv = JUB_SetContrAddrETH(contextID,pContrAddr);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_SetContrAddrETH rv: %08", rv);
    }
    return rv;
}


//JNIEXPORT jstring JNICALL native_ETH_BuildContractWithAddrAbi(JNIEnv *env, jobject obj, jlong contextID,
//                                                  jstring jJSON) {
//
//    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, NULL));
//
//    Json::Reader reader;
//    Json::Value root;
//    reader.parse(pJSON, root);
//
//    char *methodID = (char *) root["methodID"].asCString();
//    char *address = (char *) root["address"].asCString();
//
//    char *abi = nullptr;
//    JUB_RV rv = JUB_BuildContractWithAddrAbiETH(contextID, methodID, address, &abi);
//
//    if (rv != JUBR_OK) {
//        errorCode = static_cast<int>(rv);
//        return NULL;
//    }
//
//    jstring rawString = env->NewStringUTF(abi);
//    JUB_FreeMemory(abi);
//    return rawString;
//}


JNIEXPORT jstring JNICALL native_ETH_BuildContractWithAddrAmtDataAbi(JNIEnv *env, jclass obj, jlong contextID,
                                                              jstring jJSON) {

    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, NULL));

    Json::Reader reader;
    Json::Value root;
    reader.parse(pJSON, root);

    char *methodID = (char *) root["methodID"].asCString();
    char *address = (char *) root["address"].asCString();
    char *amount = (char *) root["amount"].asCString();
    char *data = (char *) root["data"].asCString();

    char *abi = nullptr;
    JUB_RV rv = JUB_BuildContractWithAddrAmtDataAbiETH(contextID, methodID, address, amount,data, &abi);

    if (rv != JUBR_OK) {
        errorCode = static_cast<int>(rv);
        return NULL;
    }

    jstring rawString = env->NewStringUTF(abi);
    JUB_FreeMemory(abi);
    return rawString;
}


JNIEXPORT jstring JNICALL native_ETH_BuildContractWithTxIDAbi(JNIEnv *env, jclass obj, jlong contextID,
                                                                 jstring jJSON) {

    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, NULL));

    Json::Reader reader;
    Json::Value root;
    reader.parse(pJSON, root);

    char *methodID = (char *) root["methodID"].asCString();
    char *transactionID = (char *) root["transactionID"].asCString();

    char *abi = nullptr;
    JUB_RV rv = JUB_BuildContractWithTxIDAbiETH(contextID, methodID, transactionID, &abi);

    if (rv != JUBR_OK) {
        errorCode = static_cast<int>(rv);
        return NULL;
    }

    jstring rawString = env->NewStringUTF(abi);
    JUB_FreeMemory(abi);
    return rawString;
}
//
//JNIEXPORT jstring JNICALL native_ETH_BuildContractWithAmtAbi(JNIEnv *env, jobject obj, jlong contextID,
//                                                              jstring jJSON) {
//
//    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, NULL));
//
//    Json::Reader reader;
//    Json::Value root;
//    reader.parse(pJSON, root);
//
//    char *methodID = (char *) root["methodID"].asCString();
//    char *amount = (char *) root["amount"].asCString();
//
//    char *abi = nullptr;
//    JUB_RV rv = JUB_BuildContractWithAmtAbiETH(contextID, methodID, amount, &abi);
//
//    if (rv != JUBR_OK) {
//        errorCode = static_cast<int>(rv);
//        return NULL;
//    }
//
//    jstring rawString = env->NewStringUTF(abi);
//    JUB_FreeMemory(abi);
//    return rawString;
//}

JNIEXPORT jint JNICALL
native_QueryBattery(JNIEnv *env, jclass obj, jlong deviceID, jintArray batteryArray) {

    jint *pBattery = env->GetIntArrayElements(batteryArray, NULL);
    JUB_BYTE battery = 0;
    JUB_RV rv = JUB_QueryBattery(deviceID, &battery);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_QueryBattery rv: %08lx", rv);
        env->ReleaseIntArrayElements(batteryArray, pBattery, 0);
        return rv;
    }
    *pBattery = battery & 0x0FF;
    env->ReleaseIntArrayElements(batteryArray, pBattery, 0);
    return rv;
}

JNIEXPORT jint JNICALL native_IsInitialize(JNIEnv *env, jclass obj, jlong deviceID) {
    JUB_UINT16 deviceId = deviceID;
    JUB_RV rv = JUB_IsInitialize(deviceId);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_IsInitialize rv: %08x", rv);
    }
    return rv;
}

JNIEXPORT jint JNICALL native_IsBootLoader(JNIEnv *env, jclass obj, jlong deviceID) {
    JUB_UINT16 deviceId = deviceID;
    JUB_RV rv = JUB_IsBootLoader(deviceId);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_IsBootLoader rv: %08x", rv);
    }
    return rv;
}

JNIEXPORT jstring JNICALL native_EnumSupportCoins(JNIEnv *env, jclass obj, jlong deviceID) {
    JUB_CHAR_PTR pCoinList = NULL;
    JUB_RV rv = Jub_EnumSupportCoins(deviceID, &pCoinList);
    if (rv != JUBR_OK) {
        LOG_ERR("Jub_EnumSupportCoins rv: %08x", rv);
        return env->NewStringUTF("");
    }
    jstring coinList = env->NewStringUTF(pCoinList);
    JUB_FreeMemory(pCoinList);
    return coinList;
}

JNIEXPORT jobjectArray JNICALL
native_ETHGetAddress(JNIEnv *env, jclass obj, jlong contextID, jstring jJSON) {

#define BIP32_PATH   "bip32_path"
#define CHANGE       "change"
#define INDEX        "addressIndex"

    jclass clazz = env->FindClass("java/lang/String");
    if (clazz == NULL) {
        LOG_ERR("clazz == NULL");
        return NULL;
    }
    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, NULL));

    Json::Reader reader;
    Json::Value root;
    reader.parse(pJSON, root);

    int input_number = root[BIP32_PATH].size();
    jobjectArray array = env->NewObjectArray(2 * input_number, clazz, 0);
    for (int i = 0; i < input_number; i++) {
        JUB_CHAR_PTR xpub;

        BIP32_Path path;
        path.change = (JUB_ENUM_BOOL) root[BIP32_PATH][i][CHANGE].asBool();
        path.addressIndex = static_cast<JUB_UINT64>(root[BIP32_PATH][i][INDEX].asInt());

        JUB_RV rv = JUB_GetHDNodeETH(contextID, XPUB, path, &xpub);
        if (rv != JUBR_OK) {
            LOG_ERR("JUB_GetHDNodeBTC: %08x", rv);
            env->SetObjectArrayElement(array, 2 * i, NULL);
            env->SetObjectArrayElement(array, 2 * i + 1, NULL);
        } else {
            jstring jsXpub = env->NewStringUTF(xpub);
            JUB_CHAR_PTR pAddress = NULL;
            rv = JUB_GetAddressETH(contextID, path, BOOL_FALSE, &pAddress);
            if (rv != JUBR_OK) {
                LOG_ERR("JUB_GetAddressBTC: %08x", rv);
                env->SetObjectArrayElement(array, 2 * i, jsXpub);
                env->SetObjectArrayElement(array, 2 * i + 1, NULL);
            } else {
                jstring address = env->NewStringUTF(pAddress);
                env->SetObjectArrayElement(array, 2 * i, jsXpub);
                env->SetObjectArrayElement(array, 2 * i + 1, address);
            }
        }
    }
    return array;
}

JNIEXPORT jstring JNICALL
native_ETHShowAddress(JNIEnv *env, jclass obj, jlong contextID, jint index) {
    jclass clazz = env->FindClass("java/lang/String");
    if (clazz == NULL) {
        LOG_ERR("clazz == NULL");
        return NULL;
    }

    BIP32_Path path;
    path.change = JUB_ENUM_BOOL(BOOL_FALSE);
    path.addressIndex = static_cast<JUB_UINT64>(index);

    JUB_CHAR_PTR pAddress = NULL;
    JUB_RV rv = JUB_GetAddressETH(static_cast<JUB_UINT16>(contextID), path, BOOL_TRUE, &pAddress);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_GetAddressBTC: %08x", rv);
        errorCode = static_cast<int>(rv);
        return NULL;
    }
    jstring address = env->NewStringUTF(pAddress);
    JUB_FreeMemory(pAddress);
    return address;
}

JNIEXPORT jstring JNICALL
native_SetMyAddressETH(JNIEnv *env, jclass obj, jlong contextID, jint change, jint index) {

    jclass clazz = env->FindClass("java/lang/String");
    if (clazz == NULL) {
        LOG_ERR("clazz == NULL");
        return NULL;
    }

    BIP32_Path path;
    path.change = JUB_ENUM_BOOL(BOOL_FALSE);
    path.addressIndex = static_cast<JUB_UINT64>(index);

    JUB_CHAR_PTR pAddress = NULL;
    JUB_RV rv = JUB_SetMyAddressETH(static_cast<JUB_UINT16>(contextID), path, &pAddress);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_SetMyAddressETH: %08x", rv);
        errorCode = static_cast<int>(rv);
        return NULL;
    }
    jstring address = env->NewStringUTF(pAddress);
    JUB_FreeMemory(pAddress);
    return address;
}

JNIEXPORT jstring JNICALL native_ETHGetMainHDNode(JNIEnv *env, jclass obj, jlong contextID) {
    jclass clazz = env->FindClass("java/lang/String");
    if (clazz == NULL) {
        LOG_ERR("clazz == NULL");
        return NULL;
    }

    JUB_CHAR_PTR xpub;
    JUB_RV rv = JUB_GetMainHDNodeETH(contextID, XPUB, &xpub);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_GetMainHDNodeETH: %08x", rv);
        return NULL;
    }
    jstring mainPub = env->NewStringUTF(xpub);
    JUB_FreeMemory(xpub);
    return mainPub;
}

JNIEXPORT jstring JNICALL native_GetVersion(JNIEnv *env, jclass obj) {
    JUB_CHAR_PTR pVersion = JUB_GetVersion();
    if (pVersion == NULL) {
        LOG_ERR("JUB_GetVersion : %s", pVersion);
        return NULL;
    }
    jstring version = env->NewStringUTF(pVersion);
    return version;
}


//=================================== HC Wallet =========================================

#ifdef HC

JNIEXPORT jint JNICALL
native_HCCreateContext(JNIEnv *env, jclass obj, jintArray jContextId, jboolean isMultiSig,
                       jstring jJSON, jlong deviceInfo) {

#define MAIN_PATH      "main_path"
#define M              "m"
#define N              "n"
#define MASTER_KEY     "cosigner"

    if (NULL == jJSON) {
        return JUBR_ARGUMENTS_BAD;
    }

    int length = env->GetStringLength(jJSON);
    if (0 == length) {
        errorCode = JUBR_ARGUMENTS_BAD;
        return JUBR_ARGUMENTS_BAD;
    }

    JUB_UINT16 *pContextID = (JUB_UINT16 *) env->GetIntArrayElements(jContextId, NULL);
    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, NULL));

    Json::Reader reader;
    Json::Value root;
    reader.parse(pJSON, root);

    CONTEXT_CONFIG_HC cfg;
    CONTEXT_CONFIG_MULTISIG_HC multiCfg;
    cfg.mainPath = (char *) root[MAIN_PATH].asCString();

    // 缓存是否是多重签名标记
    globalMultiSig = isMultiSig;

    JUB_RV rv = JUBR_OK;
    if (isMultiSig) {
        multiCfg.transType = p2sh_multisig;
        multiCfg.mainPath = cfg.mainPath;
        multiCfg.m = root[M].asInt64();
        multiCfg.n = root[N].asInt64();

        int keySize = root[MASTER_KEY].size();
        std::vector<std::string> masterKey;
        for (int i = 0; i < keySize; ++i) {
            std::string key = root[MASTER_KEY][i].asString();
            masterKey.push_back(key);
        }
        multiCfg.vCosignerMainXpub = masterKey;

        rv = JUB_CreateMultiSigContextHC(multiCfg, deviceInfo, pContextID);
    } else {
        rv = JUB_CreateContextHC(cfg, deviceInfo, pContextID);
    }

    if (rv != JUBR_OK) {
        LOG_ERR("JUB_CreateContextBTC: %08x", rv);
    } else {
        LOG_INF("contextID: %d", *pContextID);
    }
    env->ReleaseIntArrayElements(jContextId, (jint *) pContextID, 0);
    return rv;




//    JUB_UINT16 *pContextID = (JUB_UINT16 *) env->GetIntArrayElements(jContextId, NULL);
//    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, NULL));
//
//    Json::Reader reader;
//    Json::Value root;
//    reader.parse(pJSON, root);
//
//    CONTEXT_CONFIG_HC cfg;
//    cfg.mainPath = (char *) root[MAIN_PATH].asCString();
//    JUB_RV rv = JUB_CreateContextHC(cfg, deviceInfo, pContextID);
//    if (rv != JUBR_OK) {
//        LOG_ERR("JUB_CreateContextHC: %08x", rv);
//    } else {
//        LOG_INF("contextID: %d", *pContextID);
//    }
//    env->ReleaseIntArrayElements(jContextId, (jint *) pContextID, 0);
//    return rv;
}

JNIEXPORT jobjectArray
native_HCGetAddress(JNIEnv *env, jclass obj, jlong contextID, jstring jJSON) {

#define BIP32_PATH   "bip32_path"
#define CHANGE       "change"
#define INDEX        "addressIndex"

    jclass clazz = env->FindClass("java/lang/String");
    if (clazz == NULL) {
        LOG_ERR("clazz == NULL");
        return NULL;
    }
    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, FALSE));

    Json::Reader reader;
    Json::Value root;
    reader.parse(pJSON, root);

    int input_number = root[BIP32_PATH].size();
    jobjectArray array = env->NewObjectArray(2 * input_number, clazz, 0);
    for (int i = 0; i < input_number; i++) {
        JUB_CHAR_PTR xpub;

        BIP32_Path path;
        path.change = (JUB_ENUM_BOOL) root[BIP32_PATH][i][CHANGE].asBool();
        path.addressIndex = static_cast<JUB_UINT64>(root[BIP32_PATH][i][INDEX].asInt());

        JUB_RV rv = JUB_GetHDNodeHC(contextID, path, &xpub);
        if (rv != JUBR_OK) {
            LOG_ERR("JUB_GetHDNodeHC: %08x", rv);
            env->SetObjectArrayElement(array, 2 * i, NULL);
            env->SetObjectArrayElement(array, 2 * i + 1, NULL);
        } else {
            jstring jsXpub = env->NewStringUTF(xpub);
            JUB_CHAR_PTR pAddress = NULL;
            rv = JUB_GetAddressHC(contextID, path, BOOL_FALSE, &pAddress);
            if (rv != JUBR_OK) {
                LOG_ERR("JUB_GetAddressHC: %08x", rv);
                env->SetObjectArrayElement(array, 2 * i, jsXpub);
                env->SetObjectArrayElement(array, 2 * i + 1, NULL);
            } else {
                jstring address = env->NewStringUTF(pAddress);
                env->SetObjectArrayElement(array, 2 * i, jsXpub);
                env->SetObjectArrayElement(array, 2 * i + 1, address);
            }
        }
    }
    return array;
}

JNIEXPORT jstring JNICALL
native_HCShowAddress(JNIEnv *env, jclass obj, jlong contextID, jint change, jint index) {
    jclass clazz = env->FindClass("java/lang/String");
    if (clazz == NULL) {
        LOG_ERR("clazz == NULL");
        return NULL;
    }

    BIP32_Path path;
    path.change = JUB_ENUM_BOOL(change);
    path.addressIndex = static_cast<JUB_UINT64>(index);

    JUB_CHAR_PTR pAddress = NULL;
    JUB_RV rv = JUB_GetAddressHC(static_cast<JUB_UINT16>(contextID), path, BOOL_TRUE, &pAddress);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_GetAddressHC: %08x", rv);
        errorCode = static_cast<int>(rv);
        return NULL;
    }
    jstring address = env->NewStringUTF(pAddress);
    JUB_FreeMemory(pAddress);
    return address;
}

JNIEXPORT jstring native_HCGetMainHDNode(JNIEnv *env, jclass obj, jlong contextID) {
    jclass clazz = env->FindClass("java/lang/String");
    if (clazz == NULL) {
        LOG_ERR("clazz == NULL");
        return NULL;
    }

    JUB_CHAR_PTR xpub;
    JUB_RV rv = JUB_GetMainHDNodeHC(contextID, &xpub);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_GetMainHDNodeHC: %08x", rv);
        errorCode = rv;
        return NULL;
    }
    jstring mainPub = env->NewStringUTF(xpub);
    JUB_FreeMemory(xpub);
    return mainPub;
}

JNIEXPORT jstring JNICALL
native_HCTransaction(JNIEnv *env, jclass obj, jlong contextID, jstring jJSON) {
#define INPUTS         "inputs"
#define BIP32_PATH     "bip32_path"
#define CHANGE         "change"
#define INDEX          "addressIndex"
#define AMOUNT         "amount"
#define UNSIGNED_TRANS "unsigned_trans"
#define CHANGE_ADDRESS "change_address"

    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, NULL));
    Json::Reader reader;
    Json::Value root;
    reader.parse(pJSON, root);

    std::vector<INPUT_HC> inputs;
    std::vector<OUTPUT_HC> outputs;

    int input_number = root[INPUTS].size();
    for (int i = 0; i < input_number; i++) {
        INPUT_HC input;
        input.path.change = (JUB_ENUM_BOOL) root[INPUTS][i][BIP32_PATH][CHANGE].asBool();
        input.path.addressIndex = static_cast<JUB_UINT64>(root[INPUTS][i][BIP32_PATH][INDEX].asInt());
        input.amount = static_cast<JUB_UINT64>(root[INPUTS][i][AMOUNT].asUInt64());
        inputs.push_back(input);
    }

    int output_number = root[OUTPUTS].size();
    for (int i = 0; i < output_number; i++) {
        OUTPUT_HC output;
        output.changeAddress = (JUB_ENUM_BOOL) root[OUTPUTS][i][CHANGE_ADDRESS].asBool();
        if (output.changeAddress) {
            output.path.change = (JUB_ENUM_BOOL) root[OUTPUTS][i][BIP32_PATH][CHANGE].asBool();
            output.path.addressIndex = static_cast<JUB_UINT64>(root[OUTPUTS][i][BIP32_PATH][INDEX].asInt());
        }
        outputs.push_back(output);
    }

    char *unsigned_trans = const_cast<char *>(root[UNSIGNED_TRANS].asCString());

    char *raw = NULL;
    JUB_SetUnitBTC(static_cast<JUB_UINT16>(contextID), BTC);
    JUB_RV rv = JUB_SignTransactionHC(static_cast<JUB_UINT16>(contextID),
                                      &inputs[0],
                                      (JUB_UINT16) inputs.size(),
                                      &outputs[0],
                                      (JUB_UINT16) outputs.size(),
                                      unsigned_trans,
                                      &raw);

    // JUBR_MULTISIG_OK 表示多重签名结果不完整，需要后续再次签名
    if (rv != JUBR_OK && rv != JUBR_MULTISIG_OK) {
        errorCode = static_cast<int>(rv);
        return NULL;
    }
    // 用于判断多签是否完成
    errorCode = rv;
    jstring rawString = env->NewStringUTF(raw);
    JUB_FreeMemory(raw);
    return rawString;
}

#endif

//=================================== EOS Wallet =========================================


JNIEXPORT jint JNICALL
native_EOSCreateContext(JNIEnv *env, jclass obj, jintArray jContextId,
                        jstring jJSON, jlong deviceInfo) {

#define MAIN_PATH      "main_path"

    if (NULL == jJSON) {
        return JUBR_ARGUMENTS_BAD;
    }

    int length = env->GetStringLength(jJSON);
    if (0 == length) {
        errorCode = JUBR_ARGUMENTS_BAD;
        return JUBR_ARGUMENTS_BAD;
    }

    JUB_UINT16 *pContextID = (JUB_UINT16 *) env->GetIntArrayElements(jContextId, NULL);
    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, NULL));

    Json::Reader reader;
    Json::Value root;
    reader.parse(pJSON, root);

    CONTEXT_CONFIG_EOS cfg;
    cfg.mainPath = (char *) root[MAIN_PATH].asCString();


    JUB_RV rv = JUBR_OK;
    rv = JUB_CreateContextEOS(cfg, deviceInfo, pContextID);

    if (rv != JUBR_OK) {
        LOG_ERR("native_EOSCreateContext: %08x", rv);
    } else {
        LOG_INF("contextID: %d", *pContextID);
    }
    env->ReleaseIntArrayElements(jContextId, (jint *) pContextID, 0);
    return rv;
}

JNIEXPORT jstring JNICALL
native_EOSGetAddress(JNIEnv *env, jclass obj, jlong contextID, jint change, jint index) {
    jclass clazz = env->FindClass("java/lang/String");
    if (clazz == NULL) {
        LOG_ERR("clazz == NULL");
        return NULL;
    }

    BIP32_Path path;
    path.change = JUB_ENUM_BOOL(change);
    path.addressIndex = static_cast<JUB_UINT64>(index);

    JUB_CHAR_PTR pAddress = NULL;
    JUB_RV rv = JUB_GetAddressEOS(static_cast<JUB_UINT16>(contextID), path, BOOL_FALSE, &pAddress);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_GetAddressEOS: %08x", rv);
        errorCode = static_cast<int>(rv);
        return NULL;
    }
    jstring address = env->NewStringUTF(pAddress);
    JUB_FreeMemory(pAddress);
    return address;
}

JNIEXPORT jstring JNICALL
native_EOSShowAddress(JNIEnv *env, jclass obj, jlong contextID, jint change, jint index) {
    jclass clazz = env->FindClass("java/lang/String");
    if (clazz == NULL) {
        LOG_ERR("clazz == NULL");
        return NULL;
    }

    BIP32_Path path;
    path.change = JUB_ENUM_BOOL(change);
    path.addressIndex = static_cast<JUB_UINT64>(index);

    JUB_CHAR_PTR pAddress = NULL;
    JUB_RV rv = JUB_GetAddressEOS(static_cast<JUB_UINT16>(contextID), path, BOOL_TRUE, &pAddress);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_GetAddressEOS: %08x", rv);
        errorCode = static_cast<int>(rv);
        return NULL;
    }
    jstring address = env->NewStringUTF(pAddress);
    JUB_FreeMemory(pAddress);
    return address;
}

JNIEXPORT jstring JNICALL
native_EOSSetMyAddress(JNIEnv *env, jclass obj, jlong contextID, jint change, jint index) {

    jclass clazz = env->FindClass("java/lang/String");
    if (clazz == NULL) {
        LOG_ERR("clazz == NULL");
        return NULL;
    }

    BIP32_Path path;
    path.change = JUB_ENUM_BOOL(change);
    path.addressIndex = static_cast<JUB_UINT64>(index);

    JUB_CHAR_PTR pAddress = NULL;
    JUB_RV rv = JUB_SetMyAddressEOS(static_cast<JUB_UINT16>(contextID), path, &pAddress);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_SetMyAddressEOS: %08x", rv);
        errorCode = static_cast<int>(rv);
        return NULL;
    }
    jstring address = env->NewStringUTF(pAddress);
    JUB_FreeMemory(pAddress);
    return address;
}

JNIEXPORT jstring JNICALL
native_EOSTransaction(JNIEnv *env, jclass obj, jlong contextID, jstring jJSON) {

    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, NULL));
    Json::Reader reader;
    Json::Value root;
    reader.parse(pJSON, root);

    BIP32_Path path;
    path.change = JUB_ENUM_BOOL(root["EOS"]["bip32_path"]["change"].asBool());
    path.addressIndex = static_cast<JUB_UINT64>(root["EOS"]["bip32_path"]["addressIndex"].asUInt());
    char *expiration = (char *) root["EOS"]["expiration"].asCString();

    char *chainID = nullptr;
    if (!root["EOS"]["chainID"].empty()) {
        chainID = (char *) root["EOS"]["chainID"].asCString();
    }
    char *referenceBlockId = (char *) root["EOS"]["referenceBlockId"].asCString();
    char *referenceBlockTime = (char *) root["EOS"]["referenceBlockTime"].asCString();


    std::vector<JUB_ACTION_EOS> actionArray;
    for (Json::Value::iterator it = root["EOS"]["actions"].begin();
         it != root["EOS"]["actions"].end(); ++it) {
        JUB_ACTION_EOS action;
        int typeInt = (*it)["type"].asUInt();
        const char *sType = "0";
        switch (typeInt) {
            case 0x00:
                action.type = JUB_ENUM_EOS_ACTION_TYPE::XFER;
                sType = "0";
                break;
            case 0x01:
                action.type = JUB_ENUM_EOS_ACTION_TYPE::DELE;
                sType = "1";
                break;
            case 0x02:
                action.type = JUB_ENUM_EOS_ACTION_TYPE::UNDELE;
                sType = "2";
                break;
            case 0x03:
                action.type = JUB_ENUM_EOS_ACTION_TYPE::BUYRAM;
                sType = "3";
                break;
            case 0x04:
                action.type = JUB_ENUM_EOS_ACTION_TYPE::SELLRAM;
                sType = "4";
                break;
            default:
                action.type = JUB_ENUM_EOS_ACTION_TYPE::NS_ITEM_ACTION_TYPE;
                break;
        }
        action.currency = (char *) (*it)["currency"].asCString();
        action.name = (char *) (*it)["name"].asCString();

        switch (action.type) {
            case JUB_ENUM_EOS_ACTION_TYPE::XFER:
                action.transfer;
                action.transfer.from = (char *) (*it)[sType]["from"].asCString();
                action.transfer.to = (char *) (*it)[sType]["to"].asCString();
                action.transfer.asset = (char *) (*it)[sType]["asset"].asCString();
                action.transfer.memo = (char *) (*it)[sType]["memo"].asCString();
                break;
            case JUB_ENUM_EOS_ACTION_TYPE::DELE:
                action.delegate.from = (char *) (*it)[sType]["from"].asCString();
                action.delegate.receiver = (char *) (*it)[sType]["receiver"].asCString();
                action.delegate.netQty = (char *) (*it)[sType]["stake_net_quantity"].asCString();
                action.delegate.cpuQty = (char *) (*it)[sType]["stake_cpu_quantity"].asCString();
                action.delegate.bStake = JUB_ENUM_BOOL::BOOL_TRUE;
            case JUB_ENUM_EOS_ACTION_TYPE::UNDELE:
                action.delegate;
                action.delegate.from = (char *) (*it)[sType]["from"].asCString();
                action.delegate.receiver = (char *) (*it)[sType]["receiver"].asCString();
                action.delegate.netQty = (char *) (*it)[sType]["unstake_net_quantity"].asCString();
                action.delegate.cpuQty = (char *) (*it)[sType]["unstake_cpu_quantity"].asCString();
                action.delegate.bStake = JUB_ENUM_BOOL::BOOL_FALSE;
                break;
            case JUB_ENUM_EOS_ACTION_TYPE::BUYRAM:
                action.buyRam;
                action.buyRam.payer = (char *) (*it)[sType]["payer"].asCString();
                action.buyRam.quant = (char *) (*it)[sType]["quant"].asCString();
                action.buyRam.receiver = (char *) (*it)[sType]["receiver"].asCString();
                break;
            case JUB_ENUM_EOS_ACTION_TYPE::SELLRAM:
                action.sellRam;
                action.sellRam.account = (char *) (*it)[sType]["account"].asCString();
                action.sellRam.bytes = (char *) (*it)[sType]["bytes"].asCString();
                break;
            case JUB_ENUM_EOS_ACTION_TYPE::NS_ITEM_ACTION_TYPE:
                break;
        }
        actionArray.push_back(action);
    }

    char *actions = NULL;
    JUB_BuildActionEOS(static_cast<JUB_UINT16>(contextID), &actionArray[0],
                       static_cast<JUB_UINT16>(actionArray.size()), &actions);


    char *raw = NULL;
    JUB_RV rv = JUB_SignTransactionEOS(static_cast<JUB_UINT16>(contextID),
                                       path,
                                       chainID,
                                       expiration,
                                       referenceBlockId,
                                       referenceBlockTime,
                                       actions,
                                       &raw);

    // JUBR_MULTISIG_OK 表示多重签名结果不完整，需要后续再次签名
    if (rv != JUBR_OK && rv != JUBR_MULTISIG_OK) {
        errorCode = static_cast<int>(rv);
        return NULL;
    }
    // 用于判断多签是否完成
    errorCode = rv;
    jstring rawString = env->NewStringUTF(raw);
    JUB_FreeMemory(raw);
    return rawString;
}
//=================================== XRP =========================================


JNIEXPORT jint JNICALL
native_XRPCreateContext(JNIEnv *env, jclass obj, jintArray jContextId,
                        jstring jJSON, jlong deviceInfo) {

#define MAIN_PATH      "main_path"

    if (NULL == jJSON) {
        return JUBR_ARGUMENTS_BAD;
    }

    int length = env->GetStringLength(jJSON);
    if (0 == length) {
        errorCode = JUBR_ARGUMENTS_BAD;
        return JUBR_ARGUMENTS_BAD;
    }

    JUB_UINT16 *pContextID = (JUB_UINT16 *) env->GetIntArrayElements(jContextId, NULL);
    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, NULL));

    Json::Reader reader;
    Json::Value root;
    reader.parse(pJSON, root);

    CONTEXT_CONFIG_XRP cfg;
    cfg.mainPath = (char *) root[MAIN_PATH].asCString();


    JUB_RV rv = JUBR_OK;
    rv = JUB_CreateContextXRP(cfg, deviceInfo, pContextID);

    if (rv != JUBR_OK) {
        LOG_ERR("native_XRPCreateContext: %08x", rv);
    } else {
        LOG_INF("contextID: %d", *pContextID);
    }
    env->ReleaseIntArrayElements(jContextId, (jint *) pContextID, 0);
    return rv;
}

JNIEXPORT jstring JNICALL
native_XRPGetAddress(JNIEnv *env, jclass obj, jlong contextID, jint change, jint index) {
    jclass clazz = env->FindClass("java/lang/String");
    if (clazz == NULL) {
        LOG_ERR("clazz == NULL");
        return NULL;
    }

    BIP32_Path path;
    path.change = JUB_ENUM_BOOL(change);
    path.addressIndex = static_cast<JUB_UINT64>(index);

    JUB_CHAR_PTR pAddress = NULL;
    JUB_RV rv = JUB_GetAddressXRP(static_cast<JUB_UINT16>(contextID), path, BOOL_FALSE, &pAddress);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_GetAddressXRP: %08x", rv);
        errorCode = static_cast<int>(rv);
        return NULL;
    }
    jstring address = env->NewStringUTF(pAddress);
    JUB_FreeMemory(pAddress);
    return address;
}

JNIEXPORT jstring JNICALL
native_XRPGetHDNode(JNIEnv *env, jclass obj, jlong contextID, jint change, jint index,
                    jboolean useHex) {
    jclass clazz = env->FindClass("java/lang/String");
    if (clazz == NULL) {
        LOG_ERR("clazz == NULL");
        return NULL;
    }

    BIP32_Path path;
    path.change = JUB_ENUM_BOOL(change);
    path.addressIndex = static_cast<JUB_UINT64>(index);

    JUB_ENUM_PUB_FORMAT format = JUB_ENUM_PUB_FORMAT::XPUB;
    if (useHex) {
        format = JUB_ENUM_PUB_FORMAT::HEX;
    }

    JUB_CHAR_PTR pPubKey = NULL;
    JUB_RV rv = JUB_GetHDNodeXRP(static_cast<JUB_UINT16>(contextID), format, path, &pPubKey);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_GetHDNodeXRP: %08x", rv);
        errorCode = static_cast<int>(rv);
        return NULL;
    }
    jstring pubKey = env->NewStringUTF(pPubKey);
    JUB_FreeMemory(pPubKey);
    return pubKey;
}

JNIEXPORT jstring JNICALL
native_XRPShowAddress(JNIEnv *env, jclass obj, jlong contextID, jint change, jint index) {
    jclass clazz = env->FindClass("java/lang/String");
    if (clazz == NULL) {
        LOG_ERR("clazz == NULL");
        return NULL;
    }

    BIP32_Path path;
    path.change = JUB_ENUM_BOOL(change);
    path.addressIndex = static_cast<JUB_UINT64>(index);

    JUB_CHAR_PTR pAddress = NULL;
    JUB_RV rv = JUB_GetAddressXRP(static_cast<JUB_UINT16>(contextID), path, BOOL_TRUE, &pAddress);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_GetAddressXRP: %08x", rv);
        errorCode = static_cast<int>(rv);
        return NULL;
    }
    jstring address = env->NewStringUTF(pAddress);
    JUB_FreeMemory(pAddress);
    return address;
}

JNIEXPORT jstring JNICALL
native_XRPSetMyAddress(JNIEnv *env, jclass obj, jlong contextID, jint change, jint index) {

    jclass clazz = env->FindClass("java/lang/String");
    if (clazz == NULL) {
        LOG_ERR("clazz == NULL");
        return NULL;
    }

    BIP32_Path path;
    path.change = JUB_ENUM_BOOL(change);
    path.addressIndex = static_cast<JUB_UINT64>(index);

    JUB_CHAR_PTR pAddress = NULL;
    JUB_RV rv = JUB_SetMyAddressXRP(static_cast<JUB_UINT16>(contextID), path, &pAddress);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_SetMyAddressXRP: %08x", rv);
        errorCode = static_cast<int>(rv);
        return NULL;
    }
    jstring address = env->NewStringUTF(pAddress);
    JUB_FreeMemory(pAddress);
    return address;
}

JNIEXPORT jstring JNICALL
native_XRPTransaction(JNIEnv *env, jclass obj, jlong contextID, jstring jJSON) {
    DEBUG_LOG(" JSON: %s\n", env->GetStringUTFChars(jJSON,FALSE));

    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, NULL));
    Json::Reader reader;
    Json::Value root;
    reader.parse(pJSON, root);

    JUB_TX_XRP tx;
    int numType = root["XRP"]["type"].asUInt();
    const char *sType = "0";

    tx.type = JUB_ENUM_XRP_TX_TYPE::PYMT;
    if (numType != 0) {
        sType = "1";
        tx.type = JUB_ENUM_XRP_TX_TYPE::NS_ITEM_TX_TYPE;
    }
    switch (tx.type) {
        case JUB_ENUM_XRP_TX_TYPE::PYMT: {
            tx.account = (char *) root["XRP"]["account"].asCString();
            tx.fee = (char *) root["XRP"]["fee"].asCString();
            tx.sequence = (char *) root["XRP"]["sequence"].asCString();
            tx.flags = (char *) root["XRP"]["flags"].asCString();
            tx.lastLedgerSequence = (char *) root["XRP"]["lastLedgerSequence"].asCString();

            tx.pymt;
            int txPymtType = root["XRP"][sType]["type"].asUInt();
            tx.pymt.type = txPymtType == 0 ? JUB_ENUM_XRP_PYMT_TYPE::DXRP
                                           : JUB_ENUM_XRP_PYMT_TYPE::NS_ITEM_PYMT_TYPE;
//            const char* pymtType = std::to_string(tx.pymt.type).c_str();
            switch (tx.pymt.type) {
                case JUB_ENUM_XRP_PYMT_TYPE::DXRP: {
                    tx.pymt.amount;
                    tx.pymt.amount.value = (char *) root["XRP"][sType]["amount"]["value"].asCString();
                    tx.pymt.destination = (char *) root["XRP"][sType]["destination"].asCString();
                    if (!root["XRP"][sType]["destinationTag"].empty()) {
                        tx.pymt.destinationTag = (char *) root["XRP"][sType]["destinationTag"].asCString();
                    }
                    break;
                }
                case JUB_ENUM_XRP_PYMT_TYPE::NS_ITEM_PYMT_TYPE:
                    break;
            }
            break;
        }
        case JUB_ENUM_XRP_TX_TYPE::NS_ITEM_TX_TYPE: {
            tx.accountTxnID = (char *) root["XRP"]["accountTxnID"].asCString();
            tx.sourceTag = (char *) root["XRP"]["sourceTag"].asCString();
            break;
        }
    }
    JUB_XRP_MEMO memo;
    memo.type = const_cast<JUB_CHAR_PTR>("");
    memo.data = const_cast<JUB_CHAR_PTR>("");
    memo.format = const_cast<JUB_CHAR_PTR>("");
    if (!root["XRP"]["memo"].empty()) {
        if (!root["XRP"]["memo"]["type"].empty()) {
            memo.type = (char *) root["XRP"]["memo"]["type"].asCString();
        }
        if (!root["XRP"]["memo"]["data"].empty()) {
            memo.data = (char *) root["XRP"]["memo"]["data"].asCString();
        }
        if (!root["XRP"]["memo"]["format"].empty()) {
            memo.format = (char *) root["XRP"]["memo"]["format"].asCString();
        }
    }
    tx.memo = memo;
    BIP32_Path path;
    if (root["XRP"]["bip32_path"]["change"].asBool()) {
        path.change = JUB_ENUM_BOOL::BOOL_TRUE;
    } else {
        path.change = JUB_ENUM_BOOL::BOOL_FALSE;
    }


    char *raw = NULL;
    JUB_RV rv = JUB_SignTransactionXRP(static_cast<JUB_UINT16>(contextID),
                                       path,
                                       tx,
                                       &raw);

    if (rv != JUBR_OK) {
        errorCode = static_cast<int>(rv);
        return NULL;
    }
    jstring rawString = env->NewStringUTF(raw);
    JUB_FreeMemory(raw);
    return rawString;
}

//=================================== TRX =========================================


JNIEXPORT jint JNICALL
native_TRXCreateContext(JNIEnv *env, jclass obj, jintArray jContextId,
                        jstring jJSON, jlong deviceInfo) {

#define MAIN_PATH      "main_path"

    if (NULL == jJSON) {
        return JUBR_ARGUMENTS_BAD;
    }

    int length = env->GetStringLength(jJSON);
    if (0 == length) {
        errorCode = JUBR_ARGUMENTS_BAD;
        return JUBR_ARGUMENTS_BAD;
    }

    JUB_UINT16 *pContextID = (JUB_UINT16 *) env->GetIntArrayElements(jContextId, NULL);
    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, NULL));

    Json::Reader reader;
    Json::Value root;
    reader.parse(pJSON, root);

    CONTEXT_CONFIG_TRX cfg;
    cfg.mainPath = (char *) root[MAIN_PATH].asCString();


    JUB_RV rv = JUBR_OK;
    rv = JUB_CreateContextTRX(cfg, deviceInfo, pContextID);

    if (rv != JUBR_OK) {
        LOG_ERR("JUB_CreateContextTRX: %08x", rv);
    } else {
        LOG_INF("contextID: %d", *pContextID);
    }
    env->ReleaseIntArrayElements(jContextId, (jint *) pContextID, 0);
    return rv;
}

JNIEXPORT jstring JNICALL
native_TRXGetAddress(JNIEnv *env, jclass obj, jlong contextID, jint change, jint index) {
    jclass clazz = env->FindClass("java/lang/String");
    if (clazz == NULL) {
        LOG_ERR("clazz == NULL");
        return NULL;
    }

    BIP32_Path path;
    path.change = JUB_ENUM_BOOL(change);
    path.addressIndex = static_cast<JUB_UINT64>(index);

    JUB_CHAR_PTR pAddress = NULL;
    JUB_RV rv = JUB_GetAddressTRX(static_cast<JUB_UINT16>(contextID), path, BOOL_FALSE, &pAddress);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_GetAddressTRX: %08x", rv);
        errorCode = static_cast<int>(rv);
        return NULL;
    }
    jstring address = env->NewStringUTF(pAddress);
    JUB_FreeMemory(pAddress);
    return address;
}

JNIEXPORT jstring JNICALL
native_TRXGetHDNode(JNIEnv *env, jclass obj, jlong contextID, jint change, jint index,
                    jboolean useHex) {
    jclass clazz = env->FindClass("java/lang/String");
    if (clazz == NULL) {
        LOG_ERR("clazz == NULL");
        return NULL;
    }

    BIP32_Path path;
    path.change = JUB_ENUM_BOOL(change);
    path.addressIndex = static_cast<JUB_UINT64>(index);

    JUB_ENUM_PUB_FORMAT format = JUB_ENUM_PUB_FORMAT::XPUB;
    if (useHex) {
        format = JUB_ENUM_PUB_FORMAT::HEX;
    }

    JUB_CHAR_PTR pPubKey = NULL;
    JUB_RV rv = JUB_GetHDNodeTRX(static_cast<JUB_UINT16>(contextID), format, path, &pPubKey);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_GetHDNodeTRX: %08x", rv);
        errorCode = static_cast<int>(rv);
        return NULL;
    }
    jstring pubKey = env->NewStringUTF(pPubKey);
    JUB_FreeMemory(pPubKey);
    return pubKey;
}

JNIEXPORT jstring JNICALL
native_TRXShowAddress(JNIEnv *env, jclass obj, jlong contextID, jint change, jint index) {
    jclass clazz = env->FindClass("java/lang/String");
    if (clazz == NULL) {
        LOG_ERR("clazz == NULL");
        return NULL;
    }

    BIP32_Path path;
    path.change = JUB_ENUM_BOOL(change);
    path.addressIndex = static_cast<JUB_UINT64>(index);

    JUB_CHAR_PTR pAddress = NULL;
    JUB_RV rv = JUB_GetAddressTRX(static_cast<JUB_UINT16>(contextID), path, BOOL_TRUE, &pAddress);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_GetAddressTRX: %08x", rv);
        errorCode = static_cast<int>(rv);
        return NULL;
    }
    jstring address = env->NewStringUTF(pAddress);
    JUB_FreeMemory(pAddress);
    return address;
}

JNIEXPORT jstring JNICALL
native_TRXSetMyAddress(JNIEnv *env, jclass obj, jlong contextID, jint change, jint index) {

    jclass clazz = env->FindClass("java/lang/String");
    if (clazz == NULL) {
        LOG_ERR("clazz == NULL");
        return NULL;
    }

    BIP32_Path path;
    path.change = JUB_ENUM_BOOL(change);
    path.addressIndex = static_cast<JUB_UINT64>(index);

    JUB_CHAR_PTR pAddress = NULL;
    JUB_RV rv = JUB_SetMyAddressTRX(static_cast<JUB_UINT16>(contextID), path, &pAddress);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_SetMyAddressTRX: %08x", rv);
        errorCode = static_cast<int>(rv);
        return NULL;
    }
    jstring address = env->NewStringUTF(pAddress);
    JUB_FreeMemory(pAddress);
    return address;
}

JNIEXPORT jstring JNICALL
native_TRXTransaction(JNIEnv *env, jclass obj, jlong contextID, jstring jJSON, jbyteArray jPackedContractInPb) {
    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, NULL));
    Json::Reader reader;
    Json::Value root;
    reader.parse(pJSON, root);

    BIP32_Path path;
    if (root["TRX"]["bip32_path"]["change"].asBool()) {
        path.change = JUB_ENUM_BOOL::BOOL_TRUE;
    } else {
        path.change = JUB_ENUM_BOOL::BOOL_FALSE;
    }

    JUB_CHAR_PTR pPackedContractInPb = (JUB_CHAR_PTR) (env->GetByteArrayElements(jPackedContractInPb, NULL));
    char *raw = NULL;
    JUB_RV rv = JUB_SignTransactionTRX(static_cast<JUB_UINT16>(contextID),
                                       path,
                                       pPackedContractInPb,
                                       &raw);

    if (rv != JUBR_OK) {
        errorCode = static_cast<int>(rv);
        return NULL;
    }
    jstring rawString = env->NewStringUTF(raw);
    JUB_FreeMemory(raw);
    return rawString;
}

JNIEXPORT jstring JNICALL native_TRXBuildTRC20Abi(JNIEnv *env, jclass obj, jlong contextID,
                                                  jstring jJSON) {
    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, NULL));

    Json::Reader reader;
    Json::Value root;
    reader.parse(pJSON, root);

    char *token_to = (char *) root["TRC20"]["token_to"].asCString();
    char *token_value = (char *) root["TRC20"]["token_value"].asCString();
    char *tokenName = (char *) root["TRC20"]["tokenName"].asCString();
    uint16_t unitDP = root["TRC20"]["dp"].asDouble();
    char *contractAddress = (char *) root["TRC20"]["contract_address"].asCString();


    char *abi = nullptr;
    JUB_RV rv = JUB_BuildTRC20Abi(contextID, tokenName, unitDP, contractAddress, token_to,
                                  token_value, &abi);

    env->ReleaseStringUTFChars(jJSON, pJSON);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_BuildTRC20Abi: %08x", rv);
        return NULL;
    }
    jstring rawString = env->NewStringUTF(abi);
    JUB_FreeMemory(abi);
    return rawString;
}

JNIEXPORT jint JNICALL native_TRXSetTRC10Asset(JNIEnv *env, jclass obj, jlong contextID,
                                                   jstring jJSON) {
    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, NULL));

    Json::Reader reader;
    Json::Value root;
    reader.parse(pJSON, root);

    char *assetName = (char *) root["TRC10"]["assetName"].asCString();
    uint16_t unitDP = root["TRC10"]["dp"].asDouble();
    char *assetID = (char *) root["TRC10"]["assetID"].asCString();

    JUB_RV rv = JUB_SetTRC10Asset(contextID, assetName, unitDP, assetID);

    env->ReleaseStringUTFChars(jJSON, pJSON);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_SetTRC10Asset: %08x", rv);
    }
    return rv;
}

JNIEXPORT jbyteArray JNICALL
native_TRXPackContract(JNIEnv *env, jclass obj, jlong contextID, jstring jJSON) {
    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, NULL));
    Json::Reader reader;
    Json::Value root;
    reader.parse(pJSON, root);

    JUB_TX_TRX tx;

    tx.ref_block_bytes = (char *) root["TRX"]["pack"]["ref_block_bytes"].asCString();
    tx.ref_block_hash = (char *) root["TRX"]["pack"]["ref_block_hash"].asCString();
    tx.ref_block_num = nullptr;
    tx.data = nullptr;
    tx.expiration = (char *) root["TRX"]["pack"]["expiration"].asCString();
    tx.timestamp = (char *) root["TRX"]["pack"]["timestamp"].asCString();
    tx.fee_limit = (char *) root["TRX"]["pack"]["fee_limit"].asCString();

    int numType = root["TRX"]["contracts"]["type"].asUInt();
    const char *sType = "1";

    std::vector<JUB_CONTRACT_TRX> contractTrxs;
    JUB_CONTRACT_TRX contractTrx;
    contractTrx.type = JUB_ENUM_TRX_CONTRACT_TYPE::NS_ITEM_TRX_CONTRACT;
    switch (numType) {
        case 1: {
            contractTrx.type = JUB_ENUM_TRX_CONTRACT_TYPE::XFER_CONTRACT;
            sType = "1";
            JUB_XFER_CONTRACT_TRX transfer;
            transfer.owner_address = (char *) root["TRX"]["contracts"]["owner_address"].asCString();
            transfer.to_address = (char *) root["TRX"]["contracts"][sType]["to_address"].asCString();
            transfer.amount = root["TRX"]["contracts"][sType]["amount"].asUInt64();
            contractTrx.transfer = transfer;
            break;
        }
        case 2: {
            contractTrx.type = JUB_ENUM_TRX_CONTRACT_TYPE::XFER_ASSET_CONTRACT;
            sType = "2";
            JUB_XFER_ASSET_CONTRACT_TRX transferAsset;
            transferAsset.owner_address = (char *) root["TRX"]["contracts"]["owner_address"].asCString();
            transferAsset.asset_name = (char *) root["TRX"]["contracts"][sType]["asset_name"].asCString();
            transferAsset.to_address = (char *) root["TRX"]["contracts"][sType]["to_address"].asCString();
            transferAsset.amount = root["TRX"]["contracts"][sType]["amount"].asUInt64();
            contractTrx.transferAsset  = transferAsset;
            break;
        }
        case 30: {
            contractTrx.type = JUB_ENUM_TRX_CONTRACT_TYPE::CREATE_SMART_CONTRACT;
            sType = "30";
            JUB_CREATE_SMART_CONTRACT_TRX createSmart;
            createSmart.owner_address = (char *) root["TRX"]["contracts"]["owner_address"].asCString();
            createSmart.call_token_value = root["TRX"]["contracts"][sType]["call_token_value"].asUInt64();
            createSmart.token_id = root["TRX"]["contracts"][sType]["token_id"].asUInt64();
            createSmart.bytecode = (char *) root["TRX"]["contracts"][sType]["bytecode"].asCString();
            contractTrx.createSmart  = createSmart;
            break;
        }
        case 31: {
            contractTrx.type = JUB_ENUM_TRX_CONTRACT_TYPE::TRIG_SMART_CONTRACT;
            sType = "31";
            JUB_TRIG_SMART_CONTRACT_TRX triggerSmart;
            triggerSmart.owner_address = (char *) root["TRX"]["contracts"]["owner_address"].asCString();
            triggerSmart.contract_address = (char *) root["TRX"]["contracts"][sType]["contract_address"].asCString();
            triggerSmart.call_value = root["TRX"]["contracts"][sType]["call_value"].asUInt64();
            triggerSmart.data = (char *) root["TRX"]["contracts"][sType]["data"].asCString();
            triggerSmart.call_token_value = root["TRX"]["contracts"][sType]["call_token_value"].asUInt64();
            triggerSmart.token_id = root["TRX"]["contracts"][sType]["token_id"].asUInt64();
            contractTrx.triggerSmart  = triggerSmart;
            break;
        }
        default:
            break;
    }
    contractTrxs.push_back(contractTrx);
    tx.contracts = &contractTrxs[0];
    tx.contractCount = 1;

    char *packedContractInPB = NULL;
    JUB_RV rv = JUB_PackContractTRX(static_cast<JUB_UINT16>(contextID), tx, &packedContractInPB);

    if (rv != JUBR_OK) {
        errorCode = static_cast<int>(rv);
        return NULL;
    }
    size_t len = strlen(reinterpret_cast<const char *>(packedContractInPB));
    jbyteArray contractByteArray = env->NewByteArray(len);
    env->SetByteArrayRegion(contractByteArray, 0, len, reinterpret_cast<jbyte *>(packedContractInPB));
    env->ReleaseByteArrayElements(contractByteArray,env->GetByteArrayElements(contractByteArray,JNI_FALSE), 0);
    return contractByteArray;
}

//===================================== BIO ============================================

//JNIEXPORT jint JNICALL
//native_identityVerify(JNIEnv *env, jclass obj, jlong contextID, jint verifyMode,
//                      jlongArray retryTimes) {
//    JUB_ULONG_PTR pRetryTimes = reinterpret_cast<JUB_ULONG_PTR>(env->GetLongArrayElements(
//            retryTimes, NULL));
//    JUB_ENUM_IDENTITY_VERIFY_MODE mode;
//    switch (verifyMode) {
//        case 1:
//            mode = JUB_ENUM_IDENTITY_VERIFY_MODE::VIA_DEVICE;
//            break;
//        case 2:
//            mode = JUB_ENUM_IDENTITY_VERIFY_MODE::VIA_9GRIDS;
//            break;
//        case 3:
//            mode = JUB_ENUM_IDENTITY_VERIFY_MODE::VIA_APDU;
//            break;
//        case 4:
//            mode = JUB_ENUM_IDENTITY_VERIFY_MODE::VIA_FPGT;
//            break;
//        default:
//            mode = JUB_ENUM_IDENTITY_VERIFY_MODE::VIA_9GRIDS;
//            break;
//    }
//    JUB_RV ret = JUB_IdentityVerify(static_cast<JUB_UINT16>(contextID), mode, pRetryTimes);
//    if (ret != JUBR_OK) {
//        LOG_ERR("JUB_IdentityVerify: %08x", ret);
//    }
//    env->ReleaseLongArrayElements(retryTimes, reinterpret_cast<jlong *>(pRetryTimes), 0);
//    return static_cast<jint>(ret);
//}

JNIEXPORT jint JNICALL
native_identityVerifyPIN(JNIEnv *env, jclass obj, jlong deviceID, jint verifyMode, jstring jPin,
                         jlongArray retryTimes) {
    JUB_CHAR_PTR pPin = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jPin, NULL));
    JUB_ULONG_PTR pRetryTimes = reinterpret_cast<JUB_ULONG_PTR>(env->GetLongArrayElements(
            retryTimes, NULL));
    JUB_ENUM_IDENTITY_VERIFY_MODE mode;
    switch (verifyMode) {
//        case 1:
//            mode = JUB_ENUM_IDENTITY_VERIFY_MODE::VIA_DEVICE;
//            break;
        case 2:
            mode = JUB_ENUM_IDENTITY_VERIFY_MODE::VIA_9GRIDS;
            break;
//        case 3:
//            mode = JUB_ENUM_IDENTITY_VERIFY_MODE::VIA_APDU;
//            break;
        case 4:
            mode = JUB_ENUM_IDENTITY_VERIFY_MODE::VIA_FPGT;
            break;
        default:
            mode = JUB_ENUM_IDENTITY_VERIFY_MODE::VIA_9GRIDS;
            break;
    }
    JUB_RV ret = JUB_IdentityVerifyPIN(static_cast<JUB_UINT16>(deviceID), mode, pPin, pRetryTimes);
    if (ret != JUBR_OK) {
        LOG_ERR("JUB_IdentityVerifyPIN: %08x", ret);
    }
    env->ReleaseStringUTFChars(jPin, reinterpret_cast<const char *>(pPin));
    env->ReleaseLongArrayElements(retryTimes, reinterpret_cast<jlong *>(pRetryTimes), 0);
    return static_cast<jint>(ret);
}

JNIEXPORT jint JNICALL native_identityShowNineGrids(JNIEnv *env, jclass obj, jlong deviceID) {
    JUB_UINT16 deviceId = deviceID;
    JUB_RV rv = JUB_IdentityShowNineGrids(deviceId);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_IdentityShowNineGrids rv: %08x", rv);
    }
    return rv;
}


JNIEXPORT jint JNICALL native_identityCancelNineGrids(JNIEnv *env, jclass obj, jlong deviceID) {
    JUB_UINT16 deviceId = deviceID;
    JUB_RV rv = JUB_IdentityCancelNineGrids(deviceId);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_IdentityCancelNineGrids rv: %08x", rv);
    }
    return rv;
}

JNIEXPORT jstring JNICALL
native_enrollFingerprint(JNIEnv *env, jclass obj, jlong deviceID, jlong timeOut, jbyteArray fgptIndex) {
    JUB_UINT16 deviceId = deviceID;
    JUB_BYTE *pFgptIndex = (JUB_BYTE *) env->GetByteArrayElements(fgptIndex, NULL);
    JUB_ULONG pTimes;
    JUB_BYTE pFgptID;
    JUB_RV rv = JUB_EnrollFingerprint(deviceId,timeOut, pFgptIndex,
                                      &pTimes, &pFgptID);
    if (rv != JUBR_OK) {
        errorCode = rv;
        LOG_ERR("JUB_EnrollFingerprint rv: %08x", rv);
        return NULL;
    }
    errorCode = 0;
    env->ReleaseByteArrayElements(fgptIndex, reinterpret_cast<jbyte *>(pFgptIndex), 0);
    Json::FastWriter writer;
    Json::Value root;
    Json::Int64 tmstp = pTimes;
    root["times"] = tmstp;
    root["fgptID"] = pFgptID;
    jstring result = env->NewStringUTF(writer.write(root).c_str());
    return result;
}

JNIEXPORT jstring JNICALL native_enumFingerprint(JNIEnv *env, jclass obj, jlong deviceID) {
    JUB_UINT16 deviceId = deviceID;
    JUB_CHAR_PTR pFgptList = nullptr;
    JUB_RV rv = JUB_EnumFingerprint(deviceId, &pFgptList);
    if (rv != JUBR_OK) {
        errorCode = rv;
        LOG_ERR("JUB_EnumFingerprint rv: %08x", rv);
        return NULL;
    }
    jstring fgptList = env->NewStringUTF(pFgptList);

    return fgptList;
}

JNIEXPORT jint JNICALL native_eraseFingerprint(JNIEnv *env, jclass obj, jlong deviceID, jlong timeOut) {
    JUB_UINT16 deviceId = deviceID;
    JUB_RV rv = JUB_EraseFingerprint(deviceId,timeOut);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_EraseFingerprint rv: %08x", rv);
    }
    return rv;
}

JNIEXPORT jint JNICALL
native_deleteFingerprint(JNIEnv *env, jclass obj, jlong deviceID, jlong timeOut, jbyte fgptID) {
    JUB_UINT16 deviceId = deviceID;
    JUB_RV rv = JUB_DeleteFingerprint(deviceId, timeOut, fgptID);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_DeleteFingerprint rv: %08x", rv);
    }
    return rv;
}


JNIEXPORT jint JNICALL
native_verifyFingerprint(JNIEnv *env, jclass obj, jlong contextID, jlongArray retryTimes) {
    JUB_ULONG_PTR pRetryTimes = reinterpret_cast<JUB_ULONG_PTR>(env->GetLongArrayElements(
            retryTimes, NULL));
    JUB_RV ret = JUB_VerifyFingerprint(static_cast<JUB_UINT16>(contextID), pRetryTimes);
    if (ret != JUBR_OK) {
        LOG_ERR("JUB_VerifyFingerprint: %08x", ret);
    }
    env->ReleaseLongArrayElements(retryTimes, reinterpret_cast<jlong *>(pRetryTimes), 0);
    return static_cast<jint>(ret);
}


//=======================================================================================


/**
 * JNINativeMethod由三部分组成:
 * (1)Java中的函数名;
 * (2)函数签名,格式为(输入参数类型)返回值类型;
 * (3)native函数名
 */
static JNINativeMethod gMethods[] = {
        {
                "nativeGetErrorCode",
                "()I",
                (void *) native_getErrorCode
        },
        {
                "nativeInitDevice",
                "()I",
                (void *) native_initDevice
        },
        {
                "nativeStartScan",
                "(Lcom/legendwd/hyperpay/main/hardwarewallet/jubnative/InnerScanCallback;)I",
                //"(Lcom/jubiter/sdk/demo/jubnative/InnerScanCallback;)I",
                (void *) native_startScan
        },
        {
                "nativeStopScan",
                "()I",
                (void *) native_stopScan
        },
        {
                "nativeConnectDevice",
                "(Ljava/lang/String;Ljava/lang/String;I[JILcom/legendwd/hyperpay/main/hardwarewallet/jubnative/InnerDiscCallback;)I",
                (void *) native_connectDevice
        },
        {
                "nativeDisconnect",
                "(J)I",
                (void *) native_disconnectDevice
        },
        {
                "nativeIsConnected",
                "(J)I",
                (void *) native_isConnectDevice
        },
        {
                "nativeShowVirtualPwd",
                "(J)I",
                (void *) native_show
        },
        {
                "nativeGetDeviceType",
                "(J)Ljava/lang/String;",
                (void *) native_getDeviceType
        },
        {
                "nativeVerifyPIN",
                "(J[B)I",
                (void *) native_verifyPIN
        },
        {
                "nativeGetDeviceInfo",
                "(Lcom/legendwd/hyperpay/main/hardwarewallet/jubnative/utils/JUB_DEVICE_INFO;J)I",
                //"(Lcom/jubiter/sdk/demo/jubnative/utils/JUB_DEVICE_INFO;J)I",
                (void *) native_GetDeviceInfo
        },
        {
                "nativeSendApdu",
                "(JLjava/lang/String;)Ljava/lang/String;",
                (void *) native_sendAPDU
        },
        {
                "nativeGetDeviceCert",
                "(J)Ljava/lang/String;",
                (void *) native_GetDeviceCert
        },
        {
                "nativeEnumApplets",
                "(J)Ljava/lang/String;",
                (void *) native_EnumApplets
        },
        {
                "nativeGetAppletVersion",
                "(JLjava/lang/String;)Ljava/lang/String;",
                (void *) native_GetAppletVersion
        },
        {
                "nativeBTCCreateContext",
                "([IZLjava/lang/String;J)I",
                (void *) native_BTCCreateContext
        },
        {
                "nativeBTCGetAddress",
                "(JZLjava/lang/String;)[Ljava/lang/String;",
                (void *) native_BTCGetAddress
        },
        {
                "nativeBTCGetMainHDNode",
                "(J)Ljava/lang/String;",
                (void *) native_BTCGetMainHDNode
        },
        {
                "nativeBTCShowAddress",
                "(JIIZ)Ljava/lang/String;",
                (void *) native_BTC_ShowAddress
        },
        {
                "nativeBTCSetMyAddress",
                "(JIIZ)Ljava/lang/String;",
                (void *) native_BTC_SetMyAddress
        },
        {
                "nativeSetTimeOut",
                "(JI)I",
                (void *) native_SetTimeOut
        },
        {
                "nativeBTCTransaction",
                "(JZLjava/lang/String;)Ljava/lang/String;",
                (void *) native_BTCTransaction
        },
        {
                "nativeUSDTTransaction",
                "(JLjava/lang/String;)Ljava/lang/String;",
                (void *) native_USDTTransaction
        },
        {
                "nativeQRC20Transaction",
                "(JLjava/lang/String;)Ljava/lang/String;",
                (void *) native_QRC20Transaction
        },
        {
                "nativeParseTransaction",
                "(JLjava/lang/String;)Ljava/lang/String;",
                (void *) native_ParseTransactionRaw
        },
        {
                "nativeETHCreateContext",
                "([ILjava/lang/String;J)I",
                (void *) native_ETHCreateContext
        },
        {
                "nativeETHTransaction",
                "(JLjava/lang/String;)Ljava/lang/String;",
                (void *) native_ETH_Transaction
        },
        {
                "nativeETHERC20Transaction",
                "(JLjava/lang/String;)Ljava/lang/String;",
                (void *) native_ETH_ERC20_Transaction
        },
        {
                "nativeETHBuildERC20Abi",
                "(JLjava/lang/String;)Ljava/lang/String;",
                (void *) native_ETH_BuildERC20Abi
        },
        {
                "nativeETHSetContrAddr",
                "(JLjava/lang/String;)I",
                (void *) native_ETH_SetContrAddr
        },
        {
                "nativeETHShowAddress",
                "(JI)Ljava/lang/String;",
                (void *) native_ETHShowAddress
        },
        {
                "nativeETHGetAddress",
                "(JLjava/lang/String;)[Ljava/lang/String;",
                (void *) native_ETHGetAddress
        },
        {
                "nativeETHGetMainHDNode",
                "(J)Ljava/lang/String;",
                (void *) native_ETHGetMainHDNode
        },
        {
                "nativeETHSignContract",
                "(JLjava/lang/String;)Ljava/lang/String;",
                (void *) native_ETH_SignContract
        },
//        {
//                "nativeETHBuildContractWithAddrAbi",
//                "(JLjava/lang/String;)Ljava/lang/String;",
//                (void *) native_ETH_BuildContractWithAddrAbi
//        },
        {
                "nativeETHBuildContractWithAddrAmtDataAbi",
                "(JLjava/lang/String;)Ljava/lang/String;",
                (void *) native_ETH_BuildContractWithAddrAmtDataAbi
        },
        {
                "nativeETHBuildContractWithTxIDAbi",
                "(JLjava/lang/String;)Ljava/lang/String;",
                (void *) native_ETH_BuildContractWithTxIDAbi
        },
//        {
//                "nativeETHBuildContractWithAmtAbi",
//                "(JLjava/lang/String;)Ljava/lang/String;",
//                (void *) native_ETH_BuildContractWithAmtAbi
//        },
        {
                "nativeEnumSupportCoins",
                "(J)Ljava/lang/String;",
                (void *) native_EnumSupportCoins
        },
        {
                "nativeCancelVirtualPwd",
                "(J)I",
                (void *) native_CancelVirtualPwd
        },
        {
                "nativeClearContext",
                "(J)I",
                (void *) native_ClearContext
        },
        {
                "nativeQueryBattery",
                "(J[I)I",
                (void *) native_QueryBattery
        },
        {
                "nativeIsInitialize",
                "(J)I",
                (void *) native_IsInitialize
        },
        {
                "nativeIsBootLoader",
                "(J)I",
                (void *) native_IsBootLoader
        },
        {
                "nativeGetVersion",
                "()Ljava/lang/String;",
                (void *) native_GetVersion
        },
        {
                "nativeETHSetMyAddress",
                "(JII)Ljava/lang/String;",
                (void *) native_SetMyAddressETH
        },
        // EOS
        {
                "nativeEOSCreateContext",
                "([ILjava/lang/String;J)I",
                (void *) native_EOSCreateContext
        },
        {
                "nativeEOSGetAddress",
                "(JII)Ljava/lang/String;",
                (void *) native_EOSGetAddress
        },
        {
                "nativeEOSShowAddress",
                "(JII)Ljava/lang/String;",
                (void *) native_EOSShowAddress
        },
        {
                "nativeEOSSetMyAddress",
                "(JII)Ljava/lang/String;",
                (void *) native_EOSSetMyAddress
        },
        {
                "nativeEOSTransaction",
                "(JLjava/lang/String;)Ljava/lang/String;",
                (void *) native_EOSTransaction
        },
        // XRP
        {
                "nativeXRPCreateContext",
                "([ILjava/lang/String;J)I",
                (void *) native_XRPCreateContext
        },
        {
                "nativeXRPGetAddress",
                "(JII)Ljava/lang/String;",
                (void *) native_XRPGetAddress
        },
        {
                "nativeXRPGetHDNode",
                "(JIIZ)Ljava/lang/String;",
                (void *) native_XRPGetHDNode
        },
        {
                "nativeXRPShowAddress",
                "(JII)Ljava/lang/String;",
                (void *) native_XRPShowAddress
        },
        {
                "nativeXRPSetMyAddress",
                "(JII)Ljava/lang/String;",
                (void *) native_XRPSetMyAddress
        },
        {
                "nativeXRPTransaction",
                "(JLjava/lang/String;)Ljava/lang/String;",
                (void *) native_XRPTransaction
        },
        // TRX
        {
                "nativeTRXCreateContext",
                "([ILjava/lang/String;J)I",
                (void *) native_TRXCreateContext
        },
        {
                "nativeTRXGetAddress",
                "(JII)Ljava/lang/String;",
                (void *) native_TRXGetAddress
        },
        {
                "nativeTRXGetHDNode",
                "(JIIZ)Ljava/lang/String;",
                (void *) native_TRXGetHDNode
        },
        {
                "nativeTRXShowAddress",
                "(JII)Ljava/lang/String;",
                (void *) native_TRXShowAddress
        },
        {
                "nativeTRXSetMyAddress",
                "(JII)Ljava/lang/String;",
                (void *) native_TRXSetMyAddress
        },
        {
                "nativeTRXTransaction",
                "(JLjava/lang/String;[B)Ljava/lang/String;",
                (void *) native_TRXTransaction
        },
        {
                "nativeTRXBuildTRC20Abi",
                "(JLjava/lang/String;)Ljava/lang/String;",
                (void *) native_TRXBuildTRC20Abi
        },
        {
                "nativeTRXSetTRC10Asset",
                "(JLjava/lang/String;)I",
                (void *) native_TRXSetTRC10Asset
        },
        {
                "nativeTRXPackContract",
                "(JLjava/lang/String;)[B",
                (void *) native_TRXPackContract
        },
        //=============================================== Bio ================================================
//        {
//                "nativeIdentityVerify",
//                "(JI[J)I",
//                (void *) native_identityVerify
//        },
        {
                "nativeIdentityVerifyPIN",
                "(JILjava/lang/String;[J)I",
                (void *) native_identityVerifyPIN
        },
        {
                "nativeIdentityShowNineGrids",
                "(J)I",
                (void *) native_identityShowNineGrids
        },
        {
                "nativeIdentityCancelNineGrids",
                "(J)I",
                (void *) native_identityCancelNineGrids
        },
        {
                "nativeEnrollFingerprint",
                "(JJ[B)Ljava/lang/String;",
                (void *) native_enrollFingerprint
        },
        {
                "nativeEnumFingerprint",
                "(J)Ljava/lang/String;",
                (void *) native_enumFingerprint
        },
        {
                "nativeEraseFingerprint",
                "(JJ)I",
                (void *) native_eraseFingerprint
        },
        {
                "nativeDeleteFingerprint",
                "(JJB)I",
                (void *) native_deleteFingerprint
        },
        {
                "nativeVerifyFingerprint",
                "(J[J)I",
                (void *) native_verifyFingerprint
        },
#ifdef HC
        {
                "nativeHCCreateContext",
                "([IZLjava/lang/String;J)I",
                (void *) native_HCCreateContext
        },
        {
                "nativeHCGetAddress",
                "(JLjava/lang/String;)[Ljava/lang/String;",
                (void *) native_HCGetAddress
        },
        {
                "nativeHCShowAddress",
                "(JII)Ljava/lang/String;",
                (void *) native_HCShowAddress
        },
        {
                "nativeHCGetMainHDNode",
                "(J)Ljava/lang/String;",
                (void *) native_HCGetMainHDNode
        },
        {
                "nativeHCTransaction",
                "(JLjava/lang/String;)Ljava/lang/String;",
                (void *) native_HCTransaction
        },

#endif
};


//#define NATIVE_API_CLASS "com/jubiter/sdk/demo/jubnative/NativeApi"

// HC Wallet钱包的class path
#define NATIVE_API_CLASS "com/legendwd/hyperpay/main/hardwarewallet/jubnative/NativeApi"
/**
 * JNI_OnLoad 默认会在 System.loadLibrary 过程中自动调用到，因而可利用此函数，进行动态注册
 * JNI 版本的返回视对应 JDK 版本而定
 */
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = NULL;
    jint ret = JNI_FALSE;


    // 获取 env 指针
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        LOG_ERR(">>>> get env fail");
        return ret;
    }

    jint version = env->GetVersion();
    LOG_ERR(">>>> jni verion: %d", version);

    // 保存全局 JVM 以便在动态注册的皆空中使用 env 环境
    ret = env->GetJavaVM(&g_vm);
    if (ret != JNI_OK) {
        LOG_ERR(">>>> GetJavaVM fail");
        return ret;
    }
    LOG_ERR(">>>> GetJavaVM success");

    // 获取类引用
    jclass clazz = env->FindClass(NATIVE_API_CLASS);
    if (clazz == NULL) {
        LOG_ERR(">>>> clazz == NULL");
        return ret;
    }
    LOG_ERR(">>>> FindClass success");

    // 注册 JNI 方法
    if (env->RegisterNatives(clazz, gMethods, sizeof(gMethods) / sizeof(gMethods[0])) < JNI_OK) {
        LOG_ERR(">>>> RegisterNatives fail");
        return ret;
    }
    LOG_ERR(">>>> jni onload success");

    // 成功
    return JNI_VERSION_1_6;
}