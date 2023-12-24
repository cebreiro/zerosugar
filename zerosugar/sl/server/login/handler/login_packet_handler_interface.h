#pragma once
#include <boost/container/flat_set.hpp>
#include "zerosugar/sl/protocol/packet/login/login_packet_concept.h"
#include "zerosugar/sl/server/login/login_client_state.h"
#include "zerosugar/sl/service/generated/login_service_generated_interface.h"

namespace zerosugar
{
    class Buffer;
}

namespace zerosugar::sl
{
    class LoginServer;
    class LoginClient;

    class ILoginPacketHandler
    {
    public:
        using Locator = ServiceLocatorRef<ILogService, service::ILoginService>;

    public:
        virtual ~ILoginPacketHandler() = default;

        virtual bool CanHandle(const LoginClient& client) const = 0;
        virtual auto Handle(const LoginServer& server, LoginClient& client, Buffer& buffer) const -> Future<LoginPacketDeserializeResult> = 0;
        virtual auto GetOpcode() const -> int8_t = 0;
        virtual auto GetName() const -> std::string_view = 0;
    };
}

namespace zerosugar::sl::detail
{
    class LoginPacketHandlerAbstract : public ILoginPacketHandler
    {
    public:
        bool CanHandle(const LoginClient& client) const final;

    protected:
        void AddAllowedState(LoginClientState state);
        void AddAllowedStateRange(std::initializer_list<LoginClientState> states);

    private:
        boost::container::flat_set<LoginClientState> _allowedStates;
    };

    template <login_packet_concept T>
    class LoginPacketHandlerT : public LoginPacketHandlerAbstract
    {
    public:
        LoginPacketHandlerT() = default;

        auto GetOpcode() const->int8_t final;
        auto GetName() const->std::string_view final;

    private:
        auto Handle(const LoginServer& server, LoginClient& client, Buffer& buffer) const->Future<LoginPacketDeserializeResult> final;

    private:
        virtual auto HandlePacket(const LoginServer& server, LoginClient& client, const T& packet) const->Future<void> = 0;

    private:
        boost::container::flat_set<LoginClientState> _allowedStates;
    };

    template <login_packet_concept T>
    auto LoginPacketHandlerT<T>::Handle(const LoginServer& server, LoginClient& client, Buffer& buffer) const
        -> Future<LoginPacketDeserializeResult>
    {
        T t = {};

        const LoginPacketDeserializeResult result = t.Deserialize(buffer);
        if (result.errorCode == LoginPacketDeserializeResult::ErrorCode::None)
        {
            co_await this->HandlePacket(server, client, t);
        }

        co_return result;
    }

    template <login_packet_concept T>
    auto LoginPacketHandlerT<T>::GetOpcode() const -> int8_t
    {
        return T{}.GetOpcode();
    }

    template <login_packet_concept T>
    auto LoginPacketHandlerT<T>::GetName() const -> std::string_view
    {
        return typeid(*this).name();
    }
}
