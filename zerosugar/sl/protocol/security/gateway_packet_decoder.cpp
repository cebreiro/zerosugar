#include "gateway_packet_decoder.h"

#include "zerosugar/sl/protocol/security/secret_key.h"

namespace zerosugar::sl
{
	void GatewayPacketDecoder::Decode(Buffer::iterator begin, Buffer::iterator end)
	{
		int32_t i = 0;
		char prev = 0;

		for (auto iter = begin; iter != end; ++iter, ++i)
		{
			*iter = (*iter ^ static_cast<char>(PacketSecretKey::GATE_ZONE_KEY[(i + _sn) & 0xFF])) - prev;
			prev = *iter;
		}

		_sn += i;
	}
}
