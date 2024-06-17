#pragma once
#include <cstdint>
#include <string>
#include <optional>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>
#include "zerosugar/xr/network/model/generated/lobby_cs_message.h"
#include "zerosugar/xr/network/model/generated/lobby_message.h"
#include "zerosugar/xr/network/model/generated/lobby_message_json.h"

namespace zerosugar::xr::network::lobby::cs
{
    void from_json(const nlohmann::json& j, Authenticate& item);
    void to_json(nlohmann::json& j, const Authenticate& item);

    void from_json(const nlohmann::json& j, CreateCharacter& item);
    void to_json(nlohmann::json& j, const CreateCharacter& item);

    void from_json(const nlohmann::json& j, DeleteCharacter& item);
    void to_json(nlohmann::json& j, const DeleteCharacter& item);

    void from_json(const nlohmann::json& j, SelectCharacter& item);
    void to_json(nlohmann::json& j, const SelectCharacter& item);

}
