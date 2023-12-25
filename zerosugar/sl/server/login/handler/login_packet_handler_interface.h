#pragma once
#include "zerosugar/sl/protocol/packet/login/login_packet_concept.h"
#include "zerosugar/sl/server/login/login_client_state.h"

namespace zerosugar
{
    class Buffer;
}

namespace zerosugar::sl
{
    class LoginServer;
    class LoginClient;

    ENUM_CLASS(LoginPacketHandleResult, int32_t,
        (Success, 0)
        (Fail_Close, 1)
        (Fail_Continue, 2)
    )

    class ILoginPacketHandler
    {
    public:
        virtual ~ILoginPacketHandler() = default;

        virtual bool CanHandle(const LoginClient& client) const = 0;
        virtual auto Handle(LoginClient& client, Buffer& buffer) const -> Future<LoginPacketHandleResult> = 0;

        virtual auto GetOpcode() const -> int8_t = 0;
        virtual auto GetName() const -> std::string_view = 0;
    };
}

namespace zerosugar::sl::detail
{
    class LoginPacketHandlerAbstract : public ILoginPacketHandler
    {
    public:
        LoginPacketHandlerAbstract() = default;
        explicit LoginPacketHandlerAbstract(WeakPtrNotNull<LoginServer> server);

        bool CanHandle(const LoginClient& client) const final;

    protected:
        void AddAllowedState(LoginClientState state);
        void AddAllowedStateRange(std::initializer_list<LoginClientState> states);

    private:
        std::set<LoginClientState> _allowedStates;

    protected:
        WeakPtrNotNull<LoginServer> _server;
    };

    template <login_packet_concept T>
    class LoginPacketHandlerT : public LoginPacketHandlerAbstract
    {
    public:
        explicit LoginPacketHandlerT(WeakPtrNotNull<LoginServer> server);

        auto GetOpcode() const -> int8_t final;
        auto GetName() const -> std::string_view final;

    private:
        auto Handle(LoginClient& client, Buffer& buffer) const->Future<LoginPacketHandleResult> final;

    private:
        virtual auto HandlePacket(LoginServer& server, LoginClient& client, const T& packet) const -> Future<void> = 0;
    };

    template <login_packet_concept T>
    auto LoginPacketHandlerT<T>::Handle(LoginClient& client, Buffer& buffer) const
        -> Future<LoginPacketHandleResult>
    {
        const std::shared_ptr<LoginServer> loginServer = _server.lock();
        if (!loginServer)
        {
            co_return LoginPacketHandleResult::Fail_Close;
        }

        T packet = {};
        const LoginPacketDeserializeResult result = packet.Deserialize(buffer);

        switch (result.errorCode)
        {
        case LoginPacketDeserializeErrorCode::None:
        {
            Buffer read;
            (void)buffer.SliceFront(read, result.readSize);
            assert(read.GetSize() == result.readSize);

            co_await this->HandlePacket(*loginServer, client, packet);
            co_return LoginPacketHandleResult::Success;
        }
        break;
        case LoginPacketDeserializeErrorCode::Fail_ShortLength:
            co_return LoginPacketHandleResult::Fail_Continue;
        case LoginPacketDeserializeErrorCode::Fail_InvalidFormat:
        default:;
        }

        co_return LoginPacketHandleResult::Fail_Close;
    }

    template <login_packet_concept T>
    LoginPacketHandlerT<T>::LoginPacketHandlerT(WeakPtrNotNull<LoginServer> server)
        : LoginPacketHandlerAbstract(std::move(server))
    {
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
