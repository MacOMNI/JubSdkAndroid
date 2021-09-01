#include "token/JubiterBLDImpl.h"
#include <Filecoin/Transaction.h>

namespace jub {


#define SWITCH_TO_FIL_APP                       \
do {                                            \
    JUB_VERIFY_RV(_SelectApp(kPKIAID_ETH, sizeof(kPKIAID_ETH)/sizeof(JUB_BYTE)));\
} while (0);                                    \


JUB_RV JubiterBLDImpl::SelectAppletFIL() {

    SWITCH_TO_FIL_APP;
    return JUBR_OK;
}

JUB_RV JubiterBLDImpl::SetCoinTypeFIL() {

    return _SetCoinType((JUB_BYTE)JUB_ENUM_COINTYPE_ETH::COINFIL);
}

JUB_RV JubiterBLDImpl::GetAppletVersionFIL(stVersion &version) {

    uchar_vector appID(kPKIAID_ETH, 16);
    JUB_VERIFY_RV(GetAppletVersion(CharPtr2HexStr(appID), version));

    return JUBR_OK;
}


JUB_RV JubiterBLDImpl::GetAddressFIL(const std::string& path, const JUB_UINT16 tag, std::string& address) {

    uchar_vector data(path.begin(), path.end());

    APDU apdu(0x00, 0xf6, 0x00, (JUB_BYTE)tag, (JUB_ULONG)data.size(), data.data(), 0x14);
    JUB_UINT16 ret = 0;
    JUB_BYTE retData[2048] = {0,};
    JUB_ULONG ulRetDataLen = sizeof(retData)/sizeof(JUB_BYTE);
    JUB_VERIFY_RV(_SendApdu(&apdu, ret, retData, &ulRetDataLen));
    if (0x9000 != ret) {
        return JUBR_TRANSMIT_DEVICE_ERROR;
    }

    uchar_vector vAddress(retData, (unsigned int)ulRetDataLen);
    address = std::string(vAddress.begin(), vAddress.end());

    return JUBR_OK;
}


JUB_RV JubiterBLDImpl::GetHDNodeFIL(const JUB_BYTE format, const std::string& path, std::string& pubkey) {

    //path = "m/44'/60'/0'";
    uchar_vector vPath;
    vPath << path;
    uchar_vector apduData = ToTlv(0x08, vPath);

    APDU apdu(0x00, 0xe6, 0x00, format, (JUB_ULONG)apduData.size(), apduData.data());
    JUB_UINT16 ret = 0;
    JUB_BYTE retData[2048] = {0,};
    JUB_ULONG ulRetDataLen = sizeof(retData)/sizeof(JUB_BYTE);
    JUB_VERIFY_RV(_SendApdu(&apdu, ret, retData, &ulRetDataLen));
    if (0x9000 != ret) {
        return JUBR_TRANSMIT_DEVICE_ERROR;
    }

    if (JUB_ENUM_PUB_FORMAT::HEX == format) {
        uchar_vector vPubkey(retData, (unsigned int)ulRetDataLen);
        pubkey = vPubkey.getHex();
    }
    else if (JUB_ENUM_PUB_FORMAT::XPUB == format) {
        pubkey = (JUB_CHAR_PTR)retData;
    }

    return JUBR_OK;
}

JUB_RV JubiterBLDImpl::SignTXFIL(const uint64_t& nonce,
                                 const uint256_t& glimit,
                                 const uint256_t& gfeeCap,
                                 const uint256_t& gpremium,
                                 const std::string& to,
                                 const uint256_t& value,
                                 const std::string& input,
                                 const std::string& path,
                                 std::vector<uchar_vector>& vSignatureRaw) {

    try {
        uchar_vector data;

        // cidPrefix
        data << ToTlv(0x4A, TW::Filecoin::Transaction::getCidPrefix());
        // version
        data << ToTlv(0x4B, TW::Data({0x00}));
        // to address
        data << ToTlv(0x44, TW::Filecoin::Address(to).bytes);
        // from address
        std::vector<JUB_BYTE> vPath(path.begin(), path.end());
        data << ToTlv(0x47, vPath);
        // nonce
        data << ToTlv(0x41, TW::encodeBENoZero(nonce));
        // value
        auto loadingZero = [](const uint256_t& x) -> TW::Data {
            auto bytes = TW::encodeBENoZero(x);
            bytes.insert(bytes.begin(), 0);
            return bytes;
        };
        auto aa = loadingZero(value);
        data << ToTlv(0x45, loadingZero(value));
        // gas limit
        data << ToTlv(0x43, loadingZero(glimit));
        // gas fee cap
        data << ToTlv(0x4E, loadingZero(gfeeCap));
        // gas premium
        data << ToTlv(0x4F, loadingZero(gpremium));
        // abi.MethodNum (0 => send)
        data << ToTlv(0x4C, TW::Data({0x00}));
        // data (empty)
        data << ToTlv(0x4D, TW::Data());

        JUB_BYTE ins = 0x2A;
//        if (bERC20) {
//            ins = JUB_ENUM_APDU_CMD::INS_SIGN_ERC20_C8;
//        }

        //one pack can do it
        APDU apdu(0x00, ins, 0x01, 0x00, (JUB_ULONG)data.size(), data.data());
        JUB_UINT16 ret = 0;
        JUB_BYTE retData[2048] = { 0, };
        JUB_ULONG ulRetDataLen = sizeof(retData) / sizeof(JUB_BYTE);
        JUB_VERIFY_RV(_SendApdu(&apdu, ret, retData, &ulRetDataLen));
        if (0x9000 != ret) {
            return JUBR_TRANSMIT_DEVICE_ERROR;
        }

        uchar_vector signatureRaw(retData, retData + ulRetDataLen);
        vSignatureRaw.push_back(signatureRaw);
    }
    catch (...) {
        return JUBR_ERROR_ARGS;
    }

    return JUBR_OK;
}


} // namespace jub end
