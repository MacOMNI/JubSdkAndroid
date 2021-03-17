package com.jubiter.sdk.demo;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;

public class IntroActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_intro);
    }

    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.btn_main:
                startActivity(new Intent(this, MainActivity.class));
                break;
            case R.id.btn_ele:
                startActivity(new Intent(this, TestActivity.class));
                break;
            case R.id.btn_apdu:
                startActivity(new Intent(this, FidoTestActivity.class));
                break;
        }
    }
}
