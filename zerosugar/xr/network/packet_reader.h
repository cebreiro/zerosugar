#pragma once

namespace zerosugar::xr
{
    class PacketReader
    {
    public:
        PacketReader() = delete;
        explicit PacketReader(BufferReader& bufferReader);

        template <typename T> requires std::integral<T> || std::floating_point<T>
        void Read() -> T;

        void ReadString() -> std::string;
        void ReadBytes(std::span<char> buffer, int64_t size);

    private:
        BufferReader& _bufferReader;
    };

    template <typename T> requires std::integral<T> || std::floating_point<T>
    auto PacketReader::Read() -> T
    {
        return _bufferReader.Read<T>();
    }
}
