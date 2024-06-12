#pragma once
#include <cstdint>
#include <string>
#include <optional>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>
#include "zerosugar/sl/service/generated/shared_generated.h"

namespace zerosugar::sl::service
{
    void from_json(const nlohmann::json& j, AuthToken& item);
    void to_json(nlohmann::json& j, const AuthToken& item);

    void from_json(const nlohmann::json& j, Account& item);
    void to_json(nlohmann::json& j, const Account& item);

    void from_json(const nlohmann::json& j, AccountUpdate& item);
    void to_json(nlohmann::json& j, const AccountUpdate& item);

    void from_json(const nlohmann::json& j, SlotPosition& item);
    void to_json(nlohmann::json& j, const SlotPosition& item);

    void from_json(const nlohmann::json& j, Item& item);
    void to_json(nlohmann::json& j, const Item& item);

    void from_json(const nlohmann::json& j, Skill& item);
    void to_json(nlohmann::json& j, const Skill& item);

    void from_json(const nlohmann::json& j, Job& item);
    void to_json(nlohmann::json& j, const Job& item);

    void from_json(const nlohmann::json& j, CharacterStat& item);
    void to_json(nlohmann::json& j, const CharacterStat& item);

    void from_json(const nlohmann::json& j, Character& item);
    void to_json(nlohmann::json& j, const Character& item);

}
