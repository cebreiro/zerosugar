#pragma once
#include "zerosugar/sl/protocol/security/encoder.h"

namespace zerosugar::sl
{
	class GatewayPacketEncoder : public Encoder
	{
	public:
		void Encode(Buffer::iterator begin, Buffer::iterator end) override;

	private:
		int32_t _sn = 0;
	};
}
