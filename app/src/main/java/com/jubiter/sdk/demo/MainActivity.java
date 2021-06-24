package com.jubiter.sdk.demo;

import android.Manifest;
import android.app.ProgressDialog;
import android.content.Intent;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Size;
import android.support.v7.app.AppCompatActivity;
import android.text.TextUtils;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import com.google.gson.Gson;
import com.jubiter.sdk.demo.ble.FTBTDevice;
import com.jubiter.sdk.demo.ble.FtBTConnectCallback;
import com.jubiter.sdk.demo.impl.DeviceType;
import com.jubiter.sdk.demo.impl.JubCallback;
import com.jubiter.sdk.demo.impl.JubiterImpl;
import com.jubiter.sdk.demo.utils.TipDialog;
import com.legendwd.hyperpay.main.hardwarewallet.jubnative.InnerDiscCallback;
import com.legendwd.hyperpay.main.hardwarewallet.jubnative.utils.JUB_DEVICE_INFO;

import java.util.Arrays;
import java.util.List;

import pub.devrel.easypermissions.AppSettingsDialog;
import pub.devrel.easypermissions.EasyPermissions;

public class MainActivity extends AppCompatActivity implements EasyPermissions.PermissionCallbacks {

    private Button mBtnScan;
    private TextView mTxtState;
    private boolean isConnect = false;
    private JubiterImpl mJubiter;
    private ProgressDialog mDialog;
    private EditText mEditApdu;
    private final static int REQUEST_PERMISSION = 0x1001;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mJubiter = JubiterImpl.getInstance(this);

        mBtnScan = findViewById(R.id.btn_scan);
        mTxtState = findViewById(R.id.text_state);
        mDialog = new ProgressDialog(this);
        mEditApdu = findViewById(R.id.edit_apdu);

