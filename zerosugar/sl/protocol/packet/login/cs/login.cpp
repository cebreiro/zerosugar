#include "login.h"

#include <boost/algorithm/string.hpp>
#include "zerosugar/sl/protocol/packet/login/cs/opcode.h"

namespace zerosugar::sl::login::cs
{
    Login::Login(const std::array<char, account_max_size>& account,
        const std::array<char, password_max_size>& password)
        : _account(ToString(account))
        , _password(ToString(password))
    {
    }

    auto Login::Serialize() const -> Buffer
    {
        std::array<char, account_max_size> account = {};
        std::copy_n(_account.begin(), std::ssize(_account), account.begin());

        std::array<char, password_max_size> password = {};
        std::copy_n(_password.begin(), std::ssize(_password), password.begin());

        Buffer buffer;
        buffer.Add(buffer::Fragment::Create(account_max_size + password_max_size));

        BufferWriter writer(buffer);
        writer.WriteBuffer(account);
        writer.WriteBuffer(password);

        return buffer;
    }

    auto Login::Deserialize(const Buffer& buffer) -> LoginPacketDeserializeResult
    {
        if (buffer.GetSize() < account_max_size + password_max_size)
        {
            return LoginPacketDeserializeResult{
                .errorCode = LoginPacketHandlerErrorCode::Fail_ShortLength
            };
        }

        BufferReader reader(buffer.cbegin(), buffer.cend());
        _account = reader.ReadString(account_max_size);
        _password = reader.ReadString(password_max_size);

        constexpr auto space = [](char c) { return c == '\0'; };
        boost::algorithm::trim_right_if(_account, space);
        boost::algorithm::trim_right_if(_password, space);

        return LoginPacketDeserializeResult{
            .errorCode = LoginPacketHandlerErrorCode::None,
            .readSize = reader.GetReadSize(),
        };
    }

    auto Login::GetOpcode() const -> int8_t
    {
        return static_cast<int8_t>(Opcode::LoginRequest);
    }

    auto Login::GetAccount() const -> const std::string&
    {
        return _account;
    }

    auto Login::GetPassword() const -> const std::string&
    {
        return _password;
    }

    auto Login::ToString(std::span<const char> characters) -> std::string
    {
        std::string result;
        result.reserve(std::ssize(characters));

        for (int64_t i = 0; i < std::ssize(characters); ++i)
        {
            const char c = characters[i];
            if (c == 0)
            {
                break;
            }

            result.push_back(c);
        }

        return result;
    }
}
