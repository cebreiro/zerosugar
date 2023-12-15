#pragma once

namespace zerosugar::sl
{
    class LogConfig
    {
    private:
        friend void from_json(const nlohmann::json& j, LogConfig& config);

    public:
        class Console
        {
        private:
            friend void from_json(const nlohmann::json& j, Console& config);

        public:
            bool ShouldAsync() const;
            auto GetLevel() const->int32_t;

        private:
            bool _async = false;
            int32_t _level = 1;
        };

        class DailyFile
        {
        private:
            friend void from_json(const nlohmann::json& j, DailyFile& config);

        public:
            auto GetFilePath() const -> const std::filesystem::path&;

        private:
            std::filesystem::path _filePath;
        };

    public:
        LogConfig() = default;

        auto GetConsole() const -> const Console&;
        auto GetDailyFile() const -> const DailyFile&;

    private:
        Console _console;
        DailyFile _dailyFile;
    };
}
