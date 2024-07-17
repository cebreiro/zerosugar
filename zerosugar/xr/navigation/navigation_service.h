#pragma once
#include "zerosugar/xr/navigation/navi_data.h"
#include "zerosugar/xr/navigation/navi_query.h"

namespace zerosugar::collision
{
    class OBB3d;
}

namespace zerosugar::xr::navi
{
    class FVector;
    class Visualizer;
    class IVisualizer;
}

namespace zerosugar::xr
{
    class NavigationService final : public std::enable_shared_from_this<NavigationService>
    {
    public:
        NavigationService(const ServiceLocator& serviceLocator, SharedPtrNotNull<Strand> strand, navi::Data naviData);
        ~NavigationService();

        auto ShutdownAndJoin() -> Future<void>;

        bool IsRunningVisualizer() const;

        auto StartVisualize(std::function<void()> shutdownCallback) -> Future<bool>;
        void StopVisualize();

        auto GetStrand() -> Strand&;
        auto GetVisualizer() -> std::shared_ptr<navi::IVisualizer>;

    public:
        auto GetRandomPointAroundCircle(const navi::FVector& position, float radius)
            -> Future<std::optional<navi::FVector>>;

        auto FindStraightPath(const navi::FVector& start, const navi::FVector& end)
            -> Future<boost::container::static_vector<navi::FVector, navi::constant::max_straight_path_count>>;

    private:
        static auto GetPolyFindingExtents() -> navi::Extents;

    private:
        ServiceLocator _serviceLocator;
        SharedPtrNotNull<Strand> _strand;
        navi::Data _naviData;

        std::atomic<bool> _runningVisualizer = false;
        std::unique_ptr<navi::Visualizer> _visualizer;
        std::function<void()> _shutdownCallback;
        Future<void> _visualizerRunFuture;
    };
}
