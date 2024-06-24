#pragma once

namespace zerosugar::xr
{
    class GameTaskBase;

    class GameTaskExecutor
    {
    public:
        void Execute(GameTaskBase& task);

    private:
    };
}
