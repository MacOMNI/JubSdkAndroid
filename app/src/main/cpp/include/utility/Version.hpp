//
//  Version.hpp
//  JubSDK
//
//  Created by panmin on 2021/6/3.
//  Copyright © 2021 sherlockirene. All rights reserved.
//

#ifndef Version_hpp
#define Version_hpp

#include <stdio.h>
#include "mSIGNA/stdutils/uchar_vector.h"

typedef struct stVersion {
    JUB_UINT8  major;   // major number
    JUB_UINT8  minor;   // minor number
    JUB_UINT16 patch;   // patch number
} JUB_VERSION;
typedef JUB_VERSION* JUB_VERSION_PTR;


//extern struct stVersion;
struct stVersionExp : stVersion {
    stVersionExp() {
        major = 0;
        minor = 0;
        patch = 0;
    }
    stVersionExp(JUB_UINT8 _major, JUB_UINT8 _minor, JUB_UINT16 _patch)
    {
        major = _major;
        minor = _minor;
        patch = _patch;
    }

    friend bool operator<(const stVersionExp& lhs, const stVersionExp& rhs) {
//        return  (lhs.major  < rhs.major)
//            || ((lhs.major == rhs.major) && (lhs.minor  < rhs.minor))
//            || ((lhs.major == rhs.major) && (lhs.minor == rhs.minor) && (lhs.patch < rhs.patch));
        return std::tie(lhs.major, lhs.minor, lhs.patch) < std::tie(rhs.major, rhs.minor, rhs.patch);
    }

    friend bool operator>(const stVersionExp& lhs, const stVersionExp& rhs) {
//        return  (lhs.major  > rhs.major)
//            || ((lhs.major == rhs.major) && (lhs.minor  > rhs.minor))
//            || ((lhs.major == rhs.major) && (lhs.minor == rhs.minor) && (lhs.patch > rhs.patch));
        return std::tie(lhs.major, lhs.minor, lhs.patch) > std::tie(rhs.major, rhs.minor, rhs.patch);
    }

    friend bool operator==(const stVersionExp& lhs, const stVersionExp& rhs) {
//        return lhs.major == rhs.major
//            && lhs.minor == rhs.minor
//            && lhs.patch == rhs.patch;
        return std::tie(lhs.major, lhs.minor, lhs.patch) == std::tie(rhs.major, rhs.minor, rhs.patch);
    }

    friend bool operator>=(const stVersionExp& lhs, const stVersionExp& rhs) {
        return !(lhs < rhs);
    }

    static stVersionExp FromString(const std::string& versionStr) {
        stVersionExp versionExp;
        // versionStr is hex string of 4 bytes
        assert(versionStr.size() == 8);
        // decode hex string to bytes
        uchar_vector versionBytes(versionStr);

        versionExp.major = versionBytes[0];
        versionExp.minor = versionBytes[1];

        if (1 == versionExp.major) {
            // 1.x version update logic is different
            versionExp.patch = versionBytes[2]*10 + versionBytes[3];
        } else {
            // other version logic
            versionExp.patch = versionBytes[2]*256 + versionBytes[3];
        }

        return versionExp;
    }

    static std::string ToString(const stVersionExp& versionExp) {
        uchar_vector v;
        v <<  (uint8_t)versionExp.major;
        v <<  (uint8_t)versionExp.minor;
        if (1 == versionExp.major) {
            // 1.x version update logic is different
            v << (uint8_t)(versionExp.patch/10);
            v << (uint8_t)(versionExp.patch%10);
        }
        else {
            // other version logic
            v << (uint8_t)(versionExp.patch/256);
            v << (uint8_t)(versionExp.patch%256);
        }

        return v.getHex();
    }

    static std::string ToString(const stVersion& version) {
        stVersionExp versionExp;
        versionExp.major = version.major;
        versionExp.minor = version.minor;
        versionExp.patch = version.patch;

        return ToString(versionExp);
    }
};

#endif /* Version_hpp */
