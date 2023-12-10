#include "login_packet_decoder.h"

namespace zerosugar::sl
{
	LoginPacketDecoder::LoginPacketDecoder(uint32_t key1, uint32_t key2)
		: _keys({ key1, key2 })
	{
	}

	void LoginPacketDecoder::Decode(Buffer::iterator begin, Buffer::iterator end)
	{
		auto iter = begin;
		uint32_t i = 0;

		if (_keys[0] | _keys[1])
		{
			const char* key = reinterpret_cast<const char*>(_keys.data());

			char AL = *iter;
			*iter = AL ^ key[i];

			++iter;
			++i;

			for (; iter != end; ++iter, ++i)
			{
				char& c = *iter;

				char BL = c ^ (AL ^ key[i & 7]);
				AL = c;
				c = BL;
			}

			UpdateKey(i);
		}
	}

	void LoginPacketDecoder::UpdateKey(uint32_t decodedSize)
	{
		_keys[0] += decodedSize;

		if (_keys[0] >= 0x80000000)
		{
			_keys[1] = 0xFFFFFFFF;
		}
	}
}
