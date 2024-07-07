#pragma once
#include <boost/unordered/unordered_flat_map.hpp>
#include "zerosugar/xr/navigation/navi_vector.h"
#include "zerosugar/xr/navigation/navi_visualizer_interface.h"
#include "zerosugar/xr/navigation/navi_visualize_param.h"

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

        auto Run() -> Future<void>;

        void Shutdown();

        void AddDrawTarget(const AddVisualizeTargetParam& param);
        void RemoveDrawTarget(const RemoveVisualizeTargetParam& param);
        void UpdateDrawTarget(const UpdateVisualizeTargetParam& param);

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
        void RenderThreadMain();

    private:
        Strand& _strand;
        Data& _data;
        std::atomic<bool> _shutdown = false;

        std::thread _renderThread;
        std::unique_ptr<InputGeom> _geom;
        std::unique_ptr<SampleDebugDraw> _dd;
        std::unique_ptr<NavMeshTesterTool> _testTool;

        int32_t _drawMode = 0;

        struct DrawTarget
        {
            int64_t id = 0;
            Vector position;
            DrawColor drawColor = DrawColor::LightBlue;
            Scalar radius;

            std::optional<Vector> destPosition = std::nullopt;
            std::optional<DrawColor> destPositionDrawColor = std::nullopt;
        };
        boost::unordered::unordered_flat_map<int64_t, DrawTarget> _drawTargets;
    };
}
