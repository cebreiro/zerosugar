#include "navigation_service.h"

#include <recastnavigation/DetourNavMeshQuery.h>
#include "zerosugar/shared/collision/shape/obb.h"
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

    bool NavigationService::IsRunningVisualizer() const
    {
        return _runningVisualizer.load();
    }

    auto NavigationService::StartVisualize(std::function<void()> shutdownCallback) -> Future<bool>
    {
        _runningVisualizer.store(true);

        co_await *_strand;

        if (_visualizer)
        {
            co_return false;
        }

        _shutdownCallback = shutdownCallback;
        _visualizer = std::make_unique<navi::Visualizer>(*_strand, _naviData);
        _visualizerRunFuture = _visualizer->Run().Then(*_strand, [self = shared_from_this()]()
            {
                self->_visualizer.reset();
                self->_runningVisualizer.store(false);

                if (std::function<void()> callback = std::exchange(self->_shutdownCallback, {}))
                {
                    callback();
                }
            });
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

            _visualizerRunFuture = Future<void>();
        }

        co_return;
    }

    void NavigationService::AddDrawTargets(std::vector<navi::AddVisualizeTargetParam> params)
    {
        Dispatch(*_strand, [self = shared_from_this(), params = std::move(params)]()
            {
                if (!self->_visualizer)
                {
                    return;
                }

                for (const navi::AddVisualizeTargetParam& param : params)
                {
                    self->_visualizer->AddDrawTarget(param);
                }
            });
    }

    void NavigationService::AddDrawTarget(navi::AddVisualizeTargetParam param)
    {
        Dispatch(*_strand, [self = shared_from_this(), param = std::move(param)]()
            {
                if (!self->_visualizer)
                {
                    return;
                }

                self->_visualizer->AddDrawTarget(param);
            });
    }

    void NavigationService::RemoveDrawTarget(navi::RemoveVisualizeTargetParam param)
    {
        Dispatch(*_strand, [self = shared_from_this(), param = std::move(param)]()
            {
                if (!self->_visualizer)
                {
                    return;
                }

                self->_visualizer->RemoveDrawTarget(param);
            });
    }

    void NavigationService::UpdateDrawTarget(navi::UpdateVisualizeTargetParam param)
    {
        Dispatch(*_strand, [self = shared_from_this(), param = std::move(param)]()
            {
                if (!self->_visualizer)
                {
                    return;
                }

                self->_visualizer->UpdateDrawTarget(param);
            });
    }

    void NavigationService::DrawBox(const navi::FVector& min, const navi::FVector& max, std::chrono::milliseconds milli)
    {
        Dispatch(*_strand, [self = shared_from_this(), min, max, milli]()
            {
                if (!self->_visualizer)
                {
                    return;
                }

                self->_visualizer->DrawBox(min, max, milli);
            });
    }

    void NavigationService::DrawOBB(const collision::OBB3d& obb, std::chrono::milliseconds milli)
    {
        Dispatch(*_strand, [self = shared_from_this(), obb = obb, milli]()
            {
                if (!self->_visualizer)
                {
                    return;
                }

                self->_visualizer->DrawOBB(obb, milli);
            });
    }

    auto NavigationService::GetRandomPointAroundCircle(const navi::FVector& position, float radius)
        -> Future<std::optional<navi::FVector>>
    {
        [[maybe_unused]]
        auto self = shared_from_this();

        const navi::Vector pos(position);
        const navi::Extents ex = GetPolyFindingExtents();
        const navi::Scalar r(radius);

        co_await *_strand;

        const dtQueryFilter queryFilter;
        const auto [polyRef, vector] = navi::GetRandomPointAroundCircle(_naviData.GetQuery(), pos, ex, r, queryFilter);

        if (!polyRef)
        {
            co_return std::nullopt;
        }

        co_return navi::FVector(vector);
    }

    auto NavigationService::FindStraightPath(const navi::FVector& start, const navi::FVector& end)
        ->Future<boost::container::static_vector<navi::FVector, navi::constant::max_straight_path_count>>
    {
        [[maybe_unused]]
        auto self = shared_from_this();

        const navi::Vector startPos(start);
        const navi::Vector endPos(end);
        const navi::Extents ex = GetPolyFindingExtents();

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

    auto NavigationService::GetPolyFindingExtents() -> navi::Extents
    {
        return navi::Extents(400.f, 200.f, 400.f);
    }
}
