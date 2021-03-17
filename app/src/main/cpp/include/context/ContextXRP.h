//
//  ContextXRP.h
//  JubSDK
//
//  Created by Pan Min on 2019/11/25.
//  Copyright © 2019 JuBiter. All rights reserved.
//

#pragma once
#ifndef __ContextXRP__
#define __ContextXRP__

#include "JUB_SDK_XRP.h"

#include <string>
#include <vector>

#include "utility/Singleton.h"
#include "utility/xManager.hpp"

#include "context/Context.h"

#define JUB_CHECK_CONTEXT_XRP(x)                                    \
do {                                                                \
    auto context = jub::ContextManager::GetInstance()->GetOne(x);   \
    JUB_CHECK_NULL(context);                                        \
    const std::type_info& tCtx = typeid(*context);                  \
    const std::type_info& tCtxXRP = typeid(jub::ContextXRP);        \
    if (tCtx.hash_code() != tCtxXRP.hash_code()) {                  \
        return JUBR_ARGUMENTS_BAD;                                  \
    }                                                               \
} while(0)

namespace jub {

class ContextXRP : public Context {
public:
    ContextXRP(CONTEXT_CONFIG_XRP cfg, JUB_UINT16 deviceID) {
        _mainPath = cfg.mainPath;
        _deviceID = deviceID;
        _timeout = 120 * 2;
    }
    ~ContextXRP() {}

    virtual JUB_RV GetAddress(const BIP32_Path& path, const JUB_UINT16 tag, std::string& address);
    virtual JUB_RV SetMyAddress(const BIP32_Path& path, std::string& address);
    virtual JUB_RV GetHDNode(const JUB_BYTE& format, const BIP32_Path& path, std::string& pubkey);
    virtual JUB_RV GetMainHDNode(const JUB_BYTE format, std::string& xpub);

    virtual JUB_RV SignTransaction(const BIP32_Path& path,
                                   const JUB_TX_XRP& tx,
                                   std::string& signedRaw);

    virtual JUB_RV ActiveSelf() override;
}; // class ContextXRP end

} // namespace jub end

#endif // #pragma once
