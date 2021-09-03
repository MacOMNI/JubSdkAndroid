#ifndef __ETHTokenInterface__
#define __ETHTokenInterface__

#include "JUB_SDK_ETH.h"
#include "utility/Version.hpp"

#include <vector>

namespace jub {

enum JUB_ENUM_APDU_ERC_ETH : uint16_t {
    ERC_INVALID = 0,
    ERC_20   = 0x0014,
};
enum JUB_ENUM_APDU_ERC_P1 : uint8_t {
    ERC20  = 0x00,
    ERC721 = 0x01,
    TOKENS_INFO = 0x02,  // Compatible with both ERC20 & ERC721
};

class ETHTokenInterface {

public:
    virtual JUB_RV SelectAppletETH() = 0;
    virtual JUB_RV GetAppletVersionETH(stVersion& version) = 0;
    virtual JUB_RV GetAddressETH(const std::string& path, const JUB_UINT16 tag, std::string& address) = 0;
    virtual JUB_RV GetHDNodeETH(const JUB_BYTE format, const std::string& path, std::string& pubkey) = 0;
    virtual JUB_RV SignTXETH(const int erc,
                             const std::vector<JUB_BYTE>& vNonce,
                             const std::vector<JUB_BYTE>& vGasPrice,
                             const std::vector<JUB_BYTE>& vGasLimit,
                             const std::vector<JUB_BYTE>& vTo,
                             const std::vector<JUB_BYTE>& vValue,
                             const std::vector<JUB_BYTE>& vData,
                             const std::vector<JUB_BYTE>& vPath,
                             const std::vector<JUB_BYTE>& vChainID,
                             std::vector<JUB_BYTE>& vRaw) = 0;
    virtual JUB_RV SetERC20ETHToken(const std::string& tokenName,
                                    const JUB_UINT16 unitDP,
                                    const std::string& contractAddress) = 0;
    virtual JUB_RV SetERC20ETHTokens(const ERC20_TOKEN_INFO tokens[],
                                     const JUB_UINT16 iCount) = 0;
    virtual JUB_RV SignContractETH(const JUB_BYTE inputType,
                                   const std::vector<JUB_BYTE>& vNonce,
                                   const std::vector<JUB_BYTE>& vGasPrice,
                                   const std::vector<JUB_BYTE>& vGasLimit,
                                   const std::vector<JUB_BYTE>& vTo,
                                   const std::vector<JUB_BYTE>& vValue,
                                   const std::vector<JUB_BYTE>& vInput,
                                   const std::vector<JUB_BYTE>& vPath,
                                   const std::vector<JUB_BYTE>& vChainID,
                                   std::vector<JUB_BYTE>& signatureRaw) = 0;
    virtual JUB_RV SignContractHashETH(const JUB_BYTE inputType,
                                       const std::vector<JUB_BYTE>& vGasLimit,
                                       const std::vector<JUB_BYTE>& vInputHash,
                                       const std::vector<JUB_BYTE>& vUnsignedTxHash,
                                       const std::vector<JUB_BYTE>& vPath,
                                       const std::vector<JUB_BYTE>& vChainID,
                                       std::vector<JUB_BYTE>& signatureRaw) = 0;
}; // class ETHTokenInterface end

} // namespace jub end

#endif
