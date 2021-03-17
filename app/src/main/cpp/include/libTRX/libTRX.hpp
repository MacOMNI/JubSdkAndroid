#pragma once

#ifndef __libXRP__
#define __libXRP__

#include "JUB_SDK_XRP.h"
#include "context/ContextXRP.h"

#include "mSIGNA/stdutils/uchar_vector.h"
#include <google/protobuf/any.pb.h>

namespace jub {

namespace trx {

JUB_RV SerializeContract(const JUB_CONTRACT_TRX& contract,
                         ::google::protobuf::Any& parameter);

JUB_RV PackTransactionRaw(const JUB_TX_TRX& tx,
                          std::string& packedContractInPB);

JUB_RV SerializePreimage(const std::string& packedContractInPb,
                                 uchar_vector& preimageRaw);
} // namespace trx end

} // namespace jub end

#endif
