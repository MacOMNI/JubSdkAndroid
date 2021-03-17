package com.jubiter.sdk.demo.utils;

import android.app.AlertDialog;
import android.app.Dialog;
import android.content.Context;
import android.text.InputFilter;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;

import com.jubiter.sdk.demo.R;

/**
 * @Date 2018/5/18  13:40
 * @Author ZJF
 * @Version 1.0
 */
public class InputDialog {

    private AlertDialog mDialog;
    private EditText mEditText;


    public InputDialog(Context context, String title, final callback listener) {
        View view = LayoutInflater.from(context).inflate(R.layout.dialog_input, null);

        mEditText = view.findViewById(R.id.verify_et);
        mDialog = new AlertDialog.Builder(context)
                .setTitle(title)
                .setView(view)
                .setCancelable(false)
                .create();
        ((Button) view.findViewById(R.id.btn_sure)).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mDialog.dismiss();
                ThreadUtils.toMainThread(new Runnable() {
                    @Override
                    public void run() {
                        listener.onClickListener(getPin());
                    }
                });
            }
        });

        ((Button) view.findViewById(R.id.btn_cancel)).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mDialog.dismiss();
                ThreadUtils.toMainThread(new Runnable() {
                    @Override
                    public void run() {
                        listener.onCancel();
                    }
                });
            }
        });

    }

    public InputDialog(Context context, final callback listener) {
        this(context, "请输入密码", listener);
    }

    public InputDialog setInputType(int type){
        mEditText.setInputType(type);
        return this;
    }

    public InputDialog setFilters(InputFilter[] filters){
        mEditText.setFilters(filters);
        return this;
    }

    public interface callback {
        void onClickListener(String value);

        void onCancel();
    }

    public String getPin() {
        return mEditText.getText().toString().trim();
    }

    public void show() {
        mDialog.show();
    }
}
