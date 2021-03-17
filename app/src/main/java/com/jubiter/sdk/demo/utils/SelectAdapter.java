package com.jubiter.sdk.demo.utils;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.TextView;

import com.jubiter.sdk.demo.R;

import java.util.List;

public class SelectAdapter extends BaseAdapter {

    protected List<String> strings;
    protected Context mContext;

    public SelectAdapter(Context context, List<String> list) {
        mContext = context;
        strings = list;
    }

    @Override
    public int getCount() {
        return strings.size();
    }

    @Override
    public String getItem(int position) {
        return strings.get(position);
    }

    @Override
    public long getItemId(int arg0) {
        return arg0;
    }

    @Override
    public View getView(final int position, View convertView, ViewGroup parent) {
        MyViewHolder mViewHolder;
        if (convertView == null) {
            convertView = LayoutInflater.from(mContext).inflate(
                    R.layout.device_list_item, parent, false);
            mViewHolder = new MyViewHolder();
            mViewHolder.tv = (TextView) convertView.findViewById(R.id.text_device_name);
            convertView.setTag(mViewHolder);
        } else {
            mViewHolder = (MyViewHolder) convertView.getTag();
        }
        mViewHolder.tv.setText(strings.get(position));
        return convertView;
    }


    public void addItem(String item) {
        strings.add(item);
    }

    public void clean() {
        strings.clear();
    }

    public class MyViewHolder {
        public TextView tv;
        public ImageView iv;
    }
}