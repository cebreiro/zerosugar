#include "zone_packet_encoder.h"

#include "zerosugar/sl/protocol/security/secret_key.h"

namespace zerosugar::sl
{
	void ZonePacketEncoder::Encode(Buffer::iterator begin, Buffer::iterator end)
	{
		int32_t i = 0;

		char temp1 = 0;
		char temp2 = _lastValue;
		char lastValue = 0;

		for (auto iter = begin; iter != end; ++iter)
		{
			char& c = *iter;

			temp1 = c;
			c = (temp2 + temp1) ^ static_cast<char>(PacketSecretKey::GATE_ZONE_KEY[(i + _sn) & 0xFF]);
			temp2 = temp1;

			lastValue = c;
			++i;
		}

		_sn += i & 0xFF;
		_lastValue = lastValue;
	}
}
