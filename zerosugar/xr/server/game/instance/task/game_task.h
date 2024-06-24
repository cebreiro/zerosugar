#pragma once
#include <boost/container/small_vector.hpp>

namespace zerosugar::xr
{
    class Entity;
    class GameInstance;
    class GameTaskTargetInformationProvider;

    class GameTaskBase
    {
    public:
        GameTaskBase() = delete;
        explicit GameTaskBase(std::chrono::system_clock::time_point creationTimePoint);

        virtual ~GameTaskBase();

        virtual void SelectTargetId(const GameTaskTargetInformationProvider& provider) = 0;

        void Start(GameInstance& gameInstance, Entity& mainTarget, std::span<PtrNotNull<Entity>> targets) const; // NVI, set base time point
        void Complete() const;

        auto GetCreationTimePoint() const -> std::chrono::system_clock::time_point;
        auto GetMainTargetId() const -> std::optional<int64_t>;
        auto GetSubTargetIds() const -> const boost::container::small_vector<int64_t, 8>&;

    private:
        virtual void Execute(GameInstance& gameInstance, Entity& mainTarget, std::span<PtrNotNull<Entity>> targets) const = 0;
        virtual void OnComplete() const = 0;

    private:
        std::chrono::system_clock::time_point _creationTimePoint;
        std::chrono::system_clock::time_point _baseTimePoint;

    protected:
        std::optional<int64_t> _mainTargetId = std::nullopt;
        boost::container::small_vector<int64_t, 8> _subTargetIds;
    };
}
