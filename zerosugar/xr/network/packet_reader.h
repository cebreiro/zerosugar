#pragma once

namespace zerosugar::xr
{
    class PacketReader
    {
    public:
        PacketReader() = delete;
        PacketReader(Buffer::const_iterator begin, Buffer::const_iterator end);

        template <typename T> requires std::integral<T> || std::floating_point<T>
        auto Read() -> T;

        template <typename T> requires std::is_enum_v<T>
        auto Read() -> T;

        auto ReadString() -> std::string;
        void ReadBytes(std::span<char> buffer, int64_t size);

        auto GetReadSize() const -> int64_t;

    private:
        BufferReader _bufferReader;
    };

    template <typename T> requires std::integral<T> || std::floating_point<T>
    auto PacketReader::Read() -> T
    {
        return _bufferReader.Read<T>();
    }

    template <typename T> requires std::is_enum_v<T>
    auto PacketReader::Read() -> T
    {
        return static_cast<T>(Read<std::underlying_type_t<T>>());
    }
}
