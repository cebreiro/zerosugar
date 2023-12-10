#pragma once
#include <array>
#include "zerosugar/shared/network/buffer/buffer.h"
#include "zerosugar/sl/protocol/security/decoder.h"

namespace zerosugar::sl
{
	class LoginPacketDecoder : public Decoder
	{
	public:
		LoginPacketDecoder(uint32_t key1, uint32_t key2);

		void Decode(Buffer::iterator begin, Buffer::iterator end) override;

	private:
		void UpdateKey(uint32_t decodedSize);

	private:
		std::array<uint32_t, 2> _keys = {};
	};
}
