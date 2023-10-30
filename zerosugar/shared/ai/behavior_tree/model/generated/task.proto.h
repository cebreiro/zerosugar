#pragma once
#include <cstdint>
#include <memory>
#include <vector>
#include "zerosugar/shared/ai/behavior_tree/model/model.h"

namespace zerosugar::bt::model
{
    class Sequence : public Model
    {
    public:
        static constexpr const char* class_name = "sequence";

        bool Deserialize(const pugi::xml_node& node) override;

    protected:
        std::vector<std::unique_ptr<Model>> _children;

        static ModelFactory::Registry<Sequence> _registry;
    };

    class Selector : public Model
    {
    public:
        static constexpr const char* class_name = "selector";

        bool Deserialize(const pugi::xml_node& node) override;

    protected:
        std::vector<std::unique_ptr<Model>> _children;

        static ModelFactory::Registry<Selector> _registry;
    };

    class ForceSuccess : public Model
    {
    public:
        static constexpr const char* class_name = "force_success";

        bool Deserialize(const pugi::xml_node& node) override;

    protected:
        std::unique_ptr<Model> _child = {};

        static ModelFactory::Registry<ForceSuccess> _registry;
    };

    class ForceFailure : public Model
    {
    public:
        static constexpr const char* class_name = "force_failure";

        bool Deserialize(const pugi::xml_node& node) override;

    protected:
        std::unique_ptr<Model> _child = {};

        static ModelFactory::Registry<ForceFailure> _registry;
    };

    class Inverter : public Model
    {
    public:
        static constexpr const char* class_name = "inverter";

        bool Deserialize(const pugi::xml_node& node) override;

    protected:
        std::unique_ptr<Model> _child = {};

        static ModelFactory::Registry<Inverter> _registry;
    };

    class Repeat : public Model
    {
    public:
        static constexpr const char* class_name = "repeat";

        bool Deserialize(const pugi::xml_node& node) override;

    protected:
        std::unique_ptr<Model> _child = {};
        int32_t _count = {};

        static ModelFactory::Registry<Repeat> _registry;
    };

    class RetryUntilSuccess : public Model
    {
    public:
        static constexpr const char* class_name = "retry_until_success";

        bool Deserialize(const pugi::xml_node& node) override;

    protected:
        std::unique_ptr<Model> _child = {};
        int32_t _count = {};

        static ModelFactory::Registry<RetryUntilSuccess> _registry;
    };

}
