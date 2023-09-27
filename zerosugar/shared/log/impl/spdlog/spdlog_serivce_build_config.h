#pragma once
#include <cstdint>
#include <string>
#include <map>
#include <filesystem>
#include "zerosugar/shared/log/log_level.h"

namespace zerosugar
{
    namespace detail
    {
        template <typename T>
        class SpdLogServiceBaseBuildConfig
        {
            friend class SpdLogServiceBuilder;

        public:
            auto SetLogLevel(LogLevel logLevel) -> T&;
            auto SetShouldLogAsync(bool shouldLogAsync) -> T&;
            auto SetPattern(std::string pattern) -> T&;

            auto GetLogLevel() const noexcept -> LogLevel;
            bool GetShouldLogAsync() const noexcept;
            auto GetPattern() const noexcept -> const std::string&;

        private:
            LogLevel _logLevel = LogLevel::Debug;
            bool _shouldLogAsync = false;
            std::string _pattern = "[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%s:%#] %v";
        };

        template <typename T>
        T& SpdLogServiceBaseBuildConfig<T>::SetLogLevel(LogLevel logLevel)
        {
            _logLevel = logLevel;

            return static_cast<T&>(*this);
        }

        template <typename T>
        T& SpdLogServiceBaseBuildConfig<T>::SetShouldLogAsync(bool shouldLogAsync)
        {
            _shouldLogAsync = shouldLogAsync;

            return static_cast<T&>(*this);
        }

        template <typename T>
        T& SpdLogServiceBaseBuildConfig<T>::SetPattern(std::string pattern)
        {
            _pattern = std::move(pattern);

            return static_cast<T&>(*this);
        }

        template <typename T>
        auto SpdLogServiceBaseBuildConfig<T>::GetLogLevel() const noexcept -> LogLevel
        {
            return _logLevel;
        }

        template <typename T>
        bool SpdLogServiceBaseBuildConfig<T>::GetShouldLogAsync() const noexcept
        {
            return _shouldLogAsync;
        }

        template <typename T>
        auto SpdLogServiceBaseBuildConfig<T>::GetPattern() const noexcept -> const std::string&
        {
            return _pattern;
        }
    }

    class SpdLogConsoleLogConfig : public detail::SpdLogServiceBaseBuildConfig<SpdLogConsoleLogConfig>
    {
        friend class SpdLogServiceBuilder;

    public:
        SpdLogConsoleLogConfig& SetColor(LogLevel logLevel, uint16_t color);

        auto GetColors() const noexcept -> const std::map<LogLevel, uint16_t>&;

    private:
        std::map<LogLevel, uint16_t> _colors;
    };

    class SpdLogDailyFileLogConfig : public detail::SpdLogServiceBaseBuildConfig<SpdLogDailyFileLogConfig>
    {
        friend class SpdLogServiceBuilder;

    public:
        SpdLogDailyFileLogConfig& SetPath(std::filesystem::path path);

    private:
        std::filesystem::path _path = std::filesystem::current_path() / "log";
    };
}
