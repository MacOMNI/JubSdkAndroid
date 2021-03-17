package com.jubiter.sdk.demo.impl;

/**
 * @Date 2020-12-22  13:49
 * @Author ZJF
 * @Version 1.0
 */
public class BioEnrollBean {
    private int fgptID;
    private int times;


    public BioEnrollBean() {
    }

    public BioEnrollBean(int fgptID, int times) {
        this.fgptID = fgptID;
        this.times = times;
    }

    public int getFgptID() {
        return fgptID;
    }

    public void setFgptID(int fgptID) {
        this.fgptID = fgptID;
    }

    public int getTimes() {
        return times;
    }

    public void setTimes(int times) {
        this.times = times;
    }
}
