#include "login_packet_encoder.h"

namespace zerosugar::sl
{
	LoginPacketEncoder::LoginPacketEncoder(uint32_t key1, uint32_t key2)
		: _keys({ key1, key2 })
	{
	}

	void LoginPacketEncoder::Encode(Buffer::iterator begin, Buffer::iterator end)
	{
		if (_keys[0] | _keys[1])
		{
			const char* key = reinterpret_cast<const char*>(_keys.data());

			uint32_t i = 0;
			auto iter = begin;

			*iter = (*iter ^ key[i]);
			char CL = *iter;

			++iter;
			++i;

			for (; iter != end; ++iter, ++i)
			{
				char& c = *iter;

				char BL = CL ^ (c ^ key[i & 7]);
				c = BL;
				CL = c;
			}

			UpdateKey(i);
		}
	}

	void LoginPacketEncoder::UpdateKey(uint32_t encodedSize)
	{
		_keys[0] += encodedSize;

		if (_keys[0] >= 0x80000000)
		{
			_keys[1] = 0xFFFFFFFF;
		}
	}
}
