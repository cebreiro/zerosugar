#include "database_service_message_json.h"

namespace zerosugar::xr::service
{
    void from_json(const nlohmann::json& j, AddAccountParam& item)
    {
        j.at("account").get_to(item.account);
        j.at("password").get_to(item.password);
    }

    void to_json(nlohmann::json& j, const AddAccountParam& item)
    {
        j = nlohmann::json
            {
                { "account", item.account },
                { "password", item.password },
            };
    }

    void from_json(const nlohmann::json& j, AddAccountResult& item)
    {
        j.at("errorCode").get_to(item.errorCode);
    }

    void to_json(nlohmann::json& j, const AddAccountResult& item)
    {
        j = nlohmann::json
            {
                { "errorCode", item.errorCode },
            };
    }

    void from_json(const nlohmann::json& j, GetAccountParam& item)
    {
        j.at("account").get_to(item.account);
    }

    void to_json(nlohmann::json& j, const GetAccountParam& item)
    {
        j = nlohmann::json
            {
                { "account", item.account },
            };
    }

    void from_json(const nlohmann::json& j, GetAccountResult& item)
    {
        j.at("errorCode").get_to(item.errorCode);
        j.at("account").get_to(item.account);
    }

    void to_json(nlohmann::json& j, const GetAccountResult& item)
    {
        j = nlohmann::json
            {
                { "errorCode", item.errorCode },
                { "account", item.account },
            };
    }

    void from_json(const nlohmann::json& j, AddCharacterParam& item)
    {
        j.at("characterAdd").get_to(item.characterAdd);
        j.at("equipItems").get_to(item.equipItems);
        j.at("items").get_to(item.items);
    }

    void to_json(nlohmann::json& j, const AddCharacterParam& item)
    {
        j = nlohmann::json
            {
                { "characterAdd", item.characterAdd },
                { "equipItems", item.equipItems },
                { "items", item.items },
            };
    }

    void from_json(const nlohmann::json& j, AddCharacterResult& item)
    {
        j.at("errorCode").get_to(item.errorCode);
    }

    void to_json(nlohmann::json& j, const AddCharacterResult& item)
    {
        j = nlohmann::json
            {
                { "errorCode", item.errorCode },
            };
    }

    void from_json(const nlohmann::json& j, GetLobbyCharactersParam& item)
    {
        j.at("accountId").get_to(item.accountId);
    }

    void to_json(nlohmann::json& j, const GetLobbyCharactersParam& item)
    {
        j = nlohmann::json
            {
                { "accountId", item.accountId },
            };
    }

    void from_json(const nlohmann::json& j, GetLobbyCharactersResult& item)
    {
        j.at("errorCode").get_to(item.errorCode);
        j.at("lobbyCharacters").get_to(item.lobbyCharacters);
    }

    void to_json(nlohmann::json& j, const GetLobbyCharactersResult& item)
    {
        j = nlohmann::json
            {
                { "errorCode", item.errorCode },
                { "lobbyCharacters", item.lobbyCharacters },
            };
    }

}
