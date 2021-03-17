package com.legendwd.hyperpay.main.hardwarewallet.jubnative.utils;

import java.util.List;

/**
 *
 * 同 testBTC.json 结构相同
 *
 * @author fengshuo
 * @date 2019/8/16
 * @time 19:59
 */
public class BtcTransInfo {

    private String ver;
    private boolean p2sh_segwit;
    private String main_path;
    private List<Input> inputs;
    private List<Output> outputs;
    private long lock_time;

    public String getVer() {
        return ver;
    }

    public void setVer(String ver) {
        this.ver = ver;
    }

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

    public List<Input> getInputs() {
        return inputs;
    }

    public void setInputs(List<Input> inputs) {
        this.inputs = inputs;
    }

    public List<Output> getOutputs() {
        return outputs;
    }

    public void setOutputs(List<Output> outputs) {
        this.outputs = outputs;
    }

    public long getLock_time() {
        return lock_time;
    }

    public void setLock_time(long lock_time) {
        this.lock_time = lock_time;
    }

    public static class Input {

        private String preHash;
        private long preIndex;
        private long amount;
        private Bip32Path bip32_path;
        private long nSequence;
        private MultiSigType multisig;

        public Input() {
        }

        public String getPreHash() {
            return preHash;
        }

        public void setPreHash(String preHash) {
            this.preHash = preHash;
        }

        public long getPreIndex() {
            return preIndex;
        }

        public void setPreIndex(long preIndex) {
            this.preIndex = preIndex;
        }

        public long getAmount() {
            return amount;
        }

        public void setAmount(long amount) {
            this.amount = amount;
        }

        public Bip32Path getBip32_path() {
            return bip32_path;
        }

        public void setBip32_path(Bip32Path bip32_path) {
            this.bip32_path = bip32_path;
        }

        public long getnSequence() {
            return nSequence;
        }

        public void setnSequence(long nSequence) {
            this.nSequence = nSequence;
        }

        public MultiSigType getMultisig() {
            return multisig;
        }

        public void setMultisig(MultiSigType multisig) {
            this.multisig = multisig;
        }
    }

    public static class Output {

        private String address;
        private long amount;
        private boolean change_address;
        private Bip32Path bip32_path;
        private MultiSigType multisig;


        public String getAddress() {
            return address;
        }

        public void setAddress(String address) {
            this.address = address;
        }

        public long getAmount() {
            return amount;
        }

        public void setAmount(long amount) {
            this.amount = amount;
        }

        public boolean isChange_address() {
            return change_address;
        }

        public void setChange_address(boolean change_address) {
            this.change_address = change_address;
        }

        public Bip32Path getBip32_path() {
            return bip32_path;
        }

        public void setBip32_path(Bip32Path bip32_path) {
            this.bip32_path = bip32_path;
        }

        public MultiSigType getMultisig() {
            return multisig;
        }

        public void setMultisig(MultiSigType multisig) {
            this.multisig = multisig;
        }
    }

    public static class Bip32Path {
        private boolean change;
        private int addressIndex;

        public Bip32Path() {
        }

        public Bip32Path(boolean change, int addressIndex) {
            this.change = change;
            this.addressIndex = addressIndex;
        }

        public boolean isChange() {
            return change;
        }

        public void setChange(boolean change) {
            this.change = change;
        }

        public int getAddressIndex() {
            return addressIndex;
        }

        public void setAddressIndex(int addressIndex) {
            this.addressIndex = addressIndex;
        }
    }

}
