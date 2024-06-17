#pragma once
#include <cstdint>
#include <string>
#include <optional>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>
#include "zerosugar/xr/service/model/generated/database_service_message.h"
#include "zerosugar/xr/service/model/generated/data_transfer_object_message.h"
#include "zerosugar/xr/service/model/generated/data_transfer_object_message_json.h"

namespace zerosugar::xr::service
{
    void from_json(const nlohmann::json& j, AddAccountParam& item);
    void to_json(nlohmann::json& j, const AddAccountParam& item);

    void from_json(const nlohmann::json& j, AddAccountResult& item);
    void to_json(nlohmann::json& j, const AddAccountResult& item);

    void from_json(const nlohmann::json& j, GetAccountParam& item);
    void to_json(nlohmann::json& j, const GetAccountParam& item);

    void from_json(const nlohmann::json& j, GetAccountResult& item);
    void to_json(nlohmann::json& j, const GetAccountResult& item);

    void from_json(const nlohmann::json& j, AddCharacterParam& item);
    void to_json(nlohmann::json& j, const AddCharacterParam& item);

    void from_json(const nlohmann::json& j, RemoveCharacterParam& item);
    void to_json(nlohmann::json& j, const RemoveCharacterParam& item);

    void from_json(const nlohmann::json& j, RemoveCharacterResult& item);
    void to_json(nlohmann::json& j, const RemoveCharacterResult& item);

    void from_json(const nlohmann::json& j, AddCharacterResult& item);
    void to_json(nlohmann::json& j, const AddCharacterResult& item);

    void from_json(const nlohmann::json& j, GetLobbyCharactersParam& item);
    void to_json(nlohmann::json& j, const GetLobbyCharactersParam& item);

    void from_json(const nlohmann::json& j, GetLobbyCharactersResult& item);
    void to_json(nlohmann::json& j, const GetLobbyCharactersResult& item);

}
