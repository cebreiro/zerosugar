#pragma once
#include <boost/unordered/concurrent_flat_map.hpp>
#include "zerosugar/xr/server/game/instance/entity/game_entity_id.h"

namespace zerosugar::xr
{
    class GameEntity;

    class GameEntityContainer
    {
    public:
        bool Add(SharedPtrNotNull<GameEntity> entity);
        bool Remove(game_entity_id_type id);

        auto Find(game_entity_id_type id) -> SharedPtrNotNull<GameEntity>;

        template <typename T> requires std::same_as<std::ranges::range_value_t<T>, game_entity_id_type>
        void FindRange(T range, const std::function<void(SharedPtrNotNull<GameEntity>)>& callback);

    private:
        boost::unordered::concurrent_flat_map<
            game_entity_id_type, SharedPtrNotNull<GameEntity>> _concurrentFlatMap;

        static constexpr size_t bulk_size = decltype(_concurrentFlatMap)::bulk_visit_size;
    };

    template <typename T> requires std::same_as<std::ranges::range_value_t<T>, game_entity_id_type>
    void GameEntityContainer::FindRange(T range, const std::function<void(SharedPtrNotNull<GameEntity>)>& callback)
    {
        assert(callback);

        std::array<game_entity_id_type, bulk_size> buffer = {};
        const auto function = [&callback](SharedPtrNotNull<GameEntity> ptr)
            {
                assert(ptr);

                callback(std::move(ptr));
            };

        int64_t i = 0;

        auto iter = range.begin();
        for (; iter != range.end(); ++iter)
        {
            buffer[i] = *iter;
            ++i;

            if (i == bulk_size)
            {
                _concurrentFlatMap.visit(buffer.begin(), buffer.end(), function);
            }
        }

        _concurrentFlatMap.visit(buffer.begin(), buffer.begin() + i, function);
    }
}
