#pragma once
#include "zerosugar/sl/protocol/security/encoder.h"

namespace zerosugar::sl
{
	class ZonePacketEncoder : public Encoder
	{
	public:
		void Encode(Buffer::iterator begin, Buffer::iterator end) override;

	private:
		int32_t _sn = 0;
		int8_t _lastValue = 0;
	};
}
