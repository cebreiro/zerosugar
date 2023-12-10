#pragma once
#include <cstdint>
#include <array>

namespace zerosugar::sl
{
	struct PacketSecretKey
	{
		static const uint32_t LOGIN_KEY_FIRST;
		static const uint32_t LOGIN_KEY_SECOND;
		static const std::array<uint8_t, 256> GATE_ZONE_KEY;
	};
}
