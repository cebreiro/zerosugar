#pragma once

namespace zerosugar::xr
{
    ENUM_CLASS(GameEntityType, int16_t,
        (Player)
        (Monster)
        (Spawner)

        (Count)
    )

    namespace detail::game
    {
        class GameEntityId
        {
        public:
            GameEntityId() = default;
            GameEntityId(GameEntityType type, int32_t value);

            auto GetType() const -> GameEntityType;
            auto GetValue() const -> int32_t;

            auto Unwrap() const -> int64_t;

            static auto FromInt64(int64_t value) -> GameEntityId;
            static auto Null() -> GameEntityId;

        public:
            friend bool operator<(GameEntityId lhs, GameEntityId rhs);
            friend bool operator==(GameEntityId lhs, GameEntityId rhs);
            friend bool operator!=(GameEntityId lhs, GameEntityId rhs);

            friend auto hash_value(GameEntityId item) -> size_t;

        private:
            GameEntityType _type = GameEntityType::Player;
            int16_t _reserved = 0;
            int32_t _value = 0;
        };
    }

    using game_entity_id_type = detail::game::GameEntityId;
}

namespace std
{
    template <>
    struct hash<zerosugar::xr::detail::game::GameEntityId>
    {
        size_t operator()(zerosugar::xr::detail::game::GameEntityId value) const noexcept {
            return std::hash<int64_t>{}(value.Unwrap());
        }
    };
}

namespace fmt
{
    template <>
    struct formatter<zerosugar::xr::detail::game::GameEntityId> : formatter<std::string>
    {
        auto format(zerosugar::xr::detail::game::GameEntityId value, format_context& context) const
            -> fmt::format_context::iterator
        {
            return fmt::formatter<std::string>::format(fmt::format("[{}, {}]",
                ToString(value.GetType()), value.GetValue()), context);
        }
    };
}
