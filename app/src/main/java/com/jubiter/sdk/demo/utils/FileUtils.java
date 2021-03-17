package com.jubiter.sdk.demo.utils;

import android.content.Context;
import android.content.SharedPreferences;
import android.os.Environment;
import android.text.TextUtils;


import com.google.gson.Gson;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

/**
 * @Date 2018/4/19  16:06
 * @Author ZJF
 * @Version 1.0
 */
public class FileUtils {

    public static List<String> getApdu() {
        return getApduStrList();
    }

    private static List<String> getApduStrList() {
        List<String> result = new ArrayList<>();
        String path = Environment.getExternalStorageDirectory().getPath();
        File file = new File(path + File.separator + "jubiter_apdu.txt");
        if (file.exists()) {
            BufferedReader br = null;
            try {
                br = new BufferedReader(new FileReader(file));
                String str = null;
                while ((str = br.readLine()) != null) {
                    result.add(str);
                }
            } catch (FileNotFoundException e) {
                e.printStackTrace();
            } catch (IOException e) {
                e.printStackTrace();
            } finally {
                try {
                    if (br != null) {
                        br.close();
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
        return result;
    }
}
