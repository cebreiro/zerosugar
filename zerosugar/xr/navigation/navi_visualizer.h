#pragma once
#include <boost/container/flat_map.hpp>
#include <boost/unordered/unordered_flat_map.hpp>
#include "zerosugar/xr/navigation/navi_vector.h"
#include "zerosugar/xr/navigation/navi_visualizer_interface.h"
#include "zerosugar/xr/navigation/navi_visualize_param.h"

class InputGeom;
class SampleDebugDraw;
class NavMeshTesterTool;

namespace zerosugar::collision
{
	class OBB3d;
}

namespace zerosugar::xr::navi
{
    class Data;
}

namespace zerosugar::xr::navi
{
    class Visualizer final : public RecastDemoVisualizerInterface, public IVisualizer
    {
    public:
        Visualizer(Strand& strand, Data& data);
        ~Visualizer();

        auto Run(Promise<std::pair<bool, std::string>>& initPromise) -> Future<void>;

        void Shutdown();

        auto AddAgent(int64_t id, vis::Agent agent) -> Future<bool> override;
        auto RemoveAgent(int64_t id) -> Future<bool> override;

        auto UpdateAgentPosition(int64_t id, Eigen::Vector3d position) -> Future<bool> override;
        auto UpdateAgentPositionAndYaw(int64_t id, Eigen::Vector3d position, float yaw) -> Future<bool> override;
        auto UpdateAgentMovement(int64_t id, Eigen::Vector3d startPos, vis::Agent::Movement movement) -> Future<bool> override;

        auto Draw(vis::Object object, std::chrono::milliseconds milli) -> Future<void> override;
		
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
        void RenderThreadMain(Promise<std::pair<bool, std::string>>& initPromise);

        void Tick();

        void DrawAgentCylinder(const Eigen::Vector3d& pos, double radius, vis::DrawColor drawColor);
        void DrawAgentYawArrow(const Eigen::Vector3d& pos, double radius, double yaw, vis::DrawColor drawColor);

        void DrawCylinder(const Vector& min, const Vector& max, vis::DrawColor drawColor);
        void DrawOBB(const vis::OBB& obb, vis::DrawColor drawColor);
        void DrawCircle(const vis::Circle& circle, vis::DrawColor drawColor, float lineWidth = 1.f);
        void DrawArrow(const vis::Arrow& arrow, vis::DrawColor drawColor, float as0 = 0.f, float as1 = 0.4f, float lineWidth = 3.f);
        void DrawLines(const vis::Lines& lines, vis::DrawColor drawColor);

    private:
        Strand& _strand;
        Data& _data;
        std::atomic<bool> _shutdown = false;

        int32_t _drawMode = 0;
        std::thread _renderThread;
        std::unique_ptr<InputGeom> _geom;
        std::unique_ptr<SampleDebugDraw> _dd;
        std::unique_ptr<NavMeshTesterTool> _testTool;

        boost::unordered::unordered_flat_map<int64_t, vis::Agent> _agents;
        boost::container::flat_multimap<std::chrono::system_clock::time_point, vis::Object> _objects;
    };
}
