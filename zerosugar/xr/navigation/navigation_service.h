#pragma once
#include "zerosugar/xr/navigation/navi_data.h"
#include "zerosugar/xr/navigation/navi_query.h"
#include "zerosugar/xr/navigation/navi_visualize_param.h"

namespace zerosugar::xr::navi
{
    class FVector;
    class Visualizer;
}

namespace zerosugar::xr
{
    class NavigationService final : public std::enable_shared_from_this<NavigationService>
    {
    public:
        NavigationService(const ServiceLocator& serviceLocator, SharedPtrNotNull<Strand> strand, navi::Data naviData);
        ~NavigationService();

        bool IsRunningVisualizer() const;

        auto StartVisualize(std::function<void()> shutdownCallback) -> Future<bool>;
        void StopVisualize();

        auto Join() -> Future<void>;

        void AddDrawTargets(std::vector<navi::AddVisualizeTargetParam> params);
        void AddDrawTarget(navi::AddVisualizeTargetParam param);
        void RemoveDrawTarget(navi::RemoveVisualizeTargetParam param);

        void UpdateDrawTarget(navi::UpdateVisualizeTargetParam param);

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
