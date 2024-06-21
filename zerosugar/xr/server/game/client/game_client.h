#pragma once
#include "zerosugar/xr/network/packet_builder.h"
#include "zerosugar/xr/network/packet_interface.h"

namespace zerosugar
{
    class Session;
}

namespace zerosugar::xr
{
    class GameInstance;

    class GameClient final
    {
    public:
        GameClient(WeakPtrNotNull<Session> session, std::string authenticationToken, int64_t accountId, int64_t characterId,
            WeakPtrNotNull<GameInstance> gameInstance);
        ~GameClient();

        template <typename T> requires std::derived_from<T, IPacket>
        void Send(const T& item);

        void SetSession(WeakPtrNotNull<Session> session);

        auto GetAuthenticationToken() const -> const std::string&;
        auto GetAccountId() const -> int64_t;
        auto GetCharacterId() const -> int64_t;
        auto GetGameInstance() -> SharedPtrNotNull<GameInstance>;
        auto GetGameInstance() const -> SharedPtrNotNull<GameInstance>;

    private:
        void Send(Buffer buffer);

    private:
        WeakPtrNotNull<Session> _session;

        std::string _authenticationToken;
        int64_t _accountId = 0;
        int64_t _characterId = 0;
        WeakPtrNotNull<GameInstance> _gameInstance;
    };

    template <typename T> requires std::derived_from<T, IPacket>
    void GameClient::Send(const T& item)
    {
        this->Send(PacketBuilder::MakePacket(item));
    }
}
