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
        auto Find(game_entity_id_type id) const -> SharedPtrNotNull<GameEntity>;

        template <typename T> requires std::same_as<std::ranges::range_value_t<T>, game_entity_id_type>
        void FindRange(T range, const std::function<void(const SharedPtrNotNull<GameEntity>&)>& callback) const;

    private:
        boost::unordered::concurrent_flat_map<
            game_entity_id_type, SharedPtrNotNull<GameEntity>> _concurrentFlatMap;

        static constexpr size_t bulk_size = decltype(_concurrentFlatMap)::bulk_visit_size;
    };

    template <typename T> requires std::same_as<std::ranges::range_value_t<T>, game_entity_id_type>
    void GameEntityContainer::FindRange(T range, const std::function<void(const SharedPtrNotNull<GameEntity>&)>& callback) const
    {
        assert(callback);

        std::array<game_entity_id_type, bulk_size> buffer = {};
        const auto function = [&callback](const auto& pair)
            {
                assert(pair.second);

                callback(pair.second);
            };

        int64_t i = 0;

        auto iter = range.begin();
        for (; iter != range.end(); ++iter)
        {
            buffer[i] = *iter;
            ++i;

            if (i == bulk_size)
            {
                _concurrentFlatMap.cvisit(buffer.begin(), buffer.end(), function);

                i = 0;
            }
        }

        _concurrentFlatMap.cvisit(buffer.begin(), buffer.begin() + i, function);
    }
}
