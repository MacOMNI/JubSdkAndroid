//
//  ContextFIL.h
//  JubSDK
//
//  Created by Pan Min on 2020/11/12.
//  Copyright © 2020 JuBiter. All rights reserved.
//

#pragma once
#ifndef __ContextFIL__
#define __ContextFIL__

#include "JUB_SDK_FIL.h"

#include <string>
#include <vector>

#include "utility/Singleton.h"
#include "utility/xManager.hpp"

#include "context/Context.h"


#define JUB_CHECK_CONTEXT_FIL(x)                                    \
do {                                                                \
    auto context = jub::ContextManager::GetInstance()->GetOne(x);   \
    JUB_CHECK_NULL(context);                                        \
    const std::type_info& tCtx = typeid(*context);                  \
    const std::type_info& tCtxFIL = typeid(jub::ContextFIL);        \
    if (tCtx.hash_code() != tCtxFIL.hash_code()) {                  \
        return JUBR_ARGUMENTS_BAD;                                  \
    }                                                               \
} while(0)


namespace jub {

class ContextFIL : public Context {
public:
    ContextFIL(CONTEXT_CONFIG_FIL cfg, JUB_UINT16 deviceID) {
        _mainPath = cfg.mainPath;
        _deviceID = deviceID;
        _timeout = 120 * 2;
    }
    ~ContextFIL() {}

    virtual JUB_RV GetAddress(const BIP32_Path& path, const JUB_UINT16 tag, std::string& address);
    virtual JUB_RV SetMyAddress(const BIP32_Path& path, std::string& address);
    virtual JUB_RV GetHDNode(const JUB_BYTE& format, const BIP32_Path& path, std::string& pubkey);
    virtual JUB_RV GetMainHDNode(const JUB_BYTE format, std::string& xpub);

    virtual JUB_RV SignTransaction(const BIP32_Path& path,
                                   const JUB_UINT64 nonce,
                                   const JUB_UINT64 gasLimit,
                                   JUB_CHAR_CPTR gasFeeCapInAtto,
                                   JUB_CHAR_CPTR gasPremiumInAtto,
                                   JUB_CHAR_CPTR to,
                                   JUB_CHAR_CPTR valueInAtto,
                                   JUB_CHAR_CPTR input,
                                   OUT std::string& raw);

    virtual JUB_RV ActiveSelf() override;
}; // class ContextFIL end


} // namespace jub end

#endif // #pragma once
