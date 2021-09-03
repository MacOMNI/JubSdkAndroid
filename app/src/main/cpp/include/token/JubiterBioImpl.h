#ifndef __JuBiterBioImpl__
#define __JuBiterBioImpl__


#include "JubiterBLDImpl.h"


namespace jub {


class JubiterBioImpl
: public JubiterBLDImpl {
public:
    JubiterBioImpl(std::string path);
    JubiterBioImpl(DeviceType* device);
    ~JubiterBioImpl();

    static stAppInfos g_appInfo[];

public:
    const JUB_UINT16 DEFAULT_FP_TIMEOUT = 8;

    static JUB_ENUM_BOOL ConfirmType(JUB_BYTE_PTR devName) {

        JUB_ENUM_BOOL b = JUB_ENUM_BOOL::BOOL_FALSE;

        if (   0 == std::string((char*)devName).find("JuBio")
            || 0 == std::string((char*)devName).find("hpy2")
            || 0 == std::string((char*)devName).find("HPYG2")
            || 0 == std::string((char*)devName).find("mw2")
            || 0 == std::string((char*)devName).find("MWG2")
            ) {
            b = JUB_ENUM_BOOL::BOOL_TRUE;
        }

        return b;
    }

    virtual bool IsBootLoader() override;

    virtual JUB_RV UIShowMain() override;

    virtual JUB_RV IdentityVerify(IN JUB_BYTE mode, OUT JUB_ULONG &retry) override;
    virtual JUB_RV IdentityVerifyPIN(IN JUB_BYTE mode, IN const std::string &pinMix, OUT JUB_ULONG &retry) override;
    virtual JUB_RV IdentityNineGrids(IN bool bShow) override;

    virtual JUB_RV VerifyFingerprint(OUT JUB_ULONG &retry) override;
    virtual JUB_RV EnrollFingerprint(IN JUB_UINT16 fpTimeout,
                                     INOUT JUB_BYTE_PTR fgptIndex, OUT JUB_ULONG_PTR ptimes,
                                     OUT JUB_BYTE_PTR fgptID) override;
    virtual JUB_RV EnumFingerprint(std::string& fgptList) override;
    virtual JUB_RV EraseFingerprint(IN JUB_UINT16 fpTimeout) override;
    virtual JUB_RV DeleteFingerprint(IN JUB_UINT16 fpTimeout,
                                     JUB_BYTE fgptID) override;

    //ETH functions
    virtual JUB_RV SignTXETH(const int erc,
                             const std::vector<JUB_BYTE>& vNonce,
                             const std::vector<JUB_BYTE>& vGasPrice,
                             const std::vector<JUB_BYTE>& vGasLimit,
                             const std::vector<JUB_BYTE>& vTo,
                             const std::vector<JUB_BYTE>& vValue,
                             const std::vector<JUB_BYTE>& vData,
                             const std::vector<JUB_BYTE>& vPath,
                             const std::vector<JUB_BYTE>& vChainID,
                             std::vector<JUB_BYTE>& vRaw) override;
    virtual JUB_RV SetERC20ETHToken(const std::string& tokenName,
                                    const JUB_UINT16 unitDP,
                                    const std::string& contractAddress) override;
    virtual JUB_RV SetERC20ETHTokens(const ERC20_TOKEN_INFO tokens[],
                                     const JUB_UINT16 iCount) override;

protected:
    // ERC20 token extension apdu
    const std::string  ETH_APPLET_VERSION_SUPPORT_EXT_TOKENS= std::string("05070000");  // 5.7.0
}; // class JubiterBioImpl end

}  // namespace jub end

#endif  // __JuBiterBioImpl__
