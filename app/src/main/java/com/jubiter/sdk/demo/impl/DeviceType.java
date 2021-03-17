package com.jubiter.sdk.demo.impl;

public class DeviceType {
    /**
     * HID = 0x01,
     * BLE = 0x02,
     */
    private int COMMODE;
    /**
     * BLADE = 0x01,
     * BIO = 0x02
     */
    private int DEVICE;

    public int getCOMMODE() {
        return COMMODE;
    }

    public void setCOMMODE(int COMMODE) {
        this.COMMODE = COMMODE;
    }

    public int getDEVICE() {
        return DEVICE;
    }

    public void setDEVICE(int DEVICE) {
        this.DEVICE = DEVICE;
    }
}
