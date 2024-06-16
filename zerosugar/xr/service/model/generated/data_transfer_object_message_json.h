#pragma once
#include <cstdint>
#include <string>
#include <optional>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>
#include "zerosugar/xr/service/model/generated/data_transfer_object_message.h"

namespace zerosugar::xr::service
{
    void from_json(const nlohmann::json& j, DTOAccount& item);
    void to_json(nlohmann::json& j, const DTOAccount& item);

    void from_json(const nlohmann::json& j, DTOItemOption& item);
    void to_json(nlohmann::json& j, const DTOItemOption& item);

    void from_json(const nlohmann::json& j, DTOItem& item);
    void to_json(nlohmann::json& j, const DTOItem& item);

    void from_json(const nlohmann::json& j, DTOEquipItem& item);
    void to_json(nlohmann::json& j, const DTOEquipItem& item);

    void from_json(const nlohmann::json& j, DTOCharacterAdd& item);
    void to_json(nlohmann::json& j, const DTOCharacterAdd& item);

    void from_json(const nlohmann::json& j, DTOLobbyItem& item);
    void to_json(nlohmann::json& j, const DTOLobbyItem& item);

    void from_json(const nlohmann::json& j, DTOLobbyCharacter& item);
    void to_json(nlohmann::json& j, const DTOLobbyCharacter& item);

}
