package com.jubiter.sdk.demo;

import android.os.Bundle;
import android.os.CountDownTimer;
import android.os.Handler;
import android.support.v7.app.AppCompatActivity;
import android.text.TextUtils;
import android.view.View;
import android.widget.Button;
import android.widget.ScrollView;
import android.widget.TextView;

import com.jubiter.sdk.demo.ble.FTBTDevice;
import com.jubiter.sdk.demo.ble.FtBTConnectCallback;
import com.jubiter.sdk.demo.impl.JubCallback;
import com.jubiter.sdk.demo.impl.JubiterImpl;
import com.jubiter.sdk.demo.utils.SaveLogUtil;
import com.jubiter.sdk.demo.utils.StrUtil;
import com.jubiter.sdk.demo.utils.TimeUtil;
import com.legendwd.hyperpay.main.hardwarewallet.jubnative.InnerDiscCallback;
import com.legendwd.hyperpay.main.hardwarewallet.jubnative.InnerScanCallback;
import com.legendwd.hyperpay.main.hardwarewallet.jubnative.NativeApi;

import java.util.Timer;

public class TestActivity extends AppCompatActivity {

    private Button mBtnScan;
    private TextView mTxtState, mTxtInfo, mTxtTime, mTxtBv;
    private ScrollView mScrollView;
    private boolean isConnect = false;
    private long mBtDeviceHandler;
    private JubiterImpl mJubiter;
    private int testTime = 0;
    private long mStartTime = 0;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_test);

        mJubiter = JubiterImpl.getInstance(this);

        mBtnScan = findViewById(R.id.btn_scan);
        mTxtState = findViewById(R.id.text_state);
        mTxtInfo = findViewById(R.id.info_tv);
        mTxtTime = findViewById(R.id.time_tv);
        mTxtBv = findViewById(R.id.bv_tv);
        mScrollView = findViewById(R.id.scrollView);
    }

    public void onClick(View view) {
        mTxtInfo.setText(null);
        switch (view.getId()) {
            case R.id.btn_scan:
                switchBt();
                break;
            case R.id.btn_test:
                //selectApplet();
                connectTest();
                break;
            default:
                break;
        }
    }
    private void connectTest(){
        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    Thread.sleep(7000);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                NativeApi.nativeStartScan(new InnerScanCallback() {
                    @Override
                    public void onScanResult(final String name, final String uuid, int devType) {
                        new Thread(new Runnable() {
                            @Override
                            public void run() {
                                if(!TextUtils.isEmpty(name)){
                                    long[] hld = new long[1];
                                    NativeApi.nativeStopScan();
                                    int i = NativeApi.nativeConnectDevice(name, uuid, 1, hld, 30 * 1000 , new InnerDiscCallback() {
                                        @Override
                                        public void onDisconnect(String name) {

                                        }
                                    });
                                    if(i == 0){
                                        NativeApi.nativeDisconnect(hld[0]);
                                    }
                                }
                            }
                        }).start();
                    }

                    @Override
                    public void onStop() {

                    }

                    @Override
                    public void onError(int errorCode) {

                    }
                });
            }
        }).start();
    }

    private void selectApplet() {
        mStartTime = System.currentTimeMillis();
        mJubiter.sendApdu("00a4040006111111111122", new JubCallback<String, Void>() {
            @Override
            public void onSuccess(String s, Void aVoid) {
                if (s.endsWith("9000")) {
                    mTxtInfo.setText("选择应用成功");
                    genPairKey();
                } else {
                    mTxtInfo.setText("选择应用失败：" + s);
                }
            }

            @Override
            public void onFailed(int errorCode) {
                mTxtInfo.setText("选择应用失败：" + errorCode);
            }
        });
    }

    private void genPairKey() {
        mJubiter.sendApdu("8000000000", new JubCallback<String, Void>() {
            @Override
            public void onSuccess(String s, Void aVoid) {
                if (s.endsWith("9000")) {
                    mTxtInfo.setText("生成密钥对成功");
                    getBatteryV();
                } else {
                    mTxtInfo.setText("生成密钥对失败：" + s);
                }
            }

            @Override
            public void onFailed(int errorCode) {
                mTxtInfo.setText("生成密钥对失败：" + errorCode);
            }
        });
    }

    public static final int SEND_D_TIME = 150;

    private void genSign() {
        mJubiter.sendApdu("8001000000", new JubCallback<String, Void>() {
            @Override
            public void onSuccess(String s, Void aVoid) {
                if (s.endsWith("9000")) {
                    new Handler().postDelayed(new Runnable() {
                        @Override
                        public void run() {
                            getBatteryV();
                        }
                    }, SEND_D_TIME);

                    mTxtInfo.setText("签名测试中....");
                } else {
                    mTxtInfo.setText("生成签名失败：" + s);
                }
            }

            @Override
            public void onFailed(int errorCode) {
            }
        });
    }

    private void getBatteryV() {
        mJubiter.sendApdu("00D6FEED01", new JubCallback<String, Void>() {
            @Override
            public void onSuccess(String s, Void aVoid) {
                if (s.endsWith("9000")) {
                    if(s.length() < 10){
                        flushTime();
                        SaveLogUtil.saveLog((testTime) + " : " + s);
                        mTxtBv.append((testTime) + " : " + s + "\n");
                        StrUtil.scrollBottom(mScrollView, mTxtBv);
                    }
                    new Handler().postDelayed(new Runnable() {
                        @Override
                        public void run() {
                            genSign();
                        }
                    }, SEND_D_TIME);
                } else {
                    mTxtInfo.setText("获取电压失败：" + s);
                }
            }

            @Override
            public void onFailed(int errorCode) {
            }
        });
    }

    private void flushTime() {
        long currentTime = System.currentTimeMillis();
        long dur = currentTime - mStartTime;
        String time = TimeUtil.formatTime(dur);
        testTime++;
        mTxtTime.setText("已测试时间：" + time + "\n次数：" + testTime + "次");
    }


    private void switchBt() {
        if (isConnect) {
            mJubiter.disConnectDevice();
            mBtnScan.setText("连接");
            mTxtInfo.setText(null);
            isConnect = false;
        } else {
            mJubiter.connect(new FtBTConnectCallback() {
                @Override
                public void connected(FTBTDevice device, int code) {
                    if (code == 0) {
                        mTxtState.setText(device.getName() + "\n" + device.getMac());
                        mBtnScan.setText("断开");
                        mBtDeviceHandler = device.getHandler();
                        isConnect = true;
                        testTime = 0;
                    } else {
                        mTxtState.setText(String.format("连接失败：0x%x", code) + "\n");
                    }
                }

                @Override
                public void disConnected() {
                    mTxtState.setText("");
                    mBtnScan.setText("连接");
                    isConnect = false;
                }
            });
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mJubiter.onDestroy();
    }
}
