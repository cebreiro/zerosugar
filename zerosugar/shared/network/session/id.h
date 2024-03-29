#pragma once
#include <cstdint>
#include "zerosugar/shared/type/value_type.h"

namespace zerosugar::session
{
    using id_type = ValueType<uint64_t, class SessionIdTag>;
}
