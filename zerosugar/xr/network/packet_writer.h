#pragma once

namespace zerosugar::xr
{
    class PacketWriter
    {
    public:
        template <typename T> requires std::integral<T> || std::floating_point<T>
        void Write(T value);

        void Write(std::string_view str);
        void WriteBytes(std::span<const char> buffer);

        auto GetWriteSize() const -> int64_t;

        auto MakeBuffer() const -> Buffer;

    private:
        boost::container::small_vector<char, 256> _buffer;
        int64_t _size = 0;
    };

    template <typename T> requires std::integral<T> || std::floating_point<T>
    void PacketWriter::Write(T value)
    {
        const char* ptr = reinterpret_cast<const char*>(&value);

        std::copy_n(ptr, sizeof(T), std::back_inserter(_buffer));
        _size += sizeof(T);
    }
}
