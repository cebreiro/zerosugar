#pragma once

namespace zerosugar::xr
{
    class PacketWriter
    {
    public:
        template <typename T> requires std::integral<T> || std::floating_point<T>
        void Write(T value);

        template <typename T> requires std::is_enum_v<T>
        void Write(T value);

        void Write(std::string_view str);
        void WriteBytes(std::span<const char> buffer);

        auto GetBuffer() const -> std::span<const char>;
        auto GetWriteSize() const -> int64_t;

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

    template <typename T> requires std::is_enum_v<T>
    void PacketWriter::Write(T value)
    {
        using underlying_type = std::underlying_type_t<T>;

        Write<underlying_type>(static_cast<underlying_type>(value));
    }
}
