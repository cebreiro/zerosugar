#pragma once
#include <cstdint>
#include <string>
#include <optional>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>
#include "zerosugar/xr/network/model/generated/lobby_sc_message.h"
#include "zerosugar/xr/network/model/generated/lobby_message.h"
#include "zerosugar/xr/network/model/generated/lobby_message_json.h"

namespace zerosugar::xr::network::lobby::sc
{
    void from_json(const nlohmann::json& j, FailAuthenticate& item);
    void to_json(nlohmann::json& j, const FailAuthenticate& item);

    void from_json(const nlohmann::json& j, ResultCreateCharacter& item);
    void to_json(nlohmann::json& j, const ResultCreateCharacter& item);

    void from_json(const nlohmann::json& j, SuccessDeleteCharacter& item);
    void to_json(nlohmann::json& j, const SuccessDeleteCharacter& item);

    void from_json(const nlohmann::json& j, NotifyCharacterList& item);
    void to_json(nlohmann::json& j, const NotifyCharacterList& item);

    void from_json(const nlohmann::json& j, SuccessSelectCharacter& item);
    void to_json(nlohmann::json& j, const SuccessSelectCharacter& item);

}
