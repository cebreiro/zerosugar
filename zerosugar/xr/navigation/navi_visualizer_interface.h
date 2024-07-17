#pragma once
#include "zerosugar/xr/navigation/navi_visualize_param.h"

class dtNavMeshQuery;
class dtNavMesh;
struct duDebugDraw;

namespace zerosugar::xr::navi
{
    class RecastDemoVisualizerInterface
    {
    public:
        virtual ~RecastDemoVisualizerInterface() = default;

    public:
        // RecastDemo API
        virtual void handleTools() = 0;
        virtual void handleDebugMode() = 0;
        virtual void handleClick(const float* s, const float* p, bool shift) = 0;
        virtual void handleToggle() = 0;
        virtual void handleStep() = 0;
        virtual void handleRender() = 0;
        virtual void handleRenderOverlay(double* proj, double* model, int* view) = 0;
        virtual void handleUpdate(const float dt) = 0;

        virtual auto GetNavMesh() -> dtNavMesh& = 0;
        virtual auto GetNavMeshQuery() -> dtNavMeshQuery& = 0;
        virtual auto GetObjectFilePath() const -> const std::string& = 0;

        virtual auto GetDebugDraw() -> duDebugDraw& = 0;
        virtual auto GetAgentRadius() const -> float = 0;
        virtual auto GetAgentHeight() const -> float = 0;
        virtual auto GetAgentClimb() const -> float = 0;
    };

    class IVisualizer
    {
    public:
        virtual ~IVisualizer() = default;

        virtual auto AddAgent(int64_t id, vis::Agent agent) -> Future<bool> = 0;
        virtual auto RemoveAgent(int64_t id) -> Future<bool> = 0;

        virtual auto UpdateAgentPosition(int64_t id, Eigen::Vector3d position) -> Future<bool> = 0;
        virtual auto UpdateAgentPositionAndYaw(int64_t id, Eigen::Vector3d position, float yaw) -> Future<bool> = 0;
        virtual auto UpdateAgentMovement(int64_t id, Eigen::Vector3d startPos, vis::Agent::Movement movement) -> Future<bool> = 0;

        virtual auto Draw(vis::Object object, std::chrono::milliseconds milli) -> Future<void> = 0;
    };
}
