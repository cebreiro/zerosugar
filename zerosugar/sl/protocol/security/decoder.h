#pragma once
#include "zerosugar/shared/network/buffer/buffer.h"

namespace zerosugar::sl
{
	class Decoder
	{
	public:
		virtual ~Decoder() = default;

		virtual void Decode(Buffer::iterator begin, Buffer::iterator end) = 0;
	};
}
