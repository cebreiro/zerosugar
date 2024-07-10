#pragma once

namespace zerosugar::xr::network::game
{
    struct Player;
    struct RemotePlayer;
    struct PlayerBase;
    struct PlayerEquipment;
    struct PlayerInventoryItem;
    struct Equipment;
    struct Position;

    struct Monster;
}

namespace zerosugar::xr::network::game::sc
{
    struct EnterGame;
    struct AddRemotePlayer;
    struct RemoveRemotePlayer;
    struct MoveRemotePlayer;

    struct MoveMonster;
}

namespace zerosugar::xr
{
    class GameEntity;
    class GamePlayerSnapshot;
    class GameInstance;
    class GameSpatialSector;

    struct InventoryItem;

    class GameMonsterSnapshot;
}

namespace zerosugar::xr
{
    class GamePacketBuilder
    {
    public:
        GamePacketBuilder() = delete;

        static void Build(network::game::sc::EnterGame& result,
            const GameInstance& gameInstance, const GameEntity& entity, const GameSpatialSector& sector);
        static void Build(network::game::sc::EnterGame& result, const GameEntity& entity, int32_t mapId);
        static void Build(network::game::sc::AddRemotePlayer& result, const GamePlayerSnapshot& player);
        static void Build(network::game::sc::RemoveRemotePlayer& result, const GamePlayerSnapshot& player);
        static void Build(network::game::sc::MoveRemotePlayer& result, const GamePlayerSnapshot& player);

        static void Build(network::game::PlayerInventoryItem& result, const InventoryItem& item);

    public:
        static void Build(network::game::Player& result, const GameEntity& entity);
        static void Build(network::game::RemotePlayer& result, const GamePlayerSnapshot& player);
        static void Build(network::game::PlayerBase& result, const GameEntity& entity);
        static void Build(network::game::PlayerBase& result, const GamePlayerSnapshot& player);
        static void Build(network::game::PlayerEquipment& result, const GameEntity& entity);
        static void Build(network::game::PlayerEquipment& result, const GamePlayerSnapshot& player);

        static void Build(network::game::Equipment& result, const InventoryItem& item, int32_t type);
        static void Build(network::game::Position& position, const GamePlayerSnapshot& player);

        static void Build(network::game::Monster& result, const GameMonsterSnapshot& snapshot);
    };
}
