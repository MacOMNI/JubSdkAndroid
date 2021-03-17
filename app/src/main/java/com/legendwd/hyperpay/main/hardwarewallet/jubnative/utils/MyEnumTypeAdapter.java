package com.legendwd.hyperpay.main.hardwarewallet.jubnative.utils;

import com.google.gson.TypeAdapter;
import com.google.gson.stream.JsonReader;
import com.google.gson.stream.JsonWriter;

import java.io.IOException;

/**
 * @author fengshuo
 * @date 2019/8/19
 * @time 15:56
 */
public class MyEnumTypeAdapter<T> extends TypeAdapter<T> {

    @Override
    public void write(JsonWriter out, T value) throws IOException {
        if (value == null) {
            out.nullValue();
            return;
        }
        MultiSigType status = (MultiSigType) value;
        out.value(status.ordinal());
    }

    @Override
    public T read(JsonReader in) throws IOException {
        // Properly deserialize the input (if you use deserialization)
        return null;
    }
}