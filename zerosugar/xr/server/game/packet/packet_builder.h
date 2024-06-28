#pragma once

namespace zerosugar::xr::network::game
{
    struct Player;
    struct RemotePlayer;
    struct PlayerBase;
    struct PlayerEquipment;
    struct Equipment;
    struct Position;
}

namespace zerosugar::xr::network::game::sc
{
    struct EnterGame;
    struct AddRemotePlayer;
    struct RemoveRemotePlayer;
    struct MoveRemotePlayer;
}

namespace zerosugar::xr
{
    class GameEntity;
    class GamePlayerViewModel;
    class GameInstance;
    class GameSpatialSector;

    struct InventoryItem;
}

namespace zerosugar::xr
{
    class GamePacketBuilder
    {
    public:
        GamePacketBuilder() = delete;

        static void Build(network::game::sc::EnterGame& result,
            const GameInstance& gameInstance, const GameEntity& entity, const GameSpatialSector& sector);
        static void Build(network::game::sc::AddRemotePlayer& result, const GamePlayerViewModel& playerView);
        static void Build(network::game::sc::RemoveRemotePlayer& result, const GamePlayerViewModel& playerView);
        static void Build(network::game::sc::MoveRemotePlayer& result, const GamePlayerViewModel& playerView);

    private:
        static void Build(network::game::Player& result, const GameEntity& entity);
        static void Build(network::game::RemotePlayer& result, const GamePlayerViewModel& playerView);
        static void Build(network::game::PlayerBase& result, const GameEntity& entity);
        static void Build(network::game::PlayerBase& result, const GamePlayerViewModel& playerView);
        static void Build(network::game::PlayerEquipment& result, const GameEntity& entity);
        static void Build(network::game::PlayerEquipment& result, const GamePlayerViewModel& playerView);

        static void Build(network::game::Equipment& result, const InventoryItem& item, int32_t type);
        static void Build(network::game::Position& position, const GamePlayerViewModel& playerView);
    };
}
