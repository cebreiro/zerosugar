#include "game_cs_message_json.h"

namespace zerosugar::xr::network::game::cs
{
    void from_json(const nlohmann::json& j, Ping& item)
    {
        j.at("sequence").get_to(item.sequence);
        j.at("clientTimePoint").get_to(item.clientTimePoint);
        j.at("serverTimePoint").get_to(item.serverTimePoint);
    }

    void to_json(nlohmann::json& j, const Ping& item)
    {
        j = nlohmann::json
            {
                { "sequence", item.sequence },
                { "clientTimePoint", item.clientTimePoint },
                { "serverTimePoint", item.serverTimePoint },
            };
    }

    void from_json(const nlohmann::json& j, Authenticate& item)
    {
        j.at("authenticationToken").get_to(item.authenticationToken);
    }

    void to_json(nlohmann::json& j, const Authenticate& item)
    {
        j = nlohmann::json
            {
                { "authenticationToken", item.authenticationToken },
            };
    }

    void from_json(const nlohmann::json& j, LoadLevelComplete& item)
    {
        (void)j;
        (void)item;
    }

    void to_json(nlohmann::json& j, const LoadLevelComplete& item)
    {
        (void)j;
        (void)item;
    }

    void from_json(const nlohmann::json& j, StartPlayerAttack& item)
    {
        j.at("skillId").get_to(item.skillId);
        j.at("position").get_to(item.position);
        j.at("rotation").get_to(item.rotation);
    }

    void to_json(nlohmann::json& j, const StartPlayerAttack& item)
    {
        j = nlohmann::json
            {
                { "skillId", item.skillId },
                { "position", item.position },
                { "rotation", item.rotation },
            };
    }

    void from_json(const nlohmann::json& j, ApplyPlayerAttack& item)
    {
        j.at("id").get_to(item.id);
        j.at("targetCount").get_to(item.targetCount);
        j.at("targets").get_to(item.targets);
        j.at("skillId").get_to(item.skillId);
        j.at("position").get_to(item.position);
        j.at("rotation").get_to(item.rotation);
    }

    void to_json(nlohmann::json& j, const ApplyPlayerAttack& item)
    {
        j = nlohmann::json
            {
                { "id", item.id },
                { "targetCount", item.targetCount },
                { "targets", item.targets },
                { "skillId", item.skillId },
                { "position", item.position },
                { "rotation", item.rotation },
            };
    }

    void from_json(const nlohmann::json& j, MovePlayer& item)
    {
        j.at("position").get_to(item.position);
        j.at("rotation").get_to(item.rotation);
    }

    void to_json(nlohmann::json& j, const MovePlayer& item)
    {
        j = nlohmann::json
            {
                { "position", item.position },
                { "rotation", item.rotation },
            };
    }

    void from_json(const nlohmann::json& j, StopPlayer& item)
    {
        j.at("id").get_to(item.id);
        j.at("position").get_to(item.position);
    }

    void to_json(nlohmann::json& j, const StopPlayer& item)
    {
        j = nlohmann::json
            {
                { "id", item.id },
                { "position", item.position },
            };
    }

    void from_json(const nlohmann::json& j, SprintPlayer& item)
    {
        j.at("id").get_to(item.id);
    }

    void to_json(nlohmann::json& j, const SprintPlayer& item)
    {
        j = nlohmann::json
            {
                { "id", item.id },
            };
    }

    void from_json(const nlohmann::json& j, RollDodgePlayer& item)
    {
        j.at("id").get_to(item.id);
        j.at("rotation").get_to(item.rotation);
    }

    void to_json(nlohmann::json& j, const RollDodgePlayer& item)
    {
        j = nlohmann::json
            {
                { "id", item.id },
                { "rotation", item.rotation },
            };
    }

    void from_json(const nlohmann::json& j, Chat& item)
    {
        j.at("message").get_to(item.message);
    }

    void to_json(nlohmann::json& j, const Chat& item)
    {
        j = nlohmann::json
            {
                { "message", item.message },
            };
    }

    void from_json(const nlohmann::json& j, SwapItem& item)
    {
        j.at("destEquipped").get_to(item.destEquipped);
        j.at("destPosition").get_to(item.destPosition);
        j.at("srcEquipped").get_to(item.srcEquipped);
        j.at("srcPosition").get_to(item.srcPosition);
    }

    void to_json(nlohmann::json& j, const SwapItem& item)
    {
        j = nlohmann::json
            {
                { "destEquipped", item.destEquipped },
                { "destPosition", item.destPosition },
                { "srcEquipped", item.srcEquipped },
                { "srcPosition", item.srcPosition },
            };
    }

    void from_json(const nlohmann::json& j, StartDungeonMatch& item)
    {
        j.at("deugeonId").get_to(item.deugeonId);
    }

    void to_json(nlohmann::json& j, const StartDungeonMatch& item)
    {
        j = nlohmann::json
            {
                { "deugeonId", item.deugeonId },
            };
    }

    void from_json(const nlohmann::json& j, CancelDungeonMatch& item)
    {
        (void)j;
        (void)item;
    }

    void to_json(nlohmann::json& j, const CancelDungeonMatch& item)
    {
        (void)j;
        (void)item;
    }

    void from_json(const nlohmann::json& j, ApproveDungeonMatch& item)
    {
        (void)j;
        (void)item;
    }

    void to_json(nlohmann::json& j, const ApproveDungeonMatch& item)
    {
        (void)j;
        (void)item;
    }

    void from_json(const nlohmann::json& j, RejectDungeonMatch& item)
    {
        (void)j;
        (void)item;
    }

    void to_json(nlohmann::json& j, const RejectDungeonMatch& item)
    {
        (void)j;
        (void)item;
    }

}
