package com.legendwd.hyperpay.main.hardwarewallet.jubnative.utils;

import com.google.gson.annotations.SerializedName;

/**
 * 签名类型
 *
 * @author fengshuo
 * @date 2019/8/16
 * @time 19:22
 */
public enum MultiSigType {

    @SerializedName("0")
    P2PKH,
    @SerializedName("1")
    RETURN0,
    @SerializedName("2")
    P2SH_MULTISIG,
    ;

}
