#include "token/JubiterBioImpl.h"

namespace jub {


JUB_RV JubiterBioImpl::SignTXETH(const bool bERC20,
                                 const std::vector<JUB_BYTE>& vNonce,
                                 const std::vector<JUB_BYTE>& vGasPrice,
                                 const std::vector<JUB_BYTE>& vGasLimit,
                                 const std::vector<JUB_BYTE>& vTo,
                                 const std::vector<JUB_BYTE>& vValue,
                                 const std::vector<JUB_BYTE>& vData,
                                 const std::vector<JUB_BYTE>& vPath,
                                 const std::vector<JUB_BYTE>& vChainID,
                                 std::vector<JUB_BYTE>& vRaw) {

    // Remove subpackage sending APDU.
//    if (_appletVersion >= stVersionExp::FromString(ETH_APPLET_VERSION_SUPPORT_EXT_TOKENS)) {
//        return _SignTXUpgradeETH(bERC20,
//                                 vNonce,
//                                 vGasPrice,
//                                 vGasLimit,
//                                 vTo,
//                                 vValue,
//                                 vData,
//                                 vPath,
//                                 vChainID,
//                                 vRaw);
//    }
//    else {
        return _SignTXETH(bERC20,
                          vNonce,
                          vGasPrice,
                          vGasLimit,
                          vTo,
                          vValue,
                          vData,
                          vPath,
                          vChainID,
                          vRaw);
//    }
}


JUB_RV JubiterBioImpl::SetERC20ETHToken(const std::string& tokenName, const JUB_UINT16 unitDP, const std::string& contractAddress) {

    return SetERC20Token(tokenName, unitDP, contractAddress);
}

JUB_RV JubiterBioImpl::SetERC20ETHTokens(const ERC20_TOKEN_INFO tokens[],
                                         const JUB_UINT16 iCount) {

    // ETH token extension apdu
    if (JubiterBioImpl::_appletVersion >= stVersionExp::FromString(JubiterBioImpl::ETH_APPLET_VERSION_SUPPORT_EXT_TOKENS)) {
        return SetERC20Tokens(tokens, iCount);
    }
    else if (1 == iCount) {
        return SetERC20ETHToken(tokens[0].tokenName, tokens[0].unitDP, tokens[0].contractAddress);
    }

    return JUBR_ARGUMENTS_BAD;
}

} // namespace jub end
