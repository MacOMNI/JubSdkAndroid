#ifndef __HardwareTokenImpl__
#define __HardwareTokenImpl__

#include "JUB_SDK_DEV_BLE.h"
#include <memory>

#include "utility/util.h"

#include "token/interface/TokenInterface.hpp"
#include <token/interface/HCTokenInterface.hpp>
#include "device/ApduBuilder.hpp"
#include "device/JubiterHidDevice.hpp"

#ifdef HID_MODE // modify later..
#include "device/JubiterHidDevice.hpp"
using DeviceType = jub::JubiterHidDevice;
#else
#include "device/JubiterBLEDevice.hpp"
using DeviceType = jub::JubiterBLEDevice;
#endif

namespace jub {

constexpr JUB_BYTE kPKIAID_FIDO[8] = {
    0xa0, 0x00,	0x00, 0x06, 0x47, 0x2f, 0x00, 0x01
};

constexpr JUB_BYTE kPKIAID_BTC[16] = {
    0xD1, 0x56, 0x00, 0x01, 0x32, 0x83, 0x00, 0x42, 0x4C, 0x44, 0x00, 0x00, 0x42, 0x54, 0x43, 0x01
};

constexpr JUB_BYTE kPKIAID_HC[14] = {
    0x63, 0x6F, 0x6D, 0x2E, 0x66, 0x74, 0x2E, 0x68, 0x63, 0x72, 0x61, 0x73, 0x68, 0x01
};

constexpr JUB_BYTE kPKIAID_ETH[16] = {
    0xD1, 0x56, 0x00, 0x01, 0x32, 0x83, 0x00, 0x42, 0x4C, 0x44, 0x00, 0x00, 0x45, 0x54, 0x48, 0x01
};

constexpr JUB_BYTE kPKIAID_EOS[16] = {
    0xD1, 0x56, 0x00, 0x01, 0x32, 0x03, 0x00, 0x42, 0x4C, 0x44, 0x00, 0x00, 0x45, 0x54, 0x49, 0x01
};

constexpr JUB_BYTE kPKIAID_MISC[16] = {
    0xD1, 0x56, 0x00, 0x01, 0x32, 0x03, 0x00, 0x42, 0x4C, 0x44, 0x00, 0x6D, 0x69, 0x73, 0x63, 0x01
};

typedef enum class enumCoinTypeETH {
    COINETH = 0x00,
    COINFIL = 0x01,
    Default = COINETH
} JUB_ENUM_COINTYPE_ETH;

typedef enum class enumCoinTypeMisc {
    COIN = 0x00,
    COINEOS = 0x01,
    COINXRP = 0x02,
    COINTRX = 0x03,
    Default = COIN
} JUB_ENUM_COINTYPE_MISC;

typedef struct _stAppInfos_ {
    abcd::DataChunk appID;
    std::string coinName;
    std::string minimumAppletVersion;
} stAppInfos;


class JubiterBLDImpl : public CommonTokenInterface,
                       public ETHTokenInterface,
                       public HCTokenInterface,
                       public EOSTokenInterface,
                       public XRPTokenInterface,
                       public TRXTokenInterface,
                       public FILTokenInterface
{
public:
    static stAppInfos g_appInfo[];

public:
    JubiterBLDImpl(std::string path);
    JubiterBLDImpl(DeviceType* device);
    ~JubiterBLDImpl();

public:
    /* functions */
    static JUB_ENUM_BOOL ConfirmType(JUB_BYTE_PTR devName) {

        JUB_ENUM_BOOL b = JUB_ENUM_BOOL::BOOL_FALSE;

        if (   0 == std::string((char*)devName).find("JuBiter")
            || 0 == std::string((char*)devName).find("hpy")
            || 0 == std::string((char*)devName).find("HPY")
            || 0 == std::string((char*)devName).find("mw")
            || 0 == std::string((char*)devName).find("MW")
            ) {
            b = JUB_ENUM_BOOL::BOOL_TRUE;
        }

        return b;
    }

    virtual JUB_RV ConnectToken() override;
    virtual JUB_RV DisconnectToken() override;
    
    virtual JUB_RV SetERC20Token(const std::string& tokenName,
                                 const JUB_UINT16 unitDP,
                                 const std::string& contractAddress) override;
    virtual JUB_RV SetERC20Tokens(const ERC20_TOKEN_INFO tokens[],
                                  const JUB_UINT16 iCount) override;

	//HC functions
	virtual JUB_RV selectApplet_HC() override;

    //BTC functions
    virtual JUB_RV SelectAppletBTC() override;
    virtual JUB_RV GetHDNodeBTC(const JUB_BTC_TRANS_TYPE& type, const std::string& path, std::string& xpub) override;
    virtual JUB_RV GetAddressBTC(const JUB_BYTE addrFmt, const JUB_BTC_TRANS_TYPE& type, const std::string& path, const JUB_UINT16 tag, std::string& address) override;
    virtual JUB_RV GetAddressMultisigBTC(const JUB_BYTE addrFmt,
                                         const JUB_BTC_TRANS_TYPE& type,
                                         const std::string& path,
                                         const JUB_UINT16 tag,
                                         const uchar_vector& vRedeemScriptTlv,
                                         std::string& address) override;
    virtual JUB_RV SetUnitBTC(const JUB_BTC_UNIT_TYPE& unit) override;
    virtual JUB_RV SetCoinTypeBTC(const JUB_ENUM_COINTYPE_BTC& type) override;
    virtual JUB_RV SignTXBTC(const JUB_BYTE addrFmt,
                             const JUB_BTC_TRANS_TYPE& type,
                             const JUB_UINT16 inputCount,
                             const std::vector<JUB_UINT64>& vInputAmount,
                             const std::vector<std::string>& vInputPath,
                             const std::vector<JUB_UINT16>& vChangeIndex,
                             const std::vector<std::string>& vChangePath,
                             const std::vector<JUB_BYTE>& vUnsigedTrans,
                             std::vector<JUB_BYTE>& vRaw) override;
    virtual JUB_RV SignTXMultisigBTC(const JUB_BYTE addrFmt,
                                     const JUB_BTC_TRANS_TYPE& type,
                                     const JUB_UINT16 inputCount,
                                     const std::vector<JUB_UINT64>& vInputAmount,
                                     const std::vector<std::string>& vInputPath,
                                     const std::vector<uchar_vector>& vRedeemScriptTlv,
                                     const std::vector<JUB_UINT16>& vChangeIndex,
                                     const std::vector<std::string>& vChangePath,
                                     const std::vector<uchar_vector>& vChangeRedeemScriptTlv,
                                     const std::vector<JUB_BYTE>& vUnsigedTrans,
                                     std::vector<uchar_vector>& vSignatureRaw) override;

    //ETH functions
    virtual JUB_RV SelectAppletETH() override;
    virtual JUB_RV GetAppletVersionETH(stVersion& version) override;
    virtual JUB_RV GetAddressETH(const std::string& path, const JUB_UINT16 tag, std::string& address) override;
    virtual JUB_RV GetHDNodeETH(const JUB_BYTE format, const std::string& path, std::string& pubkey) override;
    virtual JUB_RV SignTXETH(const bool bERC20,
                             const std::vector<JUB_BYTE>& vNonce,
                             const std::vector<JUB_BYTE>& vGasPrice,
                             const std::vector<JUB_BYTE>& vGasLimit,
                             const std::vector<JUB_BYTE>& vTo,
                             const std::vector<JUB_BYTE>& vValue,
                             const std::vector<JUB_BYTE>& vData,
                             const std::vector<JUB_BYTE>& vPath,
                             const std::vector<JUB_BYTE>& vChainID,
                             std::vector<JUB_BYTE>& vRaw) override;
    virtual JUB_RV _SignTXETH(const bool bERC20,
                              const std::vector<JUB_BYTE>& vNonce,
                              const std::vector<JUB_BYTE>& vGasPrice,
                              const std::vector<JUB_BYTE>& vGasLimit,
                              const std::vector<JUB_BYTE>& vTo,
                              const std::vector<JUB_BYTE>& vValue,
                              const std::vector<JUB_BYTE>& vData,
                              const std::vector<JUB_BYTE>& vPath,
                              const std::vector<JUB_BYTE>& vChainID,
                              std::vector<JUB_BYTE>& vRaw);
    virtual JUB_RV _SignTXUpgradeETH(const bool bERC20,
                                     const std::vector<JUB_BYTE>& vNonce,
                                     const std::vector<JUB_BYTE>& vGasPrice,
                                     const std::vector<JUB_BYTE>& vGasLimit,
                                     const std::vector<JUB_BYTE>& vTo,
                                     const std::vector<JUB_BYTE>& vValue,
                                     const std::vector<JUB_BYTE>& vData,
                                     const std::vector<JUB_BYTE>& vPath,
                                     const std::vector<JUB_BYTE>& vChainID,
                                     std::vector<JUB_BYTE>& vRaw);

    virtual JUB_RV SetERC20ETHToken(const std::string& tokenName,
                                    const JUB_UINT16 unitDP,
                                    const std::string& contractAddress) override;
    virtual JUB_RV SetERC20ETHTokens(const ERC20_TOKEN_INFO tokens[],
                                     const JUB_UINT16 iCount) override;

    virtual JUB_RV SignContractETH(const JUB_BYTE inputType,
                                   const std::vector<JUB_BYTE>& vNonce,
                                   const std::vector<JUB_BYTE>& vGasPrice,
                                   const std::vector<JUB_BYTE>& vGasLimit,
                                   const std::vector<JUB_BYTE>& vTo,
                                   const std::vector<JUB_BYTE>& vValue,
                                   const std::vector<JUB_BYTE>& vInput,
                                   const std::vector<JUB_BYTE>& vPath,
                                   const std::vector<JUB_BYTE>& vChainID,
                                   std::vector<JUB_BYTE>& signatureRaw) override;

    virtual JUB_RV SignContractHashETH(const JUB_BYTE inputType,
                                       const std::vector<JUB_BYTE>& vGasLimit,
                                       const std::vector<JUB_BYTE>& vInputHash,
                                       const std::vector<JUB_BYTE>& vUnsignedTxHash,
                                       const std::vector<JUB_BYTE>& vPath,
                                       const std::vector<JUB_BYTE>& vChainID,
                                       std::vector<JUB_BYTE>& signatureRaw) override;

    //EOS functions
    virtual JUB_RV SelectAppletEOS() override;
    virtual JUB_RV SetCoinTypeEOS() override;
    virtual JUB_RV GetAddressEOS(const TW::EOS::Type& type, const std::string& path, const JUB_UINT16 tag, std::string& address) override;
    virtual JUB_RV GetHDNodeEOS(const JUB_BYTE format, const std::string& path, std::string& pubkey) override;
    virtual JUB_RV SignTXEOS(const TW::EOS::Type& type,
                             const std::vector<JUB_BYTE>& vPath,
                             const std::vector<JUB_BYTE>& vChainId,
                             const std::vector<JUB_BYTE>& vRaw,
                             std::vector<uchar_vector>& vSignatureRaw,
                             const bool bWithType=false) override;

    //XRP functions
    virtual JUB_RV SelectAppletXRP() override;
    virtual JUB_RV SetCoinTypeXRP() override;
    virtual JUB_RV GetAddressXRP(const std::string& path, const JUB_UINT16 tag, std::string& address) override;
    virtual JUB_RV GetHDNodeXRP(const JUB_BYTE format, const std::string& path, std::string& pubkey) override;
    virtual JUB_RV SignTXXRP(const std::vector<JUB_BYTE>& vPath,
                             std::vector<JUB_BYTE>& vUnsignedRaw,
                             std::vector<uchar_vector>& vSignatureRaw) override;

    //TRX functions
    virtual JUB_RV SelectAppletTRX() override;
    virtual JUB_RV SetCoinTypeTRX() override;
    virtual JUB_RV GetAddressTRX(const std::string& path, const JUB_UINT16 tag, std::string& address) override;
    virtual JUB_RV GetHDNodeTRX(const JUB_BYTE format, const std::string& path, std::string& pubkey) override;
    virtual JUB_RV SignTXTRX(const std::vector<JUB_BYTE>& vPath,
                             const std::vector<JUB_BYTE>& vRaw,
                             std::vector<uchar_vector>& vSignatureRaw) override;
    virtual JUB_RV SetTRC20Token(const std::string& tokenName,
                                 const JUB_UINT16 unitDP,
                                 const std::string& contractAddress) override;

    //FIL functions
    virtual JUB_RV SelectAppletFIL() override;
    virtual JUB_RV SetCoinTypeFIL() override;
    virtual JUB_RV GetAppletVersionFIL(std::string& version) override;
    virtual JUB_RV GetAddressFIL(const std::string& path, const JUB_UINT16 tag, std::string& address) override;
    virtual JUB_RV GetHDNodeFIL(const JUB_BYTE format, const std::string& path, std::string& pubkey) override;
    virtual JUB_RV SignTXFIL(const uint64_t& nonce,
                             const uint256_t& glimit,
                             const uint256_t& gfeeCap,
                             const uint256_t& gpremium,
                             const std::string& to,
                             const uint256_t& value,
                             const std::string& input,
                             const std::string& path,
                             std::vector<uchar_vector>& vSignatureRaw) override;

    //common token functions
    virtual JUB_RV QueryBattery(JUB_BYTE &percent) override;
    virtual JUB_RV ShowVirtualPwd() override;
    virtual JUB_RV CancelVirtualPwd() override;
    virtual bool   IsInitialize() override;
    virtual bool   IsBootLoader() override;
    virtual JUB_RV GetSN(JUB_BYTE sn[24]) override;
    virtual JUB_RV GetLabel(JUB_BYTE label[32]) override;
    virtual JUB_RV GetPinRetry(JUB_BYTE& retry) override;
    virtual JUB_RV GetPinMaxRetry(JUB_BYTE& maxRetry) override;
    virtual JUB_RV GetBleVersion(JUB_BYTE bleVersion[4]) override;
    virtual JUB_RV GetFwVersion(JUB_BYTE fwVersion[4]) override;

    virtual JUB_RV EnumApplet(std::string& appletList) override;
    virtual JUB_RV GetAppletVersion(const std::string& appID, stVersion& version) override;
    virtual JUB_RV EnumSupportCoins(std::string& coinList) override;
    virtual JUB_RV GetDeviceCert(std::string& cert) override;
    virtual JUB_RV SendOneApdu(const std::string& apdu, std::string& response) override;

    virtual JUB_RV VerifyPIN(const std::string &pinMix, OUT JUB_ULONG &retry) override;

    virtual JUB_RV SetTimeout(const JUB_UINT16 timeout) override;

    // BIO
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

    // get function
    std::string getPath() {
        return _path;
    }

protected:
    JUB_RV _SelectApp(const JUB_BYTE PKIAID[],
                      JUB_BYTE length);

    JUB_RV _SetCoinType(const JUB_BYTE& type);

    JUB_RV _TranPack(const abcd::DataSlice &apduData,
                     const JUB_BYTE highMark,
                     const JUB_BYTE sigType,
                     const JUB_ULONG ulSendOnceLen,
                     int finalData = false,
                     int bOnce = false);
    JUB_RV _TranPackApdu(const JUB_ULONG ncla,
                         const JUB_ULONG nins,
                         const abcd::DataSlice &apduData,
                         const JUB_BYTE highMark,
                         const JUB_BYTE sigType,
                         const JUB_ULONG ulSendOnceLen,
                         JUB_BYTE *retData = nullptr, JUB_ULONG *pulRetDataLen = nullptr,
                         int finalData = false,
                         int bOnce = false);

    JUB_RV _SendApdu(const APDU *apdu, JUB_UINT16 &wRet,
                     JUB_BYTE *retData = nullptr, JUB_ULONG *pulRetDataLen = nullptr,
                     JUB_ULONG ulMiliSecondTimeout = 1200000);

    JUB_BYTE _HighMark(const JUB_ULONG& highMark) {
        return ((highMark&0x0F) << 4);
    }

protected:
    //BTC functions
    bool _isSupportLegacyAddress();
    JUB_BYTE _RealAddressFormat(const JUB_ULONG& addrFmt);
    JUB_BYTE _RealAddressFormat(const JUB_ENUM_BTC_ADDRESS_FORMAT& addrFmt);

    std::shared_ptr<ApduBuilder> _apduBuiler;
    std::shared_ptr<DeviceType> _device;
    std::string _path;
    stVersionExp _appletVersion;

    // ERC20 token extension apdu
    const std::string ETH_APPLET_VERSION_SUPPORT_EXT_TOKEN = std::string("01040109");
    const std::string ETH_APPLET_VERSION_SUPPORT_EXT_TOKENS= std::string("01080000");
}; // class JubiterBLDImpl end

}  // namespace jub end

#endif  // __HardwareTokenImpl__