        if (!hasPermissions()) {
            requestPermissions("Permission request", REQUEST_PERMISSION, Manifest.permission.ACCESS_COARSE_LOCATION, Manifest.permission.ACCESS_FINE_LOCATION);
        }
    }

    public boolean hasPermissions() {
        return EasyPermissions.hasPermissions(this, Manifest.permission.ACCESS_COARSE_LOCATION, Manifest.permission.ACCESS_FINE_LOCATION);
    }

    public void requestPermissions(@NonNull String rationale,
                                   int requestCode, @Size(min = 1) @NonNull String... perms) {
        EasyPermissions.requestPermissions(this, rationale, requestCode, perms);
    }

    private InnerDiscCallback callback = new InnerDiscCallback() {
        @Override
        public void onDisconnect(String name) {

        }
    };

    public void onClick(View view) {
        if (view.getId() == R.id.btn_scan) {
            switchBt();
            return;
        }
        if (!isConnect) {
            return;
        }
        switch (view.getId()) {
            case R.id.btn_fingerprint_manager:
                showProgress();
                mJubiter.getDeviceType(new JubCallback<String, Void>() {
                    @Override
                    public void onSuccess(String s, Void aVoid) {
                        dismissProgress();
                        DeviceType deviceType = new Gson().fromJson(s, DeviceType.class);
                        if (deviceType.getDEVICE() == 2) {
                            startActivity(new Intent(MainActivity.this, FingerPrintManagerActivity.class));
                            return;
                        }
                        showMsg("Device not support");
                    }

                    @Override
                    public void onFailed(int errorCode) {

                    }
                });
                break;
            case R.id.btn_get_device_info:
                showProgress();
                mJubiter.getDeviceInfo(new JubCallback<JUB_DEVICE_INFO, Void>() {
                    @Override
                    public void onSuccess(JUB_DEVICE_INFO info, Void aVoid) {
                        dismissProgress();
                        Log.d("info", info.toString());
                        showMsg(info.toString());
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;
            case R.id.btn_btc_getAddress:
                showProgress();
                mJubiter.btcGetAddress(new JubCallback<String, String>() {
                    @Override
                    public void onSuccess(String s, String s2) {
                        showMsg(s + "\t");
                        dismissProgress();
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;
            case R.id.btn_qrc20_trans:
                showProgress();
                mJubiter.QRC20Trans(new JubCallback<String, String>() {
                    @Override
                    public void onSuccess(String s, String s2) {
                        showMsg(s + "\t");
                        dismissProgress();
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;
            case R.id.btn_btc_trans:
                showProgress();
                mJubiter.btcTrans(new JubCallback<String, Void>() {
                    @Override
                    public void onSuccess(String s, Void aVoid) {
                        dismissProgress();
                        showMsg(s);
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;
            case R.id.btn_btc_multisig_trans:
                mJubiter.btcMultiSigTrans(new JubCallback<String, Void>() {
                    @Override
                    public void onSuccess(String s, Void aVoid) {
                        dismissProgress();
                        showMsg(s);
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;

            case R.id.btn_send_apdu:
                String apduStr = mEditApdu.getText().toString().trim().replace(" ", "");
                if (TextUtils.isEmpty(apduStr)) {
                    return;
                }
                showProgress();
                mJubiter.sendApdu(apduStr, new JubCallback<String, Void>() {
                    @Override
                    public void onSuccess(String s, Void aVoid) {
                        dismissProgress();
                        showMsg(s);
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        showMsg("sendAPDU failed");
                    }
                });
                break;
            case R.id.btn_applet:
                showProgress();
                mJubiter.enumApplets(new JubCallback<List<String>, Void>() {
                    @Override
                    public void onSuccess(List<String> strings, Void aVoid) {
                        dismissProgress();
                        getAppletVersion(strings);
                    }

                    @Override
                    public void onFailed(int errorCode) {

                    }
                });
                break;
            case R.id.btn_eth_trans:
                showProgress();
                mJubiter.ethTrans(new JubCallback<String, Void>() {
                    @Override
                    public void onSuccess(String s, Void aVoid) {
                        dismissProgress();
                        showMsg(s + "\n");
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;
            case R.id.btn_btc_show_address:
                showProgress();
                mJubiter.btcShowAddress(0, 1, new JubCallback<String, Void>() {
                    @Override
                    public void onSuccess(String s, Void aVoid) {
                        dismissProgress();
                        showMsg(s);
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;
            case R.id.btn_btc_set_my_address:
                showProgress();
                mJubiter.btcSetMyAddress(0, 1, new JubCallback<String, Void>() {
                    @Override
                    public void onSuccess(String s, Void aVoid) {
                        dismissProgress();
                        showMsg(s);
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;
            case R.id.btn_btc_usdt_trans:
                showProgress();
                mJubiter.usdtTrans(new JubCallback<String, Void>() {
                    @Override
                    public void onSuccess(String s, Void aVoid) {
                        dismissProgress();
                        showMsg(s);
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;
            case R.id.btn_set_time_out:
                showProgress();
                mJubiter.setTimeOut(500, new JubCallback<String, Void>() {
                    @Override
                    public void onSuccess(String s, Void aVoid) {
                        dismissProgress();
                        showMsg(s);
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;


            case R.id.btn_eth_showAddress:
                showProgress();
                mJubiter.ethShowAddress(1, new JubCallback<String, Void>() {
                    @Override
                    public void onSuccess(String s, Void aVoid) {
                        dismissProgress();
                        showMsg(s);
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;

            case R.id.btn_eth_getAddress:
                showProgress();
                mJubiter.ethGetAddress(new JubCallback<String, String>() {
                    @Override
                    public void onSuccess(String s, String s2) {
                        showMsg(s);
                        dismissProgress();
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;

            case R.id.btn_btc_getMainHDNode:
                showProgress();
                mJubiter.btcGetMainHDNode(new JubCallback<String, String>() {
                    @Override
                    public void onSuccess(String s, String s2) {
                        showMsg(s);
                        dismissProgress();
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;

            case R.id.btn_eth_getMainHDNode:
                showProgress();
                mJubiter.ethGetMainHDNode(new JubCallback<String, String>() {
                    @Override
                    public void onSuccess(String s, String s2) {
                        showMsg(s + "\t");
                        dismissProgress();
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;
            case R.id.btn_eth_transaction_UniswapV2Router02:
                showProgress();
                mJubiter.ethTransactionUniswapV2Router02(new JubCallback<String, String>() {
                    @Override
                    public void onSuccess(String s, String s2) {
                        showMsg(s + "\t");
                        dismissProgress();
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;
            case R.id.btn_trx:
                showProgress();
                mJubiter.trxContract(new JubCallback<String, String>() {
                    @Override
                    public void onSuccess(String s, String s2) {
                        showMsg(s + "\t");
                        dismissProgress();
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;
            case R.id.btn_enum_support_coins:
                showProgress();
                mJubiter.enumSupportCoins(new JubCallback<String[], String>() {
                    @Override
                    public void onSuccess(String[] s, String s2) {
                        showMsg(Arrays.toString(s));
                        dismissProgress();
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;
            case R.id.btn_hc_getAddress:
                showProgress();
                mJubiter.hcGetAddress(new JubCallback<String, String>() {
                    @Override
                    public void onSuccess(String s, String s2) {
                        dismissProgress();
                        showMsg(s);
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;
            case R.id.btn_hc_getMainHDNode:
                showProgress();
                mJubiter.hcGetMainHDNode(new JubCallback<String, String>() {
                    @Override
                    public void onSuccess(String s, String s2) {
                        dismissProgress();
                        showMsg(s);
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;
            case R.id.btn_hc_showAddress:
                showProgress();
                mJubiter.hcShowAddress(0, 1, new JubCallback<String, Void>() {
                    @Override
                    public void onSuccess(String s, Void aVoid) {
                        dismissProgress();
                        showMsg(s);
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;
            case R.id.btn_hc_transaction:
                showProgress();
                mJubiter.hcTransaction(new JubCallback<String, String>() {
                    @Override
                    public void onSuccess(String s, String s2) {
                        dismissProgress();
                        showMsg(s);
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;
            case R.id.btn_eos_transaction:
                showProgress();
                mJubiter.eosTransaction(new JubCallback<String, String>() {
                    @Override
                    public void onSuccess(String s, String s2) {
                        dismissProgress();
                        showMsg(s);
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;
            case R.id.btn_xrp_get_address:
                showProgress();
                mJubiter.xrpGetAddress(new JubCallback<String, String>() {
                    @Override
                    public void onSuccess(String s, String s2) {
                        dismissProgress();
                        showMsg(s);
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;
            case R.id.btn_xrp_transaction:
                showProgress();
                mJubiter.xrpTransaction(new JubCallback<String, String>() {
                    @Override
                    public void onSuccess(String s, String s2) {
                        dismissProgress();
                        showMsg(s);
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;
            case R.id.btn_eth_build:
                showProgress();
                mJubiter.ethBuild(new JubCallback<String, String>() {
                    @Override
                    public void onSuccess(String s, String s2) {
                        dismissProgress();
                        showMsg(s);
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;
            case R.id.btn_eth_contract:
                showProgress();
                mJubiter.ethContract(new JubCallback<String, String>() {
                    @Override
                    public void onSuccess(String s, String s2) {
                        dismissProgress();
                        showMsg(s);
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;
        }
    }

    private void getAppletVersion(final List<String> strings) {
        mJubiter.getAppletVersion(strings, new JubCallback<String, Void>() {
            @Override
            public void onSuccess(String s, Void aVoid) {
                showMsg(s);
            }

            @Override
            public void onFailed(int errorCode) {

            }
        });
    }

    private void switchBt() {
        if (isConnect) {
            mJubiter.disConnectDevice();
            mBtnScan.setText("连接");
            isConnect = false;
        } else {
            mJubiter.connect(new FtBTConnectCallback() {
                @Override
                public void connected(FTBTDevice device, int code) {
                    if (code == 0) {
                        mTxtState.setText(device.getName() + "\n" + device.getMac());
                        mBtnScan.setText("断开");
                        isConnect = true;
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

    @Override
    public void onPermissionsGranted(int requestCode, @NonNull List<String> perms) {

    }

    @Override
    public void onPermissionsDenied(int requestCode, @NonNull List<String> perms) {
        if (EasyPermissions.somePermissionPermanentlyDenied(this, perms)) {
            new AppSettingsDialog.Builder(this).build().show();
        }
        finish();
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        EasyPermissions.onRequestPermissionsResult(requestCode, permissions, grantResults, this);
    }

    public void showMsg(String msg) {
        new TipDialog().init(this)
                .setTitle("Result")
                .setMsg(msg)
                .setPositiveButton("OK", null)
                .show();
    }

    public void showProgress() {
        if (mDialog != null) {
            mDialog.setMessage("通讯中....");
            mDialog.show();
        }
    }

    public void dismissProgress() {
        if (mDialog != null) {
            mDialog.dismiss();
        }
    }
}
