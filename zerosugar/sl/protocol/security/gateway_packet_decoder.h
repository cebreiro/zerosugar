#pragma once
#include "zerosugar/sl/protocol/security/decoder.h"

namespace zerosugar::sl
{
	class GatewayPacketDecoder : public Decoder
	{
	public:
		void Decode(Buffer::iterator begin, Buffer::iterator end) override;

	private:
		int32_t _sn = 0;
	};
}
