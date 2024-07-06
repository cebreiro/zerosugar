#pragma once
#include "zerosugar/xr/navigation/navi_data.h"
#include "zerosugar/xr/navigation/navi_query.h"

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

        auto StartVisualize() -> Future<bool>;
        void StopVisualize();

        auto Join() -> Future<void>;

        auto GetRandomPointAroundCircle(const navi::FVector& position, float radius)
            -> Future<std::optional<navi::FVector>>;

        auto FindStraightPath(const navi::FVector& start, const navi::FVector& end)
            -> Future<boost::container::static_vector<navi::FVector, navi::constant::max_straight_path_count>>;

        void DrawCircle(const navi::FVector& vector, float radius);

    private:
        static auto GetPolyFindingExtents() -> navi::Extents;

    private:
        ServiceLocator _serviceLocator;
        SharedPtrNotNull<Strand> _strand;
        navi::Data _naviData;

        std::unique_ptr<navi::Visualizer> _visualizer;
        Future<void> _visualizerRunFuture;
    };
}
