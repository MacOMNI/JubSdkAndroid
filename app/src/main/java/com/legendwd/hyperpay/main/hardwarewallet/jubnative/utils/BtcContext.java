package com.legendwd.hyperpay.main.hardwarewallet.jubnative.utils;

import java.util.List;

/**
 * @author fengshuo
 * @date 2019/8/19
 * @time 16:44
 */
public class BtcContext {

    private boolean p2sh_segwit;
    private String main_path;
    private int m;
    private int n;
    private List<String> cosigner;

    public BtcContext() {}

    public boolean isP2sh_segwit() {
        return p2sh_segwit;
    }

    public void setP2sh_segwit(boolean p2sh_segwit) {
        this.p2sh_segwit = p2sh_segwit;
    }

    public String getMain_path() {
        return main_path;
    }

    public void setMain_path(String main_path) {
        this.main_path = main_path;
    }

    public int getM() {
        return m;
    }

    public void setM(int m) {
        this.m = m;
    }

    public int getN() {
        return n;
    }

    public void setN(int n) {
        this.n = n;
    }

    public List<String> getCosigner() {
        return cosigner;
    }

    public void setCosigner(List<String> cosigner) {
        this.cosigner = cosigner;
    }
}
