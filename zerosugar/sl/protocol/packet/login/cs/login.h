#pragma once
#include "zerosugar/sl/protocol/packet/login/login_packet_concept.h"

namespace zerosugar::sl::login::cs
{
    class Login
    {
    public:
        static constexpr int64_t account_max_size = 14;
        static constexpr int64_t password_max_size = 16;

    public:
        Login() = default;
        Login(const std::array<char, account_max_size>& account,
            const std::array<char, password_max_size>& password);

        auto Serialize() const -> Buffer;
        auto Deserialize(const Buffer& buffer) -> LoginPacketDeserializeResult;

        auto GetOpcode() const -> int8_t;

        auto GetAccount() const -> const std::string&;
        auto GetPassword() const -> const std::string&;

    private:
        static auto ToString(std::span<const char> characters) -> std::string;

    private:
        std::string _account;
        std::string _password;
    };
}
