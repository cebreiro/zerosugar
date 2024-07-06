#include "navigation_service.h"

#include <recastnavigation/DetourNavMeshQuery.h>

#include "zerosugar/shared/execution/executor/strand.h"
#include "zerosugar/shared/execution/executor/executor_coroutine_traits.h"
#include "zerosugar/xr/navigation/navi_vector.h"
#include "zerosugar/xr/navigation/navi_visualizer.h"

namespace zerosugar::xr
{
    NavigationService::NavigationService(const ServiceLocator& serviceLocator, SharedPtrNotNull<Strand> strand, navi::Data naviData)
        : _serviceLocator(serviceLocator)
        , _strand(std::move(strand))
        , _naviData(std::move(naviData))
    {
    }

    NavigationService::~NavigationService()
    {
    }

    auto NavigationService::StartVisualize() -> Future<bool>
    {
        co_await *_strand;

        if (_visualizer)
        {
            co_return false;
        }

        _visualizer = std::make_unique<navi::Visualizer>(*_strand, _naviData);
        _visualizerRunFuture = _visualizer->Run();
    }

    void NavigationService::StopVisualize()
    {
        Dispatch(*_strand, [self = shared_from_this()]()
            {
                self->_visualizer->Shutdown();
            });
    }

    auto NavigationService::Join() -> Future<void>
    {
        co_await *_strand;

        if (_visualizer)
        {
            _visualizer->Shutdown();

            assert(_visualizerRunFuture.IsValid());
            co_await _visualizerRunFuture;

            _visualizer.reset();
            _visualizerRunFuture = Future<bool>();
        }

        co_return;
    }

    auto NavigationService::GetRandomPointAroundCircle(const navi::FVector& position, const navi::FVector& extents, float radius)
        -> Future<std::optional<navi::FVector>>
    {
        const navi::Vector pos(position);
        const navi::Vector ex(extents);

        co_await *_strand;

        const dtQueryFilter queryFilter;
        const auto [polyRef, vector] = navi::GetRandomPointAroundCircle(_naviData.GetQuery(), pos, ex, radius, queryFilter);

        if (!polyRef)
        {
            co_return std::nullopt;
        }

        co_return navi::FVector(vector);
    }

    auto NavigationService::FindStraightPath(const navi::FVector& start, const navi::FVector& end, const navi::FVector& extents)
        ->Future<boost::container::static_vector<navi::FVector, navi::constant::max_straight_path_count>>
    {
        const navi::Vector startPos(start);
        const navi::Vector endPos(end);
        const navi::Vector ex(extents);

        co_await *_strand;

        const dtQueryFilter queryFilter;
        auto naviResult = navi::FindStraightPath(_naviData.GetQuery(), startPos, endPos, ex, queryFilter);

        boost::container::static_vector<navi::FVector, navi::constant::max_straight_path_count> result;
        for (int64_t i = 0; i < std::ssize(naviResult); ++i)
        {
            result.emplace_back(naviResult[i]);
        }

        co_return result;
    }
}
