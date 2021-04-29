package com.legendwd.hyperpay.main.hardwarewallet.jubnative;

import com.legendwd.hyperpay.main.hardwarewallet.jubnative.utils.JUB_DEVICE_INFO;

/**
 * @author fengshuo
 * @date 2018/3/12
 * @time 13:26
 */

public class NativeApi {

    static {
        System.loadLibrary("coreNDK");
    }

    /**
     * 当native接口的返回值不是int时，需要调用该接口获取错误码
     *
     * @return
     */
    public static native int nativeGetErrorCode();


    //*********************************** 蓝牙接口 *****************************************

    public static native int nativeInitDevice();

    public static native int nativeStartScan(InnerScanCallback scanCallback);

    public static native int nativeStopScan();

    public static native int nativeConnectDevice(String devName, String address, int devType, long[] handle, int timeout, InnerDiscCallback discCallback);

    public static native int nativeDisconnect(long deviceHandle);

    public static native int nativeIsConnected(long deviceHandle);

    // ********************************* 功能接口 ********************************************

    public static native String nativeGetDeviceType(long deviceHandle);

    public static native int nativeShowVirtualPwd(long contextID);

    public static native int nativeCancelVirtualPwd(long contextID);

    public static native int nativeVerifyPIN(long contextID, byte[] pin);

    public static native int nativeGetDeviceInfo(JUB_DEVICE_INFO info, long deviceHandle);

    public static native String nativeSendApdu(long deviceHandle, String apduStr);

    public static native String nativeGetDeviceCert(long deviceHandle);

    public static native String nativeEnumApplets(long deviceHandle);

    public static native String nativeGetAppletVersion(long deviceHandle, String appID);

    public static native int nativeSetTimeOut(long contextID, int timeout);

    public static native String nativeEnumSupportCoins(long deviceHandle);

    public static native int nativeClearContext(long contextID);

    /**
     * 查询硬件电量
     *
     * @param deviceHandle 设备句柄
     * @param battery      出参：设备电量（十进制）
     * @return 接口是否调用成功
     */
    public static native int nativeQueryBattery(long deviceHandle, int[] battery);

    public static native int nativeIsInitialize(long deviceHandle);

    public static native int nativeIsBootLoader(long deviceHandle);

    public static native String nativeGetVersion();

    // ********************************* BTC ********************************************

    private static final  boolean USE_LEGACY_ADDRESS = false;

    //// 只要不涉及多重签名，contextBTC只需配置 p2sh_segwit、main_path 两项

    /**
     * @param contextIDs
     * @param isMultiSig   是否是多重签名
     *                     false: json只需配置 p2sh_segwit、main_path
     *                     true: 全部配置，具体值根据多签的要求填充
     * @param json
     * @param deviceHandle
     * @return
     */
    public static native int nativeBTCCreateContext(int[] contextIDs, boolean isMultiSig, String json, long deviceHandle);

    /**
     * @param contextID
     * @param change       是否是找零地址；0：非找零地址，1：找零地址，2：其他
     * @param index
     * @param useLegacyAddress  地址显示为 Legacy 样式；
     * @return
     */
    public static native String nativeBTCShowAddress(long contextID, int change, int index, boolean useLegacyAddress);

    public static String nativeBTCShowAddress(long contextID, int change, int index) {
        return nativeBTCShowAddress(contextID, change, index, USE_LEGACY_ADDRESS);
    }

    /**
     * @param contextID
     * @param change       是否是找零地址；0：非找零地址，1：找零地址，2：其他
     * @param index
     * @return
     */
    public static native String nativeBTCSetMyAddress(long contextID, int change, int index, boolean useLegacyAddress);

    public static String nativeBTCSetMyAddress(long contextID, int change, int index) {
        return nativeBTCSetMyAddress(contextID, change, index, USE_LEGACY_ADDRESS);
    }

    public static native String[] nativeBTCGetAddress(long contextID, boolean useLegacyAddress, String json);

    public static String[] nativeBTCGetAddress(long contextID, String json) {
        return nativeBTCGetAddress(contextID, USE_LEGACY_ADDRESS, json);
    }

    public static native String nativeBTCGetMainHDNode(long contextID);

    /**
     * 普通签名：
     * multisig ：P2PKH(0)
     * <p>
     * 多重签名：
     * multisig ：P2SH_MULTISIG(2)
     *
     * @param contextID
     * @param json
     * @return
     */
    public static native String nativeBTCTransaction(long contextID, boolean useLegacyAddress, String json);

    public static String nativeBTCTransaction(long contextID, String json) {
        return nativeBTCTransaction(contextID, USE_LEGACY_ADDRESS, json);
    }

    /**
     * @param contextID
     * @param json
     * @return
     */
    public static native String nativeQRC20Transaction(long contextID, String json);

