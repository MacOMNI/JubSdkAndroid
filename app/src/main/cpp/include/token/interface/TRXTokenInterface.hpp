//
//  TRXTokenInterface.hpp
//  JubSDK
//
//  Created by Pan Min on 2020/12/13.
//  Copyright © 2020 JuBiter. All rights reserved.
//

#ifndef __TRXTokenInterface__
#define __TRXTokenInterface__

#include "JUB_SDK_TRX.h"

#include <vector>
#include <google/protobuf/any.pb.h>

namespace jub {


class TRXTokenInterface {
public:
    virtual ~TRXTokenInterface() = default;

    virtual JUB_RV SelectAppletTRX() = 0;
    virtual JUB_RV SetCoinTypeTRX() = 0;
    virtual JUB_RV GetAddressTRX(const std::string& path, const JUB_UINT16 tag, std::string& address) = 0;
    virtual JUB_RV GetHDNodeTRX(const JUB_BYTE format, const std::string& path, std::string& pubkey) = 0;
    virtual JUB_RV SignTXTRX(const std::vector<JUB_BYTE>& vPath,
                             const std::vector<JUB_BYTE>& vRaw,
                             std::vector<uchar_vector>& vSignatureRaw) = 0;
    virtual JUB_RV SetTRC20Token(const std::string& tokenName,
                                 const JUB_UINT16 unitDP,
                                 const std::string& contractAddress) = 0;
}; // class TRXTokenInterface end


} // namespace jub end

#endif
