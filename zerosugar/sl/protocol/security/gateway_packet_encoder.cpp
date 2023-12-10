#include "gateway_packet_encoder.h"

#include "zerosugar/sl/protocol/security/secret_key.h"

namespace zerosugar::sl
{
	void GatewayPacketEncoder::Encode(Buffer::iterator begin, Buffer::iterator end)
	{
		auto iter = begin;

		char CL = 0;
		char BL = *iter;

		*iter = BL ^ static_cast<char>(PacketSecretKey::GATE_ZONE_KEY[0 + (_sn & 0xFF)]);

		int32_t i = 1;
		for (; iter != end; ++iter)
		{
			char& c = *iter;

			CL = c;
			CL = CL + BL;
			BL = c;
			c = CL ^ static_cast<char>(PacketSecretKey::GATE_ZONE_KEY[(i + _sn) & 0xFF]);
		}

		_sn += i & 0xFF;
	}
}
