#pragma once
#include <array>
#include "zerosugar/sl/protocol/security/encoder.h"

namespace zerosugar::sl
{
	class LoginPacketEncoder : public Encoder
	{
	public:
		LoginPacketEncoder(uint32_t key1, uint32_t key2);

		void Encode(Buffer::iterator begin, Buffer::iterator end) override;

	private:
		void UpdateKey(uint32_t encodedSize);

	private:
		std::array<uint32_t, 2> _keys;
	};
}
