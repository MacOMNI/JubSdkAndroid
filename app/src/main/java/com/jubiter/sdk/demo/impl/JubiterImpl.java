package com.jubiter.sdk.demo.impl;

import android.content.Context;
import android.os.Handler;
import android.os.Looper;
import android.text.TextUtils;
import android.util.Log;

import com.google.gson.Gson;
import com.jubiter.sdk.demo.ble.BluetoothKeyApi;
import com.jubiter.sdk.demo.ble.FTBTDevice;
import com.jubiter.sdk.demo.ble.FtBTConnectCallback;
import com.jubiter.sdk.demo.utils.JSONParseUtils;
import com.jubiter.sdk.demo.utils.StrUtil;
import com.jubiter.sdk.demo.utils.ThreadUtils;
import com.jubiter.sdk.demo.utils.VerifyPinDialog;
import com.legendwd.hyperpay.main.hardwarewallet.jubnative.NativeApi;
import com.legendwd.hyperpay.main.hardwarewallet.jubnative.utils.BriefBtcTransInfo;
import com.legendwd.hyperpay.main.hardwarewallet.jubnative.utils.BtcTransInfo;
import com.legendwd.hyperpay.main.hardwarewallet.jubnative.utils.GsonUtils;
import com.legendwd.hyperpay.main.hardwarewallet.jubnative.utils.JUB_DEVICE_INFO;
import com.legendwd.hyperpay.main.hardwarewallet.jubnative.utils.MultiSigType;
import com.legendwd.hyperpay.main.hardwarewallet.jubnative.utils.MyEnumTypeAdapter;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;


/**
 * @Date 2018/4/19  11:30
 * @Author ZJF
 * @Version 1.0
 */
public class JubiterImpl {

    private Context mContext;
    private BluetoothKeyApi mBluetoothKeyApi;
    private boolean isConnected = false;
    private BioApi mBioApi;

    private Handler mUIHandler = new Handler(Looper.getMainLooper());
    private long mDeviceHandle;

    private JubiterImpl(Context context) {
        mContext = context;
        mBluetoothKeyApi = new BluetoothKeyApi(mContext);
        mBioApi = new BioApi();
    }

    private static JubiterImpl fidoApdu;

    public static JubiterImpl getInstance(Context context) {
        if (fidoApdu == null) {
            fidoApdu = new JubiterImpl(context);
        }

        return fidoApdu;
    }

    public void onDestroy() {
        if (isConnected) {
            mBluetoothKeyApi.disConnect(mDeviceHandle);
        }
        fidoApdu = null;
    }

    public void connect(final FtBTConnectCallback connectCallback) {
        mBluetoothKeyApi.connect(new FtBTConnectCallback() {
            @Override
            public void connected(final FTBTDevice device, int code) {
                if (device == null) {
                    return;
                }
                isConnected = true;
                mDeviceHandle = device.getHandler();
                mBioApi.setDeviceHandle(mDeviceHandle);
                connectCallback.connected(device, code);
            }

            @Override
            public void disConnected() {
                isConnected = false;
                connectCallback.disConnected();
            }
        });
    }

    public int disConnectDevice() {
        return mBluetoothKeyApi.disConnect(mDeviceHandle);
    }

