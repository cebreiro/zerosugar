#include "zone_packet_decoder.h"

#include "zerosugar/sl/protocol/security/secret_key.h"

namespace zerosugar::sl
{
	void ZonePacketDecoder::Decode(Buffer::iterator begin, Buffer::iterator end)
	{
		int32_t i = 0;
		char prev = _lastValue;

		for (auto iter = begin; iter != end; ++iter, ++i)
		{
			*iter = (*iter ^ static_cast<char>(PacketSecretKey::GATE_ZONE_KEY[(i + _sn) & 0xFF])) - prev;
			prev = *iter;
		}

		_sn += i;
		_lastValue = prev;
	}
}
