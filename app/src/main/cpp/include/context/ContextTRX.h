//
//  ContextTRX.h
//  JubSDK
//
//  Created by Pan Min on 2020/12/13.
//  Copyright © 2020 JuBiter. All rights reserved.
//

#pragma once
#ifndef __ContextTRX__
#define __ContextTRX__

#include "JUB_SDK_TRX.h"

#include <string>
#include <vector>

#include "utility/Singleton.h"
#include "utility/xManager.hpp"

#include "context/Context.h"


#define JUB_CHECK_CONTEXT_TRX(x)                                    \
do {                                                                \
    auto context = jub::ContextManager::GetInstance()->GetOne(x);   \
    JUB_CHECK_NULL(context);                                        \
    const std::type_info& tCtx = typeid(*context);                  \
    const std::type_info& tCtxTRX = typeid(jub::ContextTRX);        \
    if (tCtx.hash_code() != tCtxTRX.hash_code()) {                  \
        return JUBR_ARGUMENTS_BAD;                                  \
    }                                                               \
} while(0)


namespace jub {

class ContextTRX : public Context {
public:
    ContextTRX(CONTEXT_CONFIG_TRX cfg, JUB_UINT16 deviceID) {
        _mainPath = cfg.mainPath;
        _deviceID = deviceID;
         _timeout = 120 * 2;
    }
    ~ContextTRX() {}

    virtual JUB_RV GetAddress(const BIP32_Path& path, const JUB_UINT16 tag, std::string& address);
    virtual JUB_RV SetMyAddress(const BIP32_Path& path, std::string& address);
    virtual JUB_RV GetHDNode(const JUB_BYTE format, const BIP32_Path& path, std::string& pubkey);
    virtual JUB_RV GetMainHDNode(const JUB_BYTE format, std::string& xpub);

    virtual JUB_RV SignTransaction(const BIP32_Path& path,
                                   JUB_CHAR_CPTR packedContractInPb,
                                   std::string& rawInJSON);

    virtual JUB_RV BuildTRC20Abi(JUB_CHAR_CPTR to, JUB_CHAR_CPTR value, std::string& abi);
    virtual JUB_RV SetTRC20Token(JUB_CHAR_CPTR pTokenName,
                                 JUB_UINT16 unitDP,
                                 JUB_CHAR_CPTR pContractAddress);

    virtual JUB_RV PackTransactionRaw(const JUB_TX_TRX& tx,
                                      std::string& packedContractInPB);

    virtual JUB_RV ActiveSelf() override;
}; // class ContextTRX end

} // namespace jub end

#endif // #pragma once
