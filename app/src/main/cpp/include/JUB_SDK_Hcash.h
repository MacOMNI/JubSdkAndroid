#pragma once
#include "JUB_SDK_BTC.h"


typedef stContextCfg CONTEXT_CONFIG_HC;
typedef stContextCfgMultisigBTC CONTEXT_CONFIG_MULTISIG_HC;


typedef struct stInputHC {
	JUB_UINT64		amount;
	BIP32_Path      path;

     stInputHC();
    ~stInputHC() = default;
} INPUT_HC;


typedef struct stOutputHC {
	JUB_ENUM_BOOL   changeAddress;
	BIP32_Path      path;

     stOutputHC();
    ~stOutputHC() = default;
} OUTPUT_HC;


JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_CreateContextHC(IN CONTEXT_CONFIG_HC cfg,
                           IN JUB_UINT16 deviceID,
                           OUT JUB_UINT16* contextID);


JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_CreateMultiSigContextHC(IN CONTEXT_CONFIG_MULTISIG_HC cfg,
                                   IN JUB_UINT16 deviceID,
                                   OUT JUB_UINT16* contextID);


JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_GetAddressHC(IN JUB_UINT16 contextID,
                        IN BIP32_Path path,
                        IN JUB_ENUM_BOOL bShow,
                        OUT JUB_CHAR_PTR_PTR address);


JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_GetHDNodeHC(IN JUB_UINT16 contextID,
                       IN BIP32_Path path,
                       OUT JUB_CHAR_PTR_PTR xpub);


JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_GetMainHDNodeHC(IN JUB_UINT16 contextID,
                           OUT JUB_CHAR_PTR_PTR xpub);


JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_SignTransactionHC(IN JUB_UINT16 contextID,
                             IN INPUT_HC inputs[], IN JUB_UINT16 iCount,
                             IN OUTPUT_HC outputs[], IN JUB_UINT16 oCount,
                             IN JUB_CHAR_PTR unsignedTrans,
                             OUT JUB_CHAR_PTR_PTR raw);
