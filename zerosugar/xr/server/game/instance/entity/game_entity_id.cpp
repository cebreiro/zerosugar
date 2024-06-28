#include "game_entity_id.h"

#include <bit>
#include <boost/container_hash/hash.hpp>

namespace zerosugar::xr::detail::game
{
    auto hash_value(GameEntityId item) -> size_t
    {
        boost::hash<int64_t> hasher;
        return hasher(item.Unwrap());
    }

    GameEntityId::GameEntityId(GameEntityType type, int32_t value)
        : _type(type)
        , _value(value)
    {
    }

    auto GameEntityId::GetType() const -> GameEntityType
    {
        return _type;
    }

    auto GameEntityId::GetValue() const -> int32_t
    {
        return _value;
    }

    auto GameEntityId::Unwrap() const -> int64_t
    {
        return std::bit_cast<int64_t>(*this);
    }

    auto GameEntityId::Null() -> GameEntityId
    {
        GameEntityId result;

        *reinterpret_cast<int64_t*>(&result) = int64_t{ -1 };
        static_assert(sizeof(int64_t) == sizeof(result));

        return result;
    }

    bool operator<(GameEntityId lhs, GameEntityId rhs)
    {
        return lhs.Unwrap() < rhs.Unwrap();
    }

    bool operator==(GameEntityId lhs, GameEntityId rhs)
    {
        return lhs.Unwrap() == rhs.Unwrap();
    }

    bool operator!=(GameEntityId lhs, GameEntityId rhs)
    {
        return lhs.Unwrap() != rhs.Unwrap();
    }
}