    /**
     * @param contextID
     * @param json
     * @return
     */
    public static native String nativeUSDTTransaction(long contextID, String json);

    /**
     * 将交易raw转换成json
     *
     * @param contextID
     * @param json         交易raw
     * @return
     */
    public static native String nativeParseTransaction(long contextID, String json);

    // ********************************* ETH ********************************************

    public static native int nativeETHCreateContext(int[] contextIDs, String json, long deviceHandle);

    public static native String nativeETHShowAddress(long contextID, int index);

    public static native String nativeETHSetMyAddress(long contextID, int change, int index);

    public static native int nativeETHSetContrAddr(long contextID, String contractAddress);

    public static native String[] nativeETHGetAddress(long contextID, String json);

    public static native String nativeETHGetMainHDNode(long contextID);

    public static native String nativeETHTransaction(long contextID, String json);

    public static native String nativeETHERC20Transaction(long contextID, String json);

    public static native String nativeETHBuildERC20Abi(long contextID, String json);

    //********************************* ETH multi sign ************************************

    public static native String nativeETHSignContract(long contextID, String json);

    //public static native String nativeETHBuildContractWithAddrAbi(long contextID, String json);

    public static native String nativeETHBuildContractWithAddrAmtDataAbi(long contextID, String json);

    public static native String nativeETHBuildContractWithTxIDAbi(long contextID, String json);

    //public static native String nativeETHBuildContractWithAmtAbi(long contextID, String json);

    //*********************************** HC **********************************************

    public static native int nativeHCCreateContext(int[] contextIDs, boolean isMultiSig, String json, long deviceHandle);

    public static native String[] nativeHCGetAddress(long contextID, String json);

    public static native String nativeHCShowAddress(long contextID, int change, int index);

    public static native String nativeHCGetMainHDNode(long contextID);

    public static native String nativeHCTransaction(long contextID, String json);

    //*********************************** EOS **********************************************
    public static native int nativeEOSCreateContext(int[] contextIDs, String json, long deviceHandle);

    public static native String nativeEOSShowAddress(long contextID, int change, int index);

    public static native String nativeEOSGetAddress(long contextID, int change, int index);

    public static native String nativeEOSSetMyAddress(long contextID, int change, int index);

    public static native String nativeEOSTransaction(long contextID, String json);

    //*********************************** XRP **********************************************

    public static native int nativeXRPCreateContext(int[] contextIDs, String json, long deviceHandle);

    public static native String nativeXRPShowAddress(long contextID, int change, int index);

    public static native String nativeXRPGetAddress(long contextID, int change, int index);

    public static native String nativeXRPGetHDNode(long contextID, int change, int index, boolean useHex);

    public static native String nativeXRPSetMyAddress(long contextID, int change, int index);

    public static native String nativeXRPTransaction(long contextID, String json);

    //*********************************** TRX **********************************************

    public static native int nativeTRXCreateContext(int[] contextIDs, String json, long deviceHandle);

    public static native String nativeTRXShowAddress(long contextID, int change, int index);

    public static native String nativeTRXGetAddress(long contextID, int change, int index);

    public static native String nativeTRXGetHDNode(long contextID, int change, int index, boolean useHex);

    public static native String nativeTRXSetMyAddress(long contextID, int change, int index);

    public static native String nativeTRXTransaction(long contextID, String json, byte[] packedContractInPb);

    public static native String nativeTRXBuildTRC20Abi(long contextID, String json);

    public static native int nativeTRXSetTRC10Asset(long contextID, String json);

    public static native byte[] nativeTRXPackContract(long contextID, String json);

    //*********************************** FIL **********************************************

    public static native int nativeFILCreateContext(int[] contextIDs, String json, long deviceHandle);

    public static native String nativeFILShowAddress(long contextID, int change, int index);

    public static native String nativeFILGetAddress(long contextID, int change, int index);

    public static native String nativeFILGetHDNode(long contextID, int change, int index, boolean useHex);

    public static native String nativeFILSetMyAddress(long contextID, int change, int index);

    public static native String nativeFILTransaction(long contextID, String json);

    //*********************************** bio **********************************************

    public static native int nativeIdentityVerifyPIN(long deviceHandle, int mode, String pin, long[] retryTimes);

    public static native int nativeIdentityShowNineGrids(long deviceHandle);

    public static native int nativeIdentityCancelNineGrids(long deviceHandle);

    public static native String nativeEnrollFingerprint(long deviceHandle, long timeOut, byte[] fgptIndex);

    public static native String nativeEnumFingerprint(long deviceHandle);

    public static native int nativeEraseFingerprint(long deviceHandle, long timeOut);

    public static native int nativeDeleteFingerprint(long deviceHandle, long timeOut, byte fgptID);

    public static native int nativeVerifyFingerprint(long contextID, long[] retryTimes);
}
