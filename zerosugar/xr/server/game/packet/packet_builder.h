#pragma once

namespace zerosugar::xr::network::game
{
    struct PlayerBase;
    struct PlayerEquipment;
}

namespace zerosugar::xr::network::game::sc
{
    struct EnterGame;
}

namespace zerosugar::xr
{
    class GameEntity;
    class GameInstance;
    class GameSpatialSector;
}

namespace zerosugar::xr
{
    class GamePacketBuilder
    {
    public:
        GamePacketBuilder() = delete;

        static void Build(network::game::sc::EnterGame& result,
            const GameInstance& gameInstance, const GameEntity& entity, const GameSpatialSector& sector);

    private:
        static void Build(network::game::PlayerBase& base, const GameEntity& entity);
        static void Build(network::game::PlayerEquipment& equipment, const GameEntity& entity);
    };
}
