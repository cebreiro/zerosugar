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

        Promise<std::pair<bool, std::string>> initPromise;
        Future<std::pair<bool, std::string>> future = initPromise.GetFuture();

        _shutdownCallback = shutdownCallback;
        _visualizer = std::make_unique<navi::Visualizer>(*_strand, _naviData);
        _visualizerRunFuture = _visualizer->Run(initPromise).Then(*_strand, [self = shared_from_this()]()
            {
                self->_visualizer.reset();
                self->_runningVisualizer.store(false);

                if (std::function<void()> callback = std::exchange(self->_shutdownCallback, {}))
                {
                    callback();
                }
            });

        const auto [initResult, message] = co_await future;
        if (!initResult)
        {
            ZEROSUGAR_LOG_ERROR(_serviceLocator,
                fmt::format("[{}] fail to start visuializer. error: {}",
                    GetName(), message));
        }

        co_return initResult;
    }

    void NavigationService::StopVisualize()
    {
        Dispatch(*_strand, [self = shared_from_this()]()
            {
                self->_visualizer->Shutdown();
            });
    }

    auto NavigationService::GetName() const -> std::string_view
    {
        return "navigation_service";
    }

    auto NavigationService::GetStrand() -> Strand&
    {
        return *_strand;
    }

    auto NavigationService::ShutdownAndJoin() -> Future<void>
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

    auto NavigationService::GetVisualizer() -> std::shared_ptr<navi::IVisualizer>
    {
        return std::shared_ptr<navi::IVisualizer>(shared_from_this(), _visualizer.get());
    }

    auto NavigationService::GetRandomPoints(int64_t count) -> Future<std::vector<navi::FVector>>
    {
        [[maybe_unused]]
        auto self = shared_from_this();

        co_await *_strand;

        const int64_t length = std::max<int64_t>(1, count);

        std::vector<navi::FVector> result;
        result.reserve(length);

        for (int64_t i = 0; i < length; ++i)
        {
            const dtQueryFilter queryFilter;
            const auto [polyRef, pos] = navi::GetRandomPoint(_naviData.GetQuery(), queryFilter);

            if (!polyRef)
            {
                continue;
            }

            result.emplace_back(pos);
        }

        co_return result;
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
