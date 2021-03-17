package com.jubiter.sdk.demo.utils;

import android.app.AlertDialog;
import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ListView;
import android.widget.TextView;

import com.jubiter.sdk.demo.R;
import com.jubiter.sdk.demo.utils.SelectAdapter;

import java.util.List;

/**
 * @Date 2018/5/12  22:04
 * @Author ZJF
 * @Version 1.0
 */
public class SelectDialog {

    private TextView mTitleTv,mCancel;
    private ListView mListView;
    private AlertDialog mDialog;
    private SelectAdapter mSelectAdapter;

    public SelectDialog() {
    }

    public SelectDialog init(Context context, List<String> dataList) {

        View view = LayoutInflater.from(context).inflate(R.layout.dialog_select, null);
        mDialog = new AlertDialog.Builder(context)
                .setCancelable(true)
                .setView(view).create();
        mDialog.setCanceledOnTouchOutside(false);
        mTitleTv = view.findViewById(R.id.dialog_tip_tv);
        mListView = view.findViewById(R.id.list_select);
        mCancel = view.findViewById(R.id.dialog_cancel);
        mSelectAdapter = new SelectAdapter(context, dataList);
        mListView.setAdapter(mSelectAdapter);
        mCancel.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                mDialog.dismiss();
            }
        });
        return this;
    }

    public SelectDialog setTitle(String title) {
        mTitleTv.setText(title);
        mTitleTv.setVisibility(View.VISIBLE);
        return this;
    }

    public SelectDialog setTitle(int id) {
        mTitleTv.setText(id);
        mTitleTv.setVisibility(View.VISIBLE);
        return this;
    }

    public void addItem(String item) {
        mSelectAdapter.addItem(item);
        mSelectAdapter.notifyDataSetChanged();
    }

    public SelectDialog setOnItemClickListener(AdapterView.OnItemClickListener listener){
        mListView.setOnItemClickListener(listener);
        return this;
    }

    public SelectDialog setCancelable(boolean cancelable) {
        mDialog.setCancelable(cancelable);
        return this;
    }

    public void show() {
        mDialog.show();
    }

    public void dismiss() {
        mDialog.dismiss();
    }

}