    public void getDeviceInfo(final JubCallback<JUB_DEVICE_INFO, Void> callback) {
        new Thread(new Runnable() {
            @Override
            public void run() {

                final int rv = NativeApi.nativeIsConnected(mDeviceHandle);
                if (rv != 0) {
                    mUIHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            callback.onFailed(rv);
                        }
                    });
                    return;
                }

                final JUB_DEVICE_INFO info = new JUB_DEVICE_INFO();
                final int ret = NativeApi.nativeGetDeviceInfo(info, mDeviceHandle);
                if (ret != 0) {
                    mUIHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            callback.onFailed(ret);
                        }
                    });

                } else {
                    mUIHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            callback.onSuccess(info, null);
                        }
                    });
                }
            }
        }).start();
    }

    public void getDeviceCert(final JubCallback<String, Void> callback) {
        new Thread(new Runnable() {
            @Override
            public void run() {
                final String s = NativeApi.nativeGetDeviceCert(mDeviceHandle);
                mUIHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        if (TextUtils.isEmpty(s)) {
                            callback.onFailed(-1);
                        } else {
                            callback.onSuccess(s, null);
                        }
                    }
                });

            }
        }).start();
    }

    public void getDeviceType(final JubCallback<String, Void> callback) {
        new Thread(new Runnable() {
            @Override
            public void run() {
                final String s = NativeApi.nativeGetDeviceType(mDeviceHandle);
                mUIHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        if (TextUtils.isEmpty(s)) {
                            callback.onFailed(-1);
                        } else {
                            callback.onSuccess(s, null);
                        }
                    }
                });

            }
        }).start();
    }

    public void sendApdu(final String apdu,
                         final JubCallback<String, Void> callback) {
        Log.d("sendAPDU", apdu);
        new Thread(new Runnable() {
            @Override
            public void run() {
                final String result = NativeApi.nativeSendApdu(mDeviceHandle, apdu);
                mUIHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        if (TextUtils.isEmpty(result)) {
                            callback.onFailed(-1);
                        } else {
                            callback.onSuccess(result, null);
                        }
                    }
                });
            }
        }).start();
    }

    public void enumApplets(final JubCallback<List<String>, Void> callback) {
        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    String lists = NativeApi.nativeEnumApplets(mDeviceHandle);
                    String[] split = lists.split(" ");
                    final List<String> result = Arrays.asList(split);
                    mUIHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            callback.onSuccess(result, null);
                        }
                    });
                } catch (Exception e) {
                    mUIHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            callback.onFailed(-1);
                        }
                    });
                }
            }
        }).start();
    }

    public void getAppletVersion(final List<String> appIDs,
                                 final JubCallback<String, Void> callback) {
        new Thread(new Runnable() {
            @Override
            public void run() {
                final StringBuilder sb = new StringBuilder();
                for (String appID : appIDs) {
                    final String s = NativeApi.nativeGetAppletVersion(mDeviceHandle, appID);
                    sb.append("appletID:").append(appID).append("\n  version:").append(s).append("\n");
                }

                mUIHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        callback.onSuccess(sb.toString(), null);
                    }
                });

            }
        }).start();
    }

    public void setTimeOut(final int timeOut, final JubCallback<String, Void> callback) {
        new Thread(new Runnable() {
            @Override
            public void run() {
                int[] contextIDs = new int[1];
                NativeApi.nativeBTCCreateContext(contextIDs, false,
                        JSONParseUtils.getJsonStr(mContext, "testBTC.json"),
                        mDeviceHandle);

                long contextID = contextIDs[0];
                final int ret = NativeApi.nativeSetTimeOut(contextID, timeOut);
                mUIHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        if (ret == 0) {
                            callback.onSuccess("set timeout success", null);
                        } else {
                            callback.onFailed(ret);
                        }
                    }
                });

            }
        }).start();
    }

    // Bio
    public void enumFingerprint(JubCallback<List<String>, Void> callback) {
        mBioApi.enumFingerprint(callback);
    }

    public void identityVerifyPIN(int mode, String pin, JubCallback<Void, Void> callback) {
        mBioApi.identityVerifyPIN(mode, pin, callback);
    }

    public void identityShowNineGrids(JubCallback<Void, Void> callback) {
        mBioApi.identityShowNineGrids(callback);
    }

    public void identityCancelNineGrids(JubCallback<Void, Void> callback) {
        mBioApi.identityCancelNineGrids(callback);
    }


    public void enrollFingerprint(JubCallback<String, Byte> callback) {
        mBioApi.enrollFingerprint(callback);
    }

    public void deleteFingerprint(byte id, JubCallback<Void, Void> callback) {
        mBioApi.deleteFingerprint(id, callback);
    }

    public void eraseFingerprint(JubCallback<Void, Void> callback) {
        mBioApi.eraseFingerprint(callback);
    }

    public void verifyFingerprint(long contextID, JubCallback<Void, Void> callback) {
        mBioApi.verifyFingerprint(contextID, callback);
    }

    // ***************************************** BTC **************************************************

    public void btcGetAddress(final JubCallback<String, String> callback) {
        new Thread(new Runnable() {
            @Override
            public void run() {

                int[] contextIDs = new int[1];
                int rv = NativeApi.nativeBTCCreateContext(contextIDs, false,
                        JSONParseUtils.getJsonStr(mContext, "contextBTC_1.json"),
                        mDeviceHandle);
                if (rv != 0) {
                    final int finalRet = NativeApi.nativeGetErrorCode();
                    mUIHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            callback.onFailed(finalRet);
                        }
                    });
                    return;
                }

                long contextID = contextIDs[0];
                String[] array = NativeApi.nativeBTCGetAddress(contextID, JSONParseUtils.getJsonStr(mContext, "path.json"));
                if (array == null || array.length == 0) {
                    final int finalRet = NativeApi.nativeGetErrorCode();
                    mUIHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            callback.onFailed(finalRet);
                        }
                    });
                    return;
                }

                final StringBuilder builder = new StringBuilder();

                for (int i = 0; i < array.length; i += 2) {
                    builder.append("\n\n xpub: \n").append(array[i])
                            .append("\n\n address: \n").append(array[i + 1]);
                }

                mUIHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        callback.onSuccess(builder.toString(), null);
                    }
                });

            }
        }).start();
    }

    public void btcGetMainHDNode(final JubCallback<String, String> callback) {
        new Thread(new Runnable() {
            @Override
            public void run() {

                int[] contextIDs = new int[1];
                int rv = NativeApi.nativeBTCCreateContext(contextIDs, false,
                        JSONParseUtils.getJsonStr(mContext, "contextBTC.json"),
                        mDeviceHandle);
                if (rv != 0) {
                    final int finalRet = NativeApi.nativeGetErrorCode();
                    mUIHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            callback.onFailed(finalRet);
                        }
                    });
                    return;
                }

                long contextID = contextIDs[0];
                final String mainXpub = NativeApi.nativeBTCGetMainHDNode(contextID);
                if (TextUtils.isEmpty(mainXpub)) {
                    final int finalRet = NativeApi.nativeGetErrorCode();
                    mUIHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            callback.onFailed(finalRet);
                        }
                    });
                    return;
                }
                mUIHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        callback.onSuccess(mainXpub, null);
                    }
                });

            }
        }).start();
    }


    public void btcShowAddress(final int change, final int index, final JubCallback<String, Void> callback) {
        new Thread(new Runnable() {
            @Override
            public void run() {
                int[] contextIDs = new int[1];
                NativeApi.nativeBTCCreateContext(contextIDs, false,
                        JSONParseUtils.getJsonStr(mContext, "contextBTC.json"),
                        mDeviceHandle);
                final long contextID = contextIDs[0];

                Log.d("btcTrans contextID:", " " + contextID);
                final String ret = NativeApi.nativeBTCShowAddress(contextID, change, index);
                if (TextUtils.isEmpty(ret)) {
                    mUIHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            callback.onFailed(NativeApi.nativeGetErrorCode());
                        }
                    });
                    return;
                }
                mUIHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        callback.onSuccess(ret, null);
                    }
                });

            }
        }).start();
    }

    public void btcSetMyAddress(final int change, final int index, final JubCallback<String, Void> callback) {
        new Thread(new Runnable() {
            @Override
            public void run() {
                int[] contextIDs = new int[1];
                NativeApi.nativeBTCCreateContext(contextIDs, false,
                        JSONParseUtils.getJsonStr(mContext, "testBTC.json"),
                        mDeviceHandle);
                final long contextID = contextIDs[0];

                Log.d("btcTrans contextID:", " " + contextID);
                int ret = NativeApi.nativeShowVirtualPwd(contextID);
                if (ret != 0) {
                    final int finalRet = ret;
                    mUIHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            callback.onFailed(finalRet);
                        }
                    });
                    return;
                }
                mUIHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        final VerifyPinDialog dialog = new VerifyPinDialog(mContext, new VerifyPinDialog.callback() {
                            @Override
                            public void onClickListener(String pin) {
                                int ret = 0;
                                if (TextUtils.isEmpty(pin) || pin.length() != 4) {
                                    final int finalRet = ret;
                                    mUIHandler.post(new Runnable() {
                                        @Override
                                        public void run() {
                                            callback.onFailed(finalRet);
                                        }
                                    });
                                    return;
                                }

                                ret = NativeApi.nativeVerifyPIN(contextID, pin.getBytes());

                                if (ret != 0) {
                                    final int finalRet = ret;
                                    mUIHandler.post(new Runnable() {
                                        @Override
                                        public void run() {
                                            callback.onFailed(finalRet);
                                        }
                                    });
                                    return;
                                }
                                final String result = NativeApi.nativeBTCSetMyAddress(contextID, change, index);
                                if (TextUtils.isEmpty(result)) {
                                    mUIHandler.post(new Runnable() {
                                        @Override
                                        public void run() {
                                            callback.onFailed(NativeApi.nativeGetErrorCode());
                                        }
                                    });
                                    return;
                                }
                                mUIHandler.post(new Runnable() {
                                    @Override
                                    public void run() {
                                        callback.onSuccess(result, null);
                                    }
                                });
                            }
                        });
                        dialog.show();
                    }
                });


            }
        }).start();
    }

    public void btcTrans(final JubCallback<String, Void> callback) {
        new Thread(new Runnable() {
            @Override
            public void run() {

                int[] contextIDs = new int[1];
                NativeApi.nativeBTCCreateContext(contextIDs, true,
                        JSONParseUtils.getJsonStr(mContext, "testBTC45_7-7-1.json"),
                        mDeviceHandle);
                final long contextID = contextIDs[0];

                Log.d("btcTrans contextID:", " " + contextID);
                int ret = NativeApi.nativeShowVirtualPwd(contextID);
                if (ret != 0) {
                    final int finalRet = ret;
                    mUIHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            callback.onFailed(finalRet);
                        }
                    });
                    return;
                }

                mUIHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        final VerifyPinDialog dialog = new VerifyPinDialog(mContext, new VerifyPinDialog.callback() {
                            @Override
                            public void onClickListener(String pin) {
                                int ret = 0;
                                if (TextUtils.isEmpty(pin) || pin.length() != 4) {
                                    final int finalRet = ret;
                                    mUIHandler.post(new Runnable() {
                                        @Override
                                        public void run() {
                                            callback.onFailed(finalRet);
                                        }
                                    });
                                    return;
                                }

                                ret = NativeApi.nativeVerifyPIN(contextID, pin.getBytes());
                                if (ret != 0) {
                                    final int finalRet = ret;
                                    mUIHandler.post(new Runnable() {
                                        @Override
                                        public void run() {
                                            callback.onFailed(finalRet);
                                        }
                                    });
                                    return;
                                }
                                signRaw = NativeApi.nativeBTCTransaction(contextID, JSONParseUtils.getJsonStr(mContext, "testBTC45_7-7-1.json"));
                                if (TextUtils.isEmpty(signRaw)) {
                                    mUIHandler.post(new Runnable() {
                                        @Override
                                        public void run() {
                                            callback.onFailed(NativeApi.nativeGetErrorCode());
                                        }
                                    });
                                    return;
                                }
                                Log.d("JuBiter", "多签1: " + signRaw);
                                mUIHandler.post(new Runnable() {
                                    @Override
                                    public void run() {
                                        callback.onSuccess(signRaw, null);
                                    }
                                });
                            }
                        });
                        dialog.show();
                    }
                });

            }
        }).start();
    }

    public void btcTransBio(final JubCallback<String, Void> callback) {
        new Thread(new Runnable() {
            @Override
            public void run() {

                int[] contextIDs = new int[1];
                final int ret = NativeApi.nativeBTCCreateContext(contextIDs, false,
                        JSONParseUtils.getJsonStr(mContext, "testBTC44.json"),
                        mDeviceHandle);
                if(ret!=0){
                    mUIHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            callback.onFailed(ret);
                        }
                    });
                    return;
                }
                final long contextID = contextIDs[0];

                Log.d("btcTrans contextID:", " " + contextID);
                mBioApi.verifyFingerprint(contextID, new JubCallback<Void, Void>() {
                    @Override
                    public void onSuccess(Void aVoid, Void aVoid2) {
                        callback.onSuccess("verifyFingerprint success", null);
                        ThreadUtils.execute(new Runnable() {
                            @Override
                            public void run() {
                                signRaw = NativeApi.nativeBTCTransaction(contextID, JSONParseUtils.getJsonStr(mContext, "testBTC44.json"));
                                NativeApi.nativeIsBootLoader(mDeviceHandle);
                                if (TextUtils.isEmpty(signRaw)) {
                                    mUIHandler.post(new Runnable() {
                                        @Override
                                        public void run() {
                                            callback.onFailed(NativeApi.nativeGetErrorCode());
                                        }
                                    });
                                    return;
                                }
                                mUIHandler.post(new Runnable() {
                                    @Override
                                    public void run() {
                                        callback.onSuccess(signRaw, null);
                                    }
                                });
                            }
                        });
                    }

                    @Override
                    public void onFailed(int errorCode) {

                    }
                });

            }
        }).start();
    }

    public void usdtTrans(final JubCallback<String, Void> callback) {
        new Thread(new Runnable() {
            @Override
            public void run() {

                int[] contextIDs = new int[1];
                NativeApi.nativeBTCCreateContext(contextIDs, false,
                        JSONParseUtils.getJsonStr(mContext, "testUSDT_1.json"),
                        mDeviceHandle);
                final long contextID = contextIDs[0];

                Log.d("usdtTrans contextID:", " " + contextID);
                int ret = NativeApi.nativeShowVirtualPwd(contextID);
                if (ret != 0) {
                    final int finalRet = ret;
                    mUIHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            callback.onFailed(finalRet);
                        }
                    });
                    return;
                }

                mUIHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        final VerifyPinDialog dialog = new VerifyPinDialog(mContext, new VerifyPinDialog.callback() {
                            @Override
                            public void onClickListener(String pin) {
                                int ret = 0;
                                if (TextUtils.isEmpty(pin) || pin.length() != 4) {
                                    final int finalRet = ret;
                                    mUIHandler.post(new Runnable() {
                                        @Override
                                        public void run() {
                                            callback.onFailed(finalRet);
                                        }
                                    });
                                    return;
                                }

                                ret = NativeApi.nativeVerifyPIN(contextID, pin.getBytes());
                                if (ret != 0) {
                                    final int finalRet = ret;
                                    mUIHandler.post(new Runnable() {
                                        @Override
                                        public void run() {
                                            callback.onFailed(finalRet);
                                        }
                                    });
                                    return;
                                }
                                final String raw = NativeApi.nativeUSDTTransaction(contextID,
                                        JSONParseUtils.getJsonStr(mContext, "testUSDT_1.json"));
                                if (TextUtils.isEmpty(signRaw)) {
                                    mUIHandler.post(new Runnable() {
                                        @Override
                                        public void run() {
                                            callback.onFailed(NativeApi.nativeGetErrorCode());
                                        }
                                    });
                                    return;
                                }
                                mUIHandler.post(new Runnable() {
                                    @Override
                                    public void run() {
                                        Log.d("Raw:", raw);
                                        callback.onSuccess(raw, null);
                                    }
                                });
                            }
                        });
                        dialog.show();
                    }
                });

            }
        }).start();
    }

    private String signRaw = "0100000002fbe6cbc1b373ed0f8d29df99bc64880824ad74fdec1731fca586c41a80a4fbc800000000fda60200473044022009ae62d8fd27edd948cced774dbbddfdac32e20e7952801a7892270f1ba61da90220171e17fcc3ae8dc8c55967115065877eb4a6757c5d6367a0e26eecd564e8895201473044022035ce1333c6e10dd60b96f5720335f17fc0824065914a332d364f27cbdcfcc1e5022001a44a3cce7a34594a48836582488869d59db217503118315300a0a332f50e0501483045022100ae02d17a4f4c41faaec49392db16d8a8632c90ec65579c8090d94e2237a6553702200eb44445390f996c09f2be2da744f7d4d1e7764d865b7607e6e2797591558c610147304402203428958571b3c43f0aeba87733550ad817e822f20665cfc60d66c9c3af2b616c0220373567547b1d09cc3ec4f55ce842d32d51a39d341551a87ce8c260bc6eca85fe01483045022100eeeb5a5c7f2ec0c11134e4d41414830a816846c8151d56300309a9cc6e3f8c7a02202e035c9fc3dd76aa4757042f1a728e118bf8ccfef47f367b6bb8e369939e95080147304402207e14441bd41d95660e8988946248b5415bbe86d29e906194518745854cd4f790022020ee0145969ed03d7e4875805c905b6230d99da0896005260ce250f3ac4a8a83014cf15721022e615adb8761a02d3e3989dd693e10629f84d21ddc6d9e3e6e9f75854654495d21027a30fc24843fbe5a7590981a71f088ac6a55da345167c1ad886019ae11bfa096210303915db04a8bb2c44ec52489618a612363c46291143d6117dec01dc40dfec7422103265abb62314e3173d7a79a56370da581437dc7564106dec2062d13665bc3994321033910482f94265d12abf92ad6c356b1890a71bd7ec79b3b545b495067bfa5282221039543179924bbcdf40c1126b4f2e1bf2bb9fcd9678dc243bce64af7aeb2a6519d2103d30f6671979d78a7dfb923894b946e6a3cfe436c715362389a1c7d6a50fb70d657aefffffffffbe6cbc1b373ed0f8d29df99bc64880824ad74fdec1731fca586c41a80a4fbc801000000fda7020048304502210089ab653c5168197b8ddc21ff3a245b3f94321979388dadc5b1ee028bbea080a002201679b6b9c5ea5799aa9641af3b9f6a86ec1249525994854aa6f00fa535c6f9c7014830450221008b5310d4db534b7c3231b24b1f43ee5404d9da8d36c89312e835bd629da7d90902206e9a42467a523a613924a4910e937f5d65c1f2ac1ad5aaa0e3309d02f7dc8a84014730440220098e80c170811d8eccb5221fc4c13fd34baed7a730c3b22d7cc9c495ad5cb94302206a9e49773b9d76f0581f1bec1e72e6ba265f88f041db1b85b594dcf48631ad9d01483045022100bf42b73a5da3b0519c24c75bb8c15f4c046bc475bef28319910d35c04adbcf0d022012b2ca239e4b767ec2fdc6a73f4d57c1801f60df3dcf6d97f1afa70039b3be0701473044022035e9576bc9293e83d9d1b23e1a6a7487d5f22e3cf1c1c09084bdeacaa2df6da102203f29b7e68e7bd80a3fbdb8c1f0b4b061a0a5f28241ae45f4dae6e0c4cceb23ba0147304402204d32298ed126a8a16a9cfa2f86a4cc31a1dd7c211a250618a77eec48aaf3abcc0220076e10163e76fcb73779611d2e8867b01c70ba6cacf1eb8de25f1c8b20aa78a6014cf15721025e02ff004d009e7692f4e06b2909fbbac6f68f4fca374e33842539b4f0bc7c6a21028baace0b233c780284a09de70668e01174c92bbc7f4b2b0dcf1d14961ec620972102a24ccdd5c8ab0a44352214bd9dab224608b6e996d4d052afa17c55c1e33cf54b2103211e16fee6843b420aa3c63b3d6528a668bfd1ec4b9faedcc550bd2d864e143f210332c69a1d783f694817142a1888d04c392ddbc1ce0e80d837a07b7a3b2a9c1fa421035edfdef507761d02b16faba5daa93bc7503510e18a718a2dbac0f20a39ae2ef32103863f0f58382cdaf39258d5b37c03184d0e1fc9b15ef9cc0fb8f6f3d4918be0be57aeffffffff02fc1200000000000017a914ee849c2abfa555faf80849f8f0a825aa544933ad879c0900000000000017a914d2dee1b6232cdbf34dc383d8003993115d5dc55d8700000000";

    /**
     * 以 2-2 多重签名为例
     * <p>
     * 两个支持多重签名的硬件
     * 1. 先 btcTrans 函数进行多重签名的第一次签名，其结果作为下一次签名的数据
     * 2. 重新创建 context
     * 3. 调用 NativeApi.nativeParseTransaction 解析 步骤1的签名结果，返回简要交易信息
     * 4. 构建交易的json数据，数据项的值就是步骤3中解析返回的简要交易信息里的值，其中 input的amount值，需调用者按照preHash从主网获取
     * 5. 将步骤4构建的json数据传给签名函数签名即可。
     *
     * @param callback
     */
    public void btcMultiSigTrans(final JubCallback<String, Void> callback) {
        new Thread(new Runnable() {
            @Override
            public void run() {

                int[] contextIDs = new int[1];
                String contextString = JSONParseUtils.getJsonStr(mContext, "testBTC45_7-7-2.json");
                int rv = NativeApi.nativeBTCCreateContext(contextIDs, true, contextString, mDeviceHandle);
                if (rv != 0) {
                    Log.e("JuBiter", "nativeBTCCreateContext : " + NativeApi.nativeGetErrorCode());
                    return;
                }

                final long contextID = contextIDs[0];

                // 解析上一笔签名交易
                String info = NativeApi.nativeParseTransaction(contextID, signRaw);
                if (TextUtils.isEmpty(info)) {
                    Log.e("JuBiter", "nativeParseTransaction : " + NativeApi.nativeGetErrorCode());
                    return;
                }

                String newInfo = info.replaceAll("\r\n|\n|\r|\t", "");
                Log.d("JuBiter", newInfo);
                BriefBtcTransInfo briefTransInfo = GsonUtils.convertObj(newInfo, BriefBtcTransInfo.class);

                // 实际amount值，需根据preHash值去主链查询相关值
                for (int i = 0, size = briefTransInfo.getInputs().size(); i < size; i++) {
                    BriefBtcTransInfo.Input tmp = briefTransInfo.getInputs().get(i);
                    if (TextUtils.equals(tmp.getPreHash(), "c8fba4801ac486a5fc3117ecfd74ad24088864bc99df298d0fed73b3c1cbe6fb")
                            && tmp.getPreIndex() == 0) {
                        tmp.setAmount(7140);
                    }
                    if (TextUtils.equals(tmp.getPreHash(), "c8fba4801ac486a5fc3117ecfd74ad24088864bc99df298d0fed73b3c1cbe6fb")
                            && tmp.getPreIndex() == 1) {
                        tmp.setAmount(2580);
                    }
                }

                // 按testBTC.json格式重组交易信息
                BtcTransInfo btcTransInfo = new BtcTransInfo();
                btcTransInfo.setVer("01000000");
                btcTransInfo.setP2sh_segwit(false);
                btcTransInfo.setMain_path("m/45'");

                // input
                List<BtcTransInfo.Input> inputList = new ArrayList<>();
                for (int i = 0, size = briefTransInfo.getInputs().size(); i < size; i++) {
                    BriefBtcTransInfo.Input tmp = briefTransInfo.getInputs().get(i);
                    BtcTransInfo.Input input = new BtcTransInfo.Input();
                    input.setAmount(tmp.getAmount());
                    input.setnSequence(tmp.getnSequence());
                    input.setPreHash(tmp.getPreHash());
                    input.setPreIndex(tmp.getPreIndex());
                    if (tmp.getPreIndex() == 0) {
                        input.setBip32_path(new BtcTransInfo.Bip32Path(false, 0)); // todo bipPath
                    } else {
                        input.setBip32_path(new BtcTransInfo.Bip32Path(true, 0));
                    }
                    input.setMultisig(tmp.getMultisig());   // 该值由create context时确认，此处更改值不会被启用
                    inputList.add(input);
                }
                btcTransInfo.setInputs(inputList);

                // output
                List<BtcTransInfo.Output> outputList = new ArrayList<>();
                for (int i = 0, size = briefTransInfo.getOutput().size(); i < size; i++) {
                    BriefBtcTransInfo.Output tmp = briefTransInfo.getOutput().get(i);
                    BtcTransInfo.Output output = new BtcTransInfo.Output();
                    output.setAddress(tmp.getAddress());
                    output.setAmount(tmp.getAmount());
                    if (tmp.getAddress().equalsIgnoreCase("3PSBc75ZVori2BYTnqXAc7twvQqg2MAdnv")) {
                        output.setBip32_path(new BtcTransInfo.Bip32Path(false, 0));
                        output.setChange_address(false); // 指定第一个地址为找零地址，按实际交易来
                    } else {
                        output.setBip32_path(new BtcTransInfo.Bip32Path(true, 0));  // 按实际交易来
                        output.setChange_address(true);
                    }
                    output.setMultisig(tmp.getMultisig());  // 该值由create context时确认，此处更改值不会被启用
                    outputList.add(output);
                }
                btcTransInfo.setOutputs(outputList);

                final String unsignedData = GsonUtils.toJson(btcTransInfo, MultiSigType.class, new MyEnumTypeAdapter());
                Log.d("JuBiter", "待签名数据: " + unsignedData);

                int ret = NativeApi.nativeShowVirtualPwd(contextID);
                if (ret != 0) {
                    final int finalRet = ret;
                    mUIHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            callback.onFailed(finalRet);
                        }
                    });
                    return;
                }

                mUIHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        final VerifyPinDialog dialog = new VerifyPinDialog(mContext, new VerifyPinDialog.callback() {
                            @Override
                            public void onClickListener(String pin) {
                                int ret = 0;
                                if (TextUtils.isEmpty(pin) || pin.length() != 4) {
                                    final int finalRet = ret;
                                    mUIHandler.post(new Runnable() {
                                        @Override
                                        public void run() {
                                            callback.onFailed(finalRet);
                                        }
                                    });
                                    return;
                                }

                                ret = NativeApi.nativeVerifyPIN(contextID, pin.getBytes());

                                if (ret != 0) {
                                    final int finalRet = ret;
                                    mUIHandler.post(new Runnable() {
                                        @Override
                                        public void run() {
                                            callback.onFailed(finalRet);
                                        }
                                    });
                                    return;
                                }
                                final String rawString = NativeApi.nativeBTCTransaction(contextID, unsignedData);
                                if (TextUtils.isEmpty(rawString)) {
                                    mUIHandler.post(new Runnable() {
                                        @Override
                                        public void run() {
                                            callback.onFailed(NativeApi.nativeGetErrorCode());
                                        }
                                    });
                                    return;
                                }
                                Log.d("JuBiter", "signedData: " + rawString);
                                mUIHandler.post(new Runnable() {
                                    @Override
                                    public void run() {
                                        callback.onSuccess(rawString, null);
                                    }
                                });
                            }
                        });
                        dialog.show();
                    }
                });

            }
        }).start();
    }

    public void QRC20Trans(final JubCallback<String, String> callback) {
        new Thread(new Runnable() {
            @Override
            public void run() {

                int[] contextIDs = new int[1];
                int rv = NativeApi.nativeBTCCreateContext(contextIDs, false,
                        JSONParseUtils.getJsonStr(mContext, "contextQRC20.json"),
                        mDeviceHandle);
                if (rv != 0) {
                    final int finalRet = NativeApi.nativeGetErrorCode();
                    mUIHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            callback.onFailed(finalRet);
                        }
                    });
                    return;
                }
                final int contextID = contextIDs[0];
                Log.d("QRC20Trans contextID:", " " + contextID);
                int ret = NativeApi.nativeShowVirtualPwd(contextID);
                if (ret != 0) {
                    final int finalRet = ret;
                    mUIHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            callback.onFailed(finalRet);
                        }
                    });
                    return;
                }

                mUIHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        final VerifyPinDialog dialog = new VerifyPinDialog(mContext, new VerifyPinDialog.callback() {
                            @Override
                            public void onClickListener(String pin) {
                                int ret = 0;
                                if (TextUtils.isEmpty(pin) || pin.length() != 4) {
                                    final int finalRet = ret;
                                    mUIHandler.post(new Runnable() {
                                        @Override
                                        public void run() {
                                            callback.onFailed(finalRet);
                                        }
                                    });
                                    return;
                                }

                                ret = NativeApi.nativeVerifyPIN(contextID, pin.getBytes());

                                if (ret != 0) {
                                    final int finalRet = ret;
                                    mUIHandler.post(new Runnable() {
                                        @Override
                                        public void run() {
                                            callback.onFailed(finalRet);
                                        }
                                    });
                                    return;
                                }

                                final String rawString = NativeApi.nativeQRC20Transaction(contextID,
                                        JSONParseUtils.getJsonStr(mContext, "testQTUM_qrc20.json"));
                                if (TextUtils.isEmpty(rawString)) {
                                    mUIHandler.post(new Runnable() {
                                        @Override
                                        public void run() {
                                            callback.onFailed(NativeApi.nativeGetErrorCode());
                                        }
                                    });
                                    return;
                                }
                                mUIHandler.post(new Runnable() {
                                    @Override
                                    public void run() {
                                        callback.onSuccess(rawString, null);
                                    }
                                });
                            }
                        });
                        dialog.show();
                    }
                });

            }
        }).start();
    }

    // ***************************************** ETH **************************************************

    public void ethTrans(final JubCallback<String, Void> callback) {
        new Thread(new Runnable() {
            @Override
            public void run() {

                int[] contextIDs = new int[1];
                NativeApi.nativeETHCreateContext(contextIDs,
                        JSONParseUtils.getJsonStr(mContext, "contextETH.json"),
                        mDeviceHandle);
                final long contextID = contextIDs[0];

                Log.d("ethTrans contextID:", " " + contextID);
                int ret = NativeApi.nativeShowVirtualPwd(contextID);
                if (ret != 0) {
                    final int finalRet = ret;
                    mUIHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            callback.onFailed(finalRet);
                        }
                    });
                    return;
                }

                mUIHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        final VerifyPinDialog dialog = new VerifyPinDialog(mContext, new VerifyPinDialog.callback() {
                            @Override
                            public void onClickListener(String pin) {
                                int ret = 0;
                                if (TextUtils.isEmpty(pin) || pin.length() != 4) {
                                    final int finalRet = ret;
                                    mUIHandler.post(new Runnable() {
                                        @Override
                                        public void run() {
                                            callback.onFailed(finalRet);
                                        }
                                    });
                                    return;
                                }

                                ret = NativeApi.nativeVerifyPIN(contextID, pin.getBytes());

                                if (ret != 0) {
                                    final int finalRet = ret;
                                    mUIHandler.post(new Runnable() {
                                        @Override
                                        public void run() {
                                            callback.onFailed(finalRet);
                                        }
                                    });
                                    return;
                                }
                                final String rawString = NativeApi.nativeETHTransaction(contextID, JSONParseUtils.getJsonStr(mContext, "testETH.json"));
                                if (TextUtils.isEmpty(rawString)) {
                                    mUIHandler.post(new Runnable() {
                                        @Override
                                        public void run() {
                                            callback.onFailed(NativeApi.nativeGetErrorCode());
                                        }
                                    });
                                    return;
                                }
                                mUIHandler.post(new Runnable() {
                                    @Override
                                    public void run() {
                                        callback.onSuccess(rawString, null);
                                    }
                                });
                            }
                        });
                        dialog.show();
                    }
                });

            }
        }).start();
    }

    public void ethERC20Trans(final JubCallback<String, Void> callback) {
        new Thread(new Runnable() {
            @Override
            public void run() {

                int[] contextIDs = new int[1];
                NativeApi.nativeETHCreateContext(contextIDs,
                        JSONParseUtils.getJsonStr(mContext, "testETH_setToken.json"),
                        mDeviceHandle);
                final long contextID = contextIDs[0];

                Log.d("ethTrans contextID:", " " + contextID);

                int ret = NativeApi.nativeShowVirtualPwd(contextID);
                if (ret != 0) {
                    final int finalRet = ret;
                    mUIHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            callback.onFailed(finalRet);
                        }
                    });
                    return;
                }

                mUIHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        final VerifyPinDialog dialog = new VerifyPinDialog(mContext, new VerifyPinDialog.callback() {
                            @Override
                            public void onClickListener(String pin) {
                                int ret = 0;
                                if (TextUtils.isEmpty(pin) || pin.length() != 4) {
                                    final int finalRet = ret;
                                    mUIHandler.post(new Runnable() {
                                        @Override
                                        public void run() {
                                            callback.onFailed(finalRet);
                                        }
                                    });
                                    return;
                                }

                                ret = NativeApi.nativeVerifyPIN(contextID, pin.getBytes());

                                if (ret != 0) {
                                    final int finalRet = ret;
                                    mUIHandler.post(new Runnable() {
                                        @Override
                                        public void run() {
                                            callback.onFailed(finalRet);
                                        }
                                    });
                                    return;
                                }
                                final String rawString = NativeApi.nativeETHERC20Transaction(contextID, JSONParseUtils.getJsonStr(mContext, "testETH_setToken.json"));
                                if (TextUtils.isEmpty(rawString)) {
                                    mUIHandler.post(new Runnable() {
                                        @Override
                                        public void run() {
                                            callback.onFailed(NativeApi.nativeGetErrorCode());
                                        }
                                    });
                                    return;
                                }
                                mUIHandler.post(new Runnable() {
                                    @Override
                                    public void run() {
                                        callback.onSuccess(rawString, null);
                                    }
                                });
                            }
                        });
                        dialog.show();
                    }
                });

            }
        }).start();
    }


    public void ethGetAddress(final JubCallback<String, String> callback) {
        new Thread(new Runnable() {
            @Override
            public void run() {

                int[] contextIDs = new int[1];
                int rv = NativeApi.nativeETHCreateContext(contextIDs,
                        JSONParseUtils.getJsonStr(mContext, "contextETH.json"),
                        mDeviceHandle);
                if (rv != 0) {
                    final int finalRet = NativeApi.nativeGetErrorCode();
                    mUIHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            callback.onFailed(finalRet);
                        }
                    });
                    return;
                }

                long contextID = contextIDs[0];
                String[] array = NativeApi.nativeETHGetAddress(contextID, JSONParseUtils.getJsonStr(mContext, "path.json"));
                if (array == null || array.length == 0) {
                    final int finalRet = NativeApi.nativeGetErrorCode();
                    mUIHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            callback.onFailed(finalRet);
                        }
                    });
                    return;
                }

                final StringBuilder builder = new StringBuilder();
                for (int i = 0; i < array.length; i += 2) {
                    builder.append("\n\n xpub: \n").append(array[i])
                            .append("\n\n address: \n").append(array[i + 1]);
                }

                mUIHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        callback.onSuccess(builder.toString(), null);
                    }
                });

            }
        }).start();
    }

    public void ethShowAddress(final int index, final JubCallback<String, Void> callback) {
        new Thread(new Runnable() {
            @Override
            public void run() {
                int[] contextIDs = new int[1];
                int rv = NativeApi.nativeETHCreateContext(contextIDs,
                        JSONParseUtils.getJsonStr(mContext, "contextETH.json"),
                        mDeviceHandle);

                if (rv != 0) {
                    final int finalRet = NativeApi.nativeGetErrorCode();
                    mUIHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            callback.onFailed(finalRet);
                        }
                    });
                    return;
                }

                final long contextID = contextIDs[0];
                Log.d("btcTrans contextID:", " " + contextID);
                final String ret = NativeApi.nativeETHShowAddress(contextID, index);
                if (TextUtils.isEmpty(ret)) {
                    mUIHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            callback.onFailed(NativeApi.nativeGetErrorCode());
                        }
                    });
                    return;
                }
                mUIHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        callback.onSuccess(ret, null);
                    }
                });

            }
        }).start();
    }

    public void ethGetMainHDNode(final JubCallback<String, String> callback) {
        new Thread(new Runnable() {
            @Override
            public void run() {

//                int ret = NativeApi.nativeClearContext(1);
//                Log.d("fs", ">>> nativeClearContext ret: " + ret);

                int[] contextIDs = new int[1];
                int rv = NativeApi.nativeETHCreateContext(contextIDs,
                        JSONParseUtils.getJsonStr(mContext, "contextETH.json"),
                        mDeviceHandle);
                if (rv != 0) {
                    final int finalRet = NativeApi.nativeGetErrorCode();
                    mUIHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            callback.onFailed(finalRet);
                        }
                    });
                    return;
                }

                long contextID = contextIDs[0];
                final String mainXpub = NativeApi.nativeETHGetMainHDNode(contextID);
                if (TextUtils.isEmpty(mainXpub)) {
                    final int finalRet = NativeApi.nativeGetErrorCode();
                    mUIHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            callback.onFailed(finalRet);
                        }
                    });
                    return;
                }
                mUIHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        callback.onSuccess(mainXpub, null);
                    }
                });

            }
        }).start();
    }

    public void enumSupportCoins(final JubCallback<String[], String> callback) {
        new Thread(new Runnable() {
            @Override
            public void run() {
                final String coins = NativeApi.nativeEnumSupportCoins(mDeviceHandle);
                if (TextUtils.isEmpty(coins)) {
                    final int finalRet = NativeApi.nativeGetErrorCode();
                    mUIHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            callback.onFailed(finalRet);
                        }
                    });
                    return;
                }
                final String[] list = coins.split(" ");
                mUIHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        callback.onSuccess(list, null);
                    }
                });


            }
        }).start();
    }

    public void hcGetAddress(final JubCallback<String, String> callback) {
        new Thread(new Runnable() {
            @Override
            public void run() {

                int[] contextIDs = new int[1];
                int rv = NativeApi.nativeHCCreateContext(contextIDs, true,
                        JSONParseUtils.getJsonStr(mContext, "contextHC_1.json"),
                        mDeviceHandle);
                if (rv != 0) {
                    final int finalRet = NativeApi.nativeGetErrorCode();
                    mUIHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            callback.onFailed(finalRet);
                        }
                    });
                    return;
                }

                long contextID = contextIDs[0];
                String[] array = NativeApi.nativeHCGetAddress(contextID, JSONParseUtils.getJsonStr(mContext, "path.json"));
                if (array == null || array.length == 0) {
                    final int finalRet = NativeApi.nativeGetErrorCode();
                    mUIHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            callback.onFailed(finalRet);
                        }
                    });
                    return;
                }

                final StringBuilder builder = new StringBuilder();

                for (int i = 0; i < array.length; i += 2) {
                    builder.append(array[i])
                            .append("\n\n address: \n").append(array[i + 1]);
                    Log.d("test", builder.toString());
                }

                mUIHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        callback.onSuccess(builder.toString(), null);
                    }
                });

            }
        }).start();
    }

    public void hcGetMainHDNode(final JubCallback<String, String> callback) {
        new Thread(new Runnable() {
            @Override
            public void run() {

                int[] contextIDs = new int[1];
                int rv = NativeApi.nativeHCCreateContext(contextIDs, false,
                        JSONParseUtils.getJsonStr(mContext, "contextHC.json"),
                        mDeviceHandle);
                if (rv != 0) {
                    final int finalRet = NativeApi.nativeGetErrorCode();
                    mUIHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            callback.onFailed(finalRet);
                        }
                    });
                    return;
                }

                long contextID = contextIDs[0];
                final String mainXpub = NativeApi.nativeHCGetMainHDNode(contextID);
                if (TextUtils.isEmpty(mainXpub)) {
                    final int finalRet = NativeApi.nativeGetErrorCode();
                    mUIHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            callback.onFailed(finalRet);
                        }
                    });
                    return;
                }
                mUIHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        callback.onSuccess(mainXpub, null);
                    }
                });

            }
        }).start();
    }

    public void hcTransaction(final JubCallback<String, String> callback) {
        new Thread(new Runnable() {
            @Override
            public void run() {

                int[] contextIDs = new int[1];
                NativeApi.nativeHCCreateContext(contextIDs, false,
                        JSONParseUtils.getJsonStr(mContext, "contextHC.json"),
                        mDeviceHandle);
                final long contextID = contextIDs[0];

                Log.d("btcTrans contextID:", " " + contextID);
                int ret = NativeApi.nativeShowVirtualPwd(contextID);
                if (ret != 0) {
                    final int finalRet = ret;
                    mUIHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            callback.onFailed(finalRet);
                        }
                    });
                    return;
                }

                mUIHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        final VerifyPinDialog dialog = new VerifyPinDialog(mContext, new VerifyPinDialog.callback() {
                            @Override
                            public void onClickListener(String pin) {
                                int ret = 0;
                                if (TextUtils.isEmpty(pin) || pin.length() != 4) {
                                    final int finalRet = ret;
                                    mUIHandler.post(new Runnable() {
                                        @Override
                                        public void run() {
                                            callback.onFailed(finalRet);
                                        }
                                    });
                                    return;
                                }

                                ret = NativeApi.nativeVerifyPIN(contextID, pin.getBytes());

                                if (ret != 0) {
                                    final int finalRet = ret;
                                    mUIHandler.post(new Runnable() {
                                        @Override
                                        public void run() {
                                            callback.onFailed(finalRet);
                                        }
                                    });
                                    return;
                                }
                                final String rawString = NativeApi.nativeHCTransaction(contextID, JSONParseUtils.getJsonStr(mContext, "testHC.json"));
                                if (TextUtils.isEmpty(rawString)) {
                                    mUIHandler.post(new Runnable() {
                                        @Override
                                        public void run() {
                                            callback.onFailed(NativeApi.nativeGetErrorCode());
                                        }
                                    });
                                    return;
                                }
                                mUIHandler.post(new Runnable() {
                                    @Override
                                    public void run() {
                                        callback.onSuccess(rawString, null);
                                    }
                                });
                            }
                        });
                        dialog.show();
                    }
                });

            }
        }).start();
    }

    public void eosTransaction(final JubCallback<String, String> callback) {
        new Thread(new Runnable() {
            @Override
            public void run() {

                int[] contextIDs = new int[1];
                NativeApi.nativeEOSCreateContext(contextIDs,
                        JSONParseUtils.getJsonStr(mContext, "testEOS.json"),
                        mDeviceHandle);
                final long contextID = contextIDs[0];

                Log.d("eosTrans contextID:", " " + contextID);
                int ret = NativeApi.nativeShowVirtualPwd(contextID);
                if (ret != 0) {
                    final int finalRet = ret;
                    mUIHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            callback.onFailed(finalRet);
                        }
                    });
                    return;
                }

                mUIHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        final VerifyPinDialog dialog = new VerifyPinDialog(mContext, new VerifyPinDialog.callback() {
                            @Override
                            public void onClickListener(String pin) {
                                int ret = 0;
                                if (TextUtils.isEmpty(pin) || pin.length() != 4) {
                                    final int finalRet = ret;
                                    mUIHandler.post(new Runnable() {
                                        @Override
                                        public void run() {
                                            callback.onFailed(finalRet);
                                        }
                                    });
                                    return;
                                }

                                ret = NativeApi.nativeVerifyPIN(contextID, pin.getBytes());

                                if (ret != 0) {
                                    final int finalRet = ret;
                                    mUIHandler.post(new Runnable() {
                                        @Override
                                        public void run() {
                                            callback.onFailed(finalRet);
                                        }
                                    });
                                    return;
                                }
                                final String rawString = NativeApi.nativeEOSTransaction(contextID, JSONParseUtils.getJsonStr(mContext, "testEOS.json"));
                                if (TextUtils.isEmpty(rawString)) {
                                    mUIHandler.post(new Runnable() {
                                        @Override
                                        public void run() {
                                            callback.onFailed(NativeApi.nativeGetErrorCode());
                                        }
                                    });
                                    return;
                                }
                                mUIHandler.post(new Runnable() {
                                    @Override
                                    public void run() {
                                        callback.onSuccess(rawString, null);
                                    }
                                });
                            }
                        });
                        dialog.show();
                    }
                });

            }
        }).start();
    }

    public void hcShowAddress(final int change, final int index, final JubCallback<String, Void> callback) {
        new Thread(new Runnable() {
            @Override
            public void run() {
                int[] contextIDs = new int[1];
                NativeApi.nativeHCCreateContext(contextIDs, false,
                        JSONParseUtils.getJsonStr(mContext, "contextHC.json"),
                        mDeviceHandle);
                final long contextID = contextIDs[0];

                Log.d("btcTrans contextID:", " " + contextID);
                final String ret = NativeApi.nativeHCShowAddress(contextID, change, index);
                if (TextUtils.isEmpty(ret)) {
                    mUIHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            callback.onFailed(NativeApi.nativeGetErrorCode());
                        }
                    });
                    return;
                }
                mUIHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        callback.onSuccess(ret, null);
                    }
                });

            }
        }).start();
    }

    public void xrpGetAddress(final JubCallback<String, String> callback) {
        new Thread(new Runnable() {
            @Override
            public void run() {

                int[] contextIDs = new int[1];
                int rv = NativeApi.nativeXRPCreateContext(contextIDs,
                        JSONParseUtils.getJsonStr(mContext, "testXRP.json"),
                        mDeviceHandle);
                if (rv != 0) {
                    final int finalRet = NativeApi.nativeGetErrorCode();
                    mUIHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            callback.onFailed(finalRet);
                        }
                    });
                    return;
                }

                long contextID = contextIDs[0];
                final String array = NativeApi.nativeXRPGetAddress(contextID, 0, 0);
                final String pubKey = NativeApi.nativeXRPGetHDNode(contextID, 0, 0, true);
                if (array == null && pubKey == null) {
                    final int finalRet = NativeApi.nativeGetErrorCode();
                    mUIHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            callback.onFailed(finalRet);
                        }
                    });
                    return;
                }


                mUIHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        callback.onSuccess(array + "\n" + pubKey, null);
                    }
                });

            }
        }).start();
    }

    public void xrpTransaction(final JubCallback<String, String> callback) {
        new Thread(new Runnable() {
            @Override
            public void run() {

                int[] contextIDs = new int[1];
                NativeApi.nativeXRPCreateContext(contextIDs,
                        JSONParseUtils.getJsonStr(mContext, "testXRP_1.json"),
                        mDeviceHandle);
                final long contextID = contextIDs[0];

                Log.d("xrpTrans contextID:", " " + contextID);
                int ret = NativeApi.nativeShowVirtualPwd(contextID);
                if (ret != 0) {
                    final int finalRet = ret;
                    mUIHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            callback.onFailed(finalRet);
                        }
                    });
                    return;
                }

                mUIHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        final VerifyPinDialog dialog = new VerifyPinDialog(mContext, new VerifyPinDialog.callback() {
                            @Override
                            public void onClickListener(String pin) {
                                int ret = 0;
                                if (TextUtils.isEmpty(pin) || pin.length() != 4) {
                                    final int finalRet = ret;
                                    mUIHandler.post(new Runnable() {
                                        @Override
                                        public void run() {
                                            callback.onFailed(finalRet);
                                        }
                                    });
                                    return;
                                }

                                ret = NativeApi.nativeVerifyPIN(contextID, pin.getBytes());

                                if (ret != 0) {
                                    final int finalRet = ret;
                                    mUIHandler.post(new Runnable() {
                                        @Override
                                        public void run() {
                                            callback.onFailed(finalRet);
                                        }
                                    });
                                    return;
                                }
                                final String rawString = NativeApi.nativeXRPTransaction(contextID, JSONParseUtils.getJsonStr(mContext, "testXRP_1.json"));
                                if (TextUtils.isEmpty(rawString)) {
                                    mUIHandler.post(new Runnable() {
                                        @Override
                                        public void run() {
                                            callback.onFailed(NativeApi.nativeGetErrorCode());
                                        }
                                    });
                                    return;
                                }
                                mUIHandler.post(new Runnable() {
                                    @Override
                                    public void run() {
                                        callback.onSuccess(rawString, null);
                                    }
                                });
                            }
                        });
                        dialog.show();
                    }
                });

            }
        }).start();
    }


    public void ethBuild(final JubCallback<String, String> callback) {
        new Thread(new Runnable() {
            @Override
            public void run() {

                int[] contextIDs = new int[1];
                NativeApi.nativeETHCreateContext(contextIDs,
                        JSONParseUtils.getJsonStr(mContext, "testETH_multi_contract.json"),
                        mDeviceHandle);
                final long contextID = contextIDs[0];

                Log.d("ethBuild contextID:", " " + contextID);

                final String rawString = NativeApi.nativeETHBuildContractWithAddrAmtDataAbi(contextID, JSONParseUtils.getJsonStr(mContext, "testETH_multi_build_with_addr_amt.json"));
                if (TextUtils.isEmpty(rawString)) {
                    mUIHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            callback.onFailed(NativeApi.nativeGetErrorCode());
                        }
                    });
                    return;
                }
                mUIHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        callback.onSuccess(rawString, null);
                    }
                });

            }
        }).start();
    }


    public void ethContract(final JubCallback<String, String> callback) {
        new Thread(new Runnable() {
            @Override
            public void run() {

                int[] contextIDs = new int[1];
                NativeApi.nativeETHCreateContext(contextIDs,
                        JSONParseUtils.getJsonStr(mContext, "testETH_multi_contract_test.json"),
                        mDeviceHandle);
                final long contextID = contextIDs[0];
                String data = NativeApi.nativeETHBuildERC20Abi(contextID, JSONParseUtils.getJsonStr(mContext, "testETH_setToken.json"));
                final String rawString = NativeApi.nativeETHBuildContractWithAddrAmtDataAbi(contextID, JSONParseUtils.getJsonStr(mContext, "testETH_multi_build_with_addr_amt.json"));
                Log.d("ethContract contextID:", " " + contextID);
                int ret = NativeApi.nativeShowVirtualPwd(contextID);
                if (ret != 0) {
                    final int finalRet = ret;
                    mUIHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            callback.onFailed(finalRet);
                        }
                    });
                    return;
                }

                mUIHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        final VerifyPinDialog dialog = new VerifyPinDialog(mContext, new VerifyPinDialog.callback() {
                            @Override
                            public void onClickListener(String pin) {
                                int ret = 0;
                                if (TextUtils.isEmpty(pin) || pin.length() != 4) {
                                    final int finalRet = ret;
                                    mUIHandler.post(new Runnable() {
                                        @Override
                                        public void run() {
                                            callback.onFailed(finalRet);
                                        }
                                    });
                                    return;
                                }

                                ret = NativeApi.nativeVerifyPIN(contextID, pin.getBytes());

                                if (ret != 0) {
                                    final int finalRet = ret;
                                    mUIHandler.post(new Runnable() {
                                        @Override
                                        public void run() {
                                            callback.onFailed(finalRet);
                                        }
                                    });
                                    return;
                                }
                                final String rawString = NativeApi.nativeETHSignContract(contextID, JSONParseUtils.getJsonStr(mContext, "testETH_multi_contract_test.json"));
                                if (TextUtils.isEmpty(rawString)) {
                                    mUIHandler.post(new Runnable() {
                                        @Override
                                        public void run() {
                                            callback.onFailed(NativeApi.nativeGetErrorCode());
                                        }
                                    });
                                    return;
                                }
                                mUIHandler.post(new Runnable() {
                                    @Override
                                    public void run() {
                                        callback.onSuccess(rawString, null);
                                    }
                                });
                            }
                        });
                        dialog.show();
                    }
                });

            }
        }).start();
    }


    public void ethTransactionUniswapV2Router02(final JubCallback<String, String> callback) {
        new Thread(new Runnable() {
            @Override
            public void run() {

                int[] contextIDs = new int[1];
                NativeApi.nativeETHCreateContext(contextIDs,
                        JSONParseUtils.getJsonStr(mContext, "testETH_UniswapV2Router02.json"),
                        mDeviceHandle);
                final long contextID = contextIDs[0];
                int ret = NativeApi.nativeShowVirtualPwd(contextID);
                if (ret != 0) {
                    final int finalRet = ret;
                    mUIHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            callback.onFailed(finalRet);
                        }
                    });
                    return;
                }

                mUIHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        final VerifyPinDialog dialog = new VerifyPinDialog(mContext, new VerifyPinDialog.callback() {
                            @Override
                            public void onClickListener(String pin) {
                                int ret = 0;
                                if (TextUtils.isEmpty(pin) || pin.length() != 4) {
                                    final int finalRet = ret;
                                    mUIHandler.post(new Runnable() {
                                        @Override
                                        public void run() {
                                            callback.onFailed(finalRet);
                                        }
                                    });
                                    return;
                                }

                                ret = NativeApi.nativeVerifyPIN(contextID, pin.getBytes());

                                if (ret != 0) {
                                    final int finalRet = ret;
                                    mUIHandler.post(new Runnable() {
                                        @Override
                                        public void run() {
                                            callback.onFailed(finalRet);
                                        }
                                    });
                                    return;
                                }
                                // contractAddress is from testETH_UniswapV2Router02.json
                                ret = NativeApi.nativeETHSetContrAddr(contextID, "0x7a250d5630b4cf539739df2c5dacb4c659f2488d");
                                if (ret != 0) {
                                    final int finalRet = ret;
                                    mUIHandler.post(new Runnable() {
                                        @Override
                                        public void run() {
                                            callback.onFailed(finalRet);
                                        }
                                    });
                                    return;
                                }
                                final String rawString = NativeApi.nativeETHSignContract(contextID, JSONParseUtils.getJsonStr(mContext, "testETH_UniswapV2Router02.json"));
                                if (TextUtils.isEmpty(rawString)) {
                                    mUIHandler.post(new Runnable() {
                                        @Override
                                        public void run() {
                                            callback.onFailed(NativeApi.nativeGetErrorCode());
                                        }
                                    });
                                    return;
                                }
                                mUIHandler.post(new Runnable() {
                                    @Override
                                    public void run() {
                                        callback.onSuccess(rawString, null);
                                    }
                                });
                            }
                        });
                        dialog.show();
                    }
                });

            }
        }).start();
    }

    public void trxContract(final JubCallback<String, String> callback) {
        new Thread(new Runnable() {
            @Override
            public void run() {

                int[] contextIDs = new int[1];
                NativeApi.nativeTRXCreateContext(contextIDs,
                        JSONParseUtils.getJsonStr(mContext, "testTRX.json"),
                        mDeviceHandle);
                final long contextID = contextIDs[0];
                final byte[] data = NativeApi.nativeTRXPackContract(contextID,
                        JSONParseUtils.getJsonStr(mContext, "testTRX.json"));
                if(data == null){
                    int code = NativeApi.nativeGetErrorCode();
                    Log.e("ERROR",code+"");
                    return;
                }
                mUIHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        callback.onSuccess(StrUtil.byteArr2HexStr(data), null);
                    }
                });
            }
        }).start();
    }
}
