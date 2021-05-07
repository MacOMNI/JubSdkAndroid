#ifndef __JuBiterBLDBioImpl__
#define __JuBiterBLDBioImpl__


#include "JubiterBLDImpl.h"


namespace jub {


class JubiterBLDBioImpl
: public JubiterBLDImpl {
public:
    static stAppInfos g_appInfo[];

public:
    JubiterBLDBioImpl(std::string path);
    JubiterBLDBioImpl(DeviceType* device);
    ~JubiterBLDBioImpl();

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
}; // class JubiterBLDBioImpl end

}  // namespace jub end

#endif  // __JuBiterBLDBioImpl__
