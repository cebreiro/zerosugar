#pragma once
#include <cstdint>
#include <string>
#include <optional>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>
#include "zerosugar/sl/service/generated/repository_service_generated.h"
#include "zerosugar/sl/service/generated/shared_generated.h"
#include "zerosugar/sl/service/generated/shared_generated_json_serialize.h"

namespace zerosugar::sl::service
{
    void from_json(const nlohmann::json& j, FindAccountParam& item);
    void to_json(nlohmann::json& j, const FindAccountParam& item);

    void from_json(const nlohmann::json& j, FindAccountResult& item);
    void to_json(nlohmann::json& j, const FindAccountResult& item);

    void from_json(const nlohmann::json& j, UpdateAccountParam& item);
    void to_json(nlohmann::json& j, const UpdateAccountParam& item);

    void from_json(const nlohmann::json& j, UpdateAccountResult& item);
    void to_json(nlohmann::json& j, const UpdateAccountResult& item);

    void from_json(const nlohmann::json& j, GetCharacterListParam& item);
    void to_json(nlohmann::json& j, const GetCharacterListParam& item);

    void from_json(const nlohmann::json& j, GetCharacterListResult& item);
    void to_json(nlohmann::json& j, const GetCharacterListResult& item);

    void from_json(const nlohmann::json& j, CreateCharacterParam& item);
    void to_json(nlohmann::json& j, const CreateCharacterParam& item);

    void from_json(const nlohmann::json& j, CreateCharacterResult& item);
    void to_json(nlohmann::json& j, const CreateCharacterResult& item);

    void from_json(const nlohmann::json& j, DeleteCharacterParam& item);
    void to_json(nlohmann::json& j, const DeleteCharacterParam& item);

    void from_json(const nlohmann::json& j, DeleteCharacterResult& item);
    void to_json(nlohmann::json& j, const DeleteCharacterResult& item);

    void from_json(const nlohmann::json& j, NameCheckCharacterParam& item);
    void to_json(nlohmann::json& j, const NameCheckCharacterParam& item);

    void from_json(const nlohmann::json& j, NameCheckCharacterResult& item);
    void to_json(nlohmann::json& j, const NameCheckCharacterResult& item);

    void from_json(const nlohmann::json& j, LoadCharacterParam& item);
    void to_json(nlohmann::json& j, const LoadCharacterParam& item);

    void from_json(const nlohmann::json& j, LoadCharacterResult& item);
    void to_json(nlohmann::json& j, const LoadCharacterResult& item);

}
