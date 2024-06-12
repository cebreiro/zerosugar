#include "repository_service_generated_json_serialize.h"

namespace zerosugar::sl::service
{
    void from_json(const nlohmann::json& j, FindAccountParam& item)
    {
        j.at("account").get_to(item.account);
    }

    void to_json(nlohmann::json& j, const FindAccountParam& item)
    {
        j = nlohmann::json
            {
                { "account", item.account },
            };
    }

    void from_json(const nlohmann::json& j, FindAccountResult& item)
    {
        j.at("errorCode").get_to(item.errorCode);
        if (const auto iter = j.find("account"); iter != j.end())
        {
            item.account.emplace(*iter);
        }
    }

    void to_json(nlohmann::json& j, const FindAccountResult& item)
    {
        j = nlohmann::json
            {
                { "errorCode", item.errorCode },
            };

        if (item.account.has_value())
        {
            j.push_back(nlohmann::json{ "account", *item.account });
        }
    }

    void from_json(const nlohmann::json& j, UpdateAccountParam& item)
    {
        j.at("accountUpdate").get_to(item.accountUpdate);
    }

    void to_json(nlohmann::json& j, const UpdateAccountParam& item)
    {
        j = nlohmann::json
            {
                { "accountUpdate", item.accountUpdate },
            };
    }

    void from_json(const nlohmann::json& j, UpdateAccountResult& item)
    {
        j.at("errorCode").get_to(item.errorCode);
    }

    void to_json(nlohmann::json& j, const UpdateAccountResult& item)
    {
        j = nlohmann::json
            {
                { "errorCode", item.errorCode },
            };
    }

    void from_json(const nlohmann::json& j, GetCharacterListParam& item)
    {
        j.at("accountId").get_to(item.accountId);
    }

    void to_json(nlohmann::json& j, const GetCharacterListParam& item)
    {
        j = nlohmann::json
            {
                { "accountId", item.accountId },
            };
    }

    void from_json(const nlohmann::json& j, GetCharacterListResult& item)
    {
        j.at("errorCode").get_to(item.errorCode);
        j.at("character").get_to(item.character);
    }

    void to_json(nlohmann::json& j, const GetCharacterListResult& item)
    {
        j = nlohmann::json
            {
                { "errorCode", item.errorCode },
                { "character", item.character },
            };
    }

    void from_json(const nlohmann::json& j, CreateCharacterParam& item)
    {
        j.at("accountId").get_to(item.accountId);
        j.at("character").get_to(item.character);
    }

    void to_json(nlohmann::json& j, const CreateCharacterParam& item)
    {
        j = nlohmann::json
            {
                { "accountId", item.accountId },
                { "character", item.character },
            };
    }

    void from_json(const nlohmann::json& j, CreateCharacterResult& item)
    {
        j.at("errorCode").get_to(item.errorCode);
    }

    void to_json(nlohmann::json& j, const CreateCharacterResult& item)
    {
        j = nlohmann::json
            {
                { "errorCode", item.errorCode },
            };
    }

    void from_json(const nlohmann::json& j, DeleteCharacterParam& item)
    {
        j.at("characterId").get_to(item.characterId);
    }

    void to_json(nlohmann::json& j, const DeleteCharacterParam& item)
    {
        j = nlohmann::json
            {
                { "characterId", item.characterId },
            };
    }

    void from_json(const nlohmann::json& j, DeleteCharacterResult& item)
    {
        j.at("errorCode").get_to(item.errorCode);
    }

    void to_json(nlohmann::json& j, const DeleteCharacterResult& item)
    {
        j = nlohmann::json
            {
                { "errorCode", item.errorCode },
            };
    }

    void from_json(const nlohmann::json& j, NameCheckCharacterParam& item)
    {
        j.at("name").get_to(item.name);
    }

    void to_json(nlohmann::json& j, const NameCheckCharacterParam& item)
    {
        j = nlohmann::json
            {
                { "name", item.name },
            };
    }

    void from_json(const nlohmann::json& j, NameCheckCharacterResult& item)
    {
        j.at("errorCode").get_to(item.errorCode);
    }

    void to_json(nlohmann::json& j, const NameCheckCharacterResult& item)
    {
        j = nlohmann::json
            {
                { "errorCode", item.errorCode },
            };
    }

    void from_json(const nlohmann::json& j, LoadCharacterParam& item)
    {
        j.at("characterId").get_to(item.characterId);
    }

    void to_json(nlohmann::json& j, const LoadCharacterParam& item)
    {
        j = nlohmann::json
            {
                { "characterId", item.characterId },
            };
    }

    void from_json(const nlohmann::json& j, LoadCharacterResult& item)
    {
        j.at("errorCode").get_to(item.errorCode);
        j.at("character").get_to(item.character);
    }

    void to_json(nlohmann::json& j, const LoadCharacterResult& item)
    {
        j = nlohmann::json
            {
                { "errorCode", item.errorCode },
                { "character", item.character },
            };
    }

}
