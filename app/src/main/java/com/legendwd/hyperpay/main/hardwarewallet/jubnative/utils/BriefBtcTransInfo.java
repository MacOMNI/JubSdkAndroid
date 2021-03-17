package com.legendwd.hyperpay.main.hardwarewallet.jubnative.utils;

import java.util.List;

/**
 * @author fengshuo
 * @date 2019/8/16
 * @time 15:25
 */
public class BriefBtcTransInfo {

    private List<Input> inputs;
    private List<Output> output;
    private long lock_time;

    public List<Input> getInputs() {
        return inputs;
    }

    public void setInputs(List<Input> inputs) {
        this.inputs = inputs;
    }

    public List<Output> getOutput() {
        return output;
    }

    public void setOutput(List<Output> output) {
        this.output = output;
    }

    public long getLock_time() {
        return lock_time;
    }

    public void setLock_time(long lock_time) {
        this.lock_time = lock_time;
    }

    public class Input {
        private long amount;
        private MultiSigType multisig;
        private long nSequence;
        private String preHash;
        private long preIndex;

        public long getAmount() {
            return amount;
        }

        public void setAmount(long amount) {
            this.amount = amount;
        }

        public MultiSigType getMultisig() {
            return multisig;
        }

        public void setMultisig(MultiSigType multisig) {
            this.multisig = multisig;
        }

        public long getnSequence() {
            return nSequence;
        }

        public void setnSequence(long nSequence) {
            this.nSequence = nSequence;
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
    }

    public class Output {
        private long amount;
        private MultiSigType multisig;
        private String address;

        public long getAmount() {
            return amount;
        }

        public void setAmount(long amount) {
            this.amount = amount;
        }

        public MultiSigType getMultisig() {
            return multisig;
        }

        public void setMultisig(MultiSigType multisig) {
            this.multisig = multisig;
        }

        public String getAddress() {
            return address;
        }

        public void setAddress(String address) {
            this.address = address;
        }
    }
}
