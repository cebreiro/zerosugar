#pragma once
#include "zerosugar/xr/navigation/navi_visualizer_interface.h"

class InputGeom;
class SampleDebugDraw;
class NavMeshTesterTool;

namespace zerosugar::xr::navi
{
    class Data;
}

namespace zerosugar::xr::navi
{
    class Visualizer final : public RecastDemoVisualizerInterface
    {
    public:
        Visualizer(Strand& strand, Data& data);
        ~Visualizer();

        auto Run() -> Future<bool>;

        void Shutdown();

    public:
        void handleTools() override;
        void handleDebugMode() override;
        void handleClick(const float* s, const float* p, bool shift) override;
        void handleToggle() override;
        void handleStep() override;
        void handleRender() override;
        void handleRenderOverlay(double* proj, double* model, int* view) override;
        void handleUpdate(const float dt) override;

        auto GetNavMesh() -> dtNavMesh& override;
        auto GetNavMeshQuery() -> dtNavMeshQuery& override;
        auto GetObjectFilePath() const -> const std::string& override;

        auto GetDebugDraw() -> duDebugDraw& override;
        auto GetAgentRadius() const -> float override;
        auto GetAgentHeight() const -> float override;
        auto GetAgentClimb() const -> float override;

    private:
        Strand& _strand;
        Data& _data;
        bool _shutdown = false;
        std::chrono::milliseconds _tickInterval = std::chrono::milliseconds(200);

        std::unique_ptr<InputGeom> _geom;
        std::unique_ptr<SampleDebugDraw> _dd;
        std::unique_ptr<NavMeshTesterTool> _testTool;

        int32_t _drawMode = 0;
    };
}
