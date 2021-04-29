#include <token/JubiterBLDImpl.h>

namespace jub {

#define SWITCH_TO_HC_APP                        \
do {				                            \
    JUB_VERIFY_RV(_SelectApp(kPKIAID_HC, sizeof(kPKIAID_HC)/sizeof(JUB_BYTE)));\
} while (0)                                     \

JUB_RV JubiterBLDImpl::selectApplet_HC() {
    SWITCH_TO_HC_APP;
    return JUBR_OK;
}

} // namespace jub end
