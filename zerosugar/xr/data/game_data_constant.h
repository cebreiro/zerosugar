#pragma once

namespace zerosugar::xr
{
    struct PlayerAttack
    {
        double duration = 0.0;
        std::vector<double> attackEffectDelay;
    };

    class DataConstant
    {
        DataConstant();

    public:
        auto GetPlayerAttacks() const -> const std::array<PlayerAttack, 4>&;

        static auto GetInstance() -> const DataConstant&;

    private:
        std::array<PlayerAttack, 4> _playerAttacks;
    };
}
