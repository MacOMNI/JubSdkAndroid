package com.jubiter.sdk.demo.impl;

import android.text.TextUtils;


import com.google.gson.Gson;
import com.jubiter.sdk.demo.impl.JubCallback;
import com.jubiter.sdk.demo.utils.ThreadUtils;
import com.legendwd.hyperpay.main.hardwarewallet.jubnative.NativeApi;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

/**
 * @Date 2020-09-06  15:56
 * @Author ZJF
 * @Version 1.0
 */
public class BioApi {
    private long deviceHandle;

    public BioApi() {
    }

    public void setDeviceHandle(long deviceHandle) {
        this.deviceHandle = deviceHandle;
    }

    public void enumFingerprint(final JubCallback<List<String>, Void> callback) {
        ThreadUtils.execute(new Runnable() {
            @Override
            public void run() {
                String fingerList = NativeApi.nativeEnumFingerprint(deviceHandle);
                if (!TextUtils.isEmpty(fingerList)) {
                    String[] split = fingerList.split(" ");
                    final List<String> list = new ArrayList<>(Arrays.asList(split));
                    ThreadUtils.toMainThread(new Runnable() {
                        @Override
                        public void run() {
                            callback.onSuccess(list, null);
                        }
                    });
                    return;
                }
                final int code = NativeApi.nativeGetErrorCode();
                ThreadUtils.toMainThread(new Runnable() {
                    @Override
                    public void run() {
                        if (code == 0) {
                            List<String> list = new ArrayList<>();
                            callback.onSuccess(list, null);
                        } else {
                            callback.onFailed(code);
                        }
                    }
                });
            }
        });
    }

    public void identityVerifyPIN(final int mode, final String pin, final JubCallback<Void, Void> callback) {
        ThreadUtils.execute(new Runnable() {
            @Override
            public void run() {
                final long[] retry = new long[1];
                final int ret = NativeApi.nativeIdentityVerifyPIN(deviceHandle, mode, pin, retry);
                ThreadUtils.toMainThread(new Runnable() {
                    @Override
                    public void run() {
                        if (ret == 0) {
                            callback.onSuccess(null, null);
                        } else {
                            callback.onFailed((int) retry[0]);
                        }
                    }
                });
            }
        });
    }

    public void identityShowNineGrids(final JubCallback<Void, Void> callback) {
        ThreadUtils.execute(new Runnable() {
            @Override
            public void run() {
                final int ret = NativeApi.nativeIdentityShowNineGrids(deviceHandle);
                ThreadUtils.toMainThread(new Runnable() {
                    @Override
                    public void run() {
                        if (ret == 0) {
                            callback.onSuccess(null, null);
                        } else {
                            callback.onFailed(ret);
                        }
                    }
                });
            }
        });
    }

    public void identityCancelNineGrids(final JubCallback<Void, Void> callback) {
        ThreadUtils.execute(new Runnable() {
            @Override
            public void run() {
                final int ret = NativeApi.nativeIdentityCancelNineGrids(deviceHandle);
                ThreadUtils.toMainThread(new Runnable() {
                    @Override
                    public void run() {
                        if (ret == 0) {
                            callback.onSuccess(null, null);
                        } else {
                            callback.onFailed(ret);
                        }
                    }
                });
            }
        });
    }


    public void enrollFingerprint(final JubCallback<String, Byte> callback) {
        ThreadUtils.execute(new Runnable() {
            @Override
            public void run() {
                final byte[] fgptIndex = new byte[1];
                while (true) {
                    final String result = NativeApi.nativeEnrollFingerprint(deviceHandle, 30, fgptIndex);
                    if (TextUtils.isEmpty(result)) {
                        break;
                    }
                    ThreadUtils.toMainThread(new Runnable() {
                        @Override
                        public void run() {
                            callback.onSuccess(result, fgptIndex[0]);
                        }
                    });
                    BioEnrollBean bean = new Gson().fromJson(result, BioEnrollBean.class);
                    if (bean.getTimes() == fgptIndex[0]) {
                        break;
                    }
                }
                final int code = NativeApi.nativeGetErrorCode();
                ThreadUtils.toMainThread(new Runnable() {
                    @Override
                    public void run() {
                        if (code == 0) {
                            callback.onSuccess(null, null);
                        } else {
                            callback.onFailed(code);
                        }
                    }
                });
            }
        });
    }

    public void deleteFingerprint(final byte id, final JubCallback<Void, Void> callback) {
        ThreadUtils.execute(new Runnable() {
            @Override
            public void run() {
                final int ret = NativeApi.nativeDeleteFingerprint(deviceHandle, 30, id);
                ThreadUtils.toMainThread(new Runnable() {
                    @Override
                    public void run() {
                        if (ret == 0) {
                            callback.onSuccess(null, null);
                        } else {
                            callback.onFailed(ret);
                        }
                    }
                });
            }
        });
    }

    public void eraseFingerprint(final JubCallback<Void, Void> callback) {
        ThreadUtils.execute(new Runnable() {
            @Override
            public void run() {
                final int ret = NativeApi.nativeEraseFingerprint(deviceHandle, 30);
                ThreadUtils.toMainThread(new Runnable() {
                    @Override
                    public void run() {
                        if (ret == 0) {
                            callback.onSuccess(null, null);
                        } else {
                            callback.onFailed(ret);
                        }
                    }
                });
            }
        });
    }

    public void verifyFingerprint(final long contextID, final JubCallback<Void, Void> callback) {
        ThreadUtils.execute(new Runnable() {
            @Override
            public void run() {
                final long[] retry = new long[1];
                final int ret = NativeApi.nativeVerifyFingerprint(contextID, retry);
                ThreadUtils.toMainThread(new Runnable() {
                    @Override
                    public void run() {
                        if (ret == 0) {
                            callback.onSuccess(null, null);
                        } else {
                            callback.onFailed(ret);
                        }
                    }
                });
            }
        });
    }
}
