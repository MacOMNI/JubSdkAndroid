//
//  FILTokenInterface.hpp
//  JubSDK
//
//  Created by Pan Min on 2021/4/27.
//  Copyright © 2020 JuBiter. All rights reserved.
//

#ifndef __FILTokenInterface__
#define __FILTokenInterface__

#include "JUB_SDK_FIL.h"

#include <vector>

#include "mSIGNA/stdutils/uchar_vector.h"
#include "utility/util.h"
#include <uint256_t/uint256_t.h>

namespace jub {

class FILTokenInterface {

public:
    virtual JUB_RV SelectAppletFIL() = 0;
    virtual JUB_RV SetCoinTypeFIL() = 0;
    virtual JUB_RV GetAppletVersionFIL(std::string& version) = 0;
    virtual JUB_RV GetAddressFIL(const std::string& path, const JUB_UINT16 tag, std::string& address) = 0;
    virtual JUB_RV GetHDNodeFIL(const JUB_BYTE format, const std::string& path, std::string& pubkey) = 0;
    virtual JUB_RV SignTXFIL(const uint64_t& nonce,
                             const uint256_t& glimit,
                             const uint256_t& gfeeCap,
                             const uint256_t& gpremium,
                             const std::string& to,
                             const uint256_t& value,
                             const std::string& input,
                             const std::string& path,
                             std::vector<uchar_vector>& vSignatureRaw) = 0;
}; // class FILTokenInterface end

} // namespace jub end

#endif
