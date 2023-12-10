#pragma once
#include "zerosugar/shared/network/buffer/buffer.h"

namespace zerosugar::sl
{
	class Encoder
	{
	public:
		virtual ~Encoder() = default;

		virtual void Encode(Buffer::iterator begin, Buffer::iterator end) = 0;
	};
}
