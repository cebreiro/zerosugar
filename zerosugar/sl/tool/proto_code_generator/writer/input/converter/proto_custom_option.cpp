#include "proto_custom_option.h"

namespace zerosugar::sl
{
    const ProtoCustomOption ProtoCustomOption::_instance;

    ProtoCustomOption::ProtoCustomOption()
        : _opcodeId(60000, 0)
        , _lengthId(70000, 0)
        , _sizeElementId(70001, "")
    {
    }

    auto ProtoCustomOption::OpcodeId() -> const message_opcode_option_id_type&
    {
        return _instance._opcodeId;
    }

    auto ProtoCustomOption::LengthId() -> const field_length_option_id_type&
    {
        return _instance._lengthId;
    }

    auto ProtoCustomOption::SizeElementId() -> const field_size_element_option_id_type&
    {
        return _instance._sizeElementId;
    }
}
