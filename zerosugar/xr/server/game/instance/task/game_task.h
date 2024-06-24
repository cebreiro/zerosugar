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
        GameTaskBase(int64_t senderId, std::chrono::system_clock::time_point creationTimePoint,
            std::optional<std::chrono::system_clock::time_point> executionStartExpectedTimePoint);

        virtual ~GameTaskBase();

        virtual void SelectTargetId(const GameTaskTargetInformationProvider& provider) = 0;
        void Start(GameInstance& gameInstance, Entity& mainTarget, std::span<PtrNotNull<Entity>> targets) const; // NVI, set base time point

        auto GetSenderId() const -> int64_t;
        auto GetCreationTimePoint() const -> std::chrono::system_clock::time_point;
        auto GetMainTargetId() const -> std::optional<int64_t>;
        auto GetSubTargetIds() const -> const boost::container::small_vector<int64_t, 8>&;

    private:
        virtual void Execute(GameInstance& gameInstance, Entity& mainTarget, std::span<PtrNotNull<Entity>> targets) const = 0;

    private:
        int64_t _senderId = 0;
        std::chrono::system_clock::time_point _creationTimePoint;
        std::chrono::system_clock::time_point _executionStartExpectedTimePoint;
        std::chrono::system_clock::time_point _baseTimePoint;

    protected:
        std::optional<int64_t> _mainTargetId = std::nullopt;
        boost::container::small_vector<int64_t, 8> _subTargetIds;
    };
}
