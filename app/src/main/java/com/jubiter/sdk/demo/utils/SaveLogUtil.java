package com.jubiter.sdk.demo.utils;

import android.os.Environment;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Date;


public class SaveLogUtil {

    public static void saveLog(final String msg) {
        new Thread(new Runnable() {
            @Override
            public void run() {
                String path = Environment.getExternalStorageDirectory().getPath();
                File fileDir = new File(path + File.separator + "_TEST_LOG");
                if (!fileDir.exists()) {
                    fileDir.mkdir();
                }
                File logFile = new File(fileDir.getAbsolutePath() + File.separator + getTime(false) + ".txt");
                if (!logFile.exists()) {
                    try {
                        logFile.createNewFile();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
                try {
                    FileOutputStream fos = new FileOutputStream(logFile, true);
                    //fos.write(getTime(true).getBytes());
                    //fos.write("\r\n".getBytes());
                    //fos.write(strType.getBytes());
                    //fos.write("\r\n".getBytes());
                    fos.write(msg.getBytes());
                    fos.write("\r\n".getBytes());
                    fos.close();
                } catch (FileNotFoundException e) {
                    e.printStackTrace();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }).start();
    }

    private static String getTime(boolean isAll) {
        SimpleDateFormat sdf;
        if (isAll) {
            sdf = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
        } else {
            sdf = new SimpleDateFormat("yyyyMMdd");
        }

        return sdf.format(new Date());
    }

    public static boolean deleteTodayLog(){
        String path = Environment.getExternalStorageDirectory().getPath();
        File fileDir = new File(path + File.separator + "_TEST_LOG");
        if (fileDir.exists()) {
            File logFile = new File(fileDir.getAbsolutePath() + File.separator + getTime(false) + ".txt");
            if (logFile.exists()) {
                return logFile.delete();
            }
        }
        return true;
    }

}
