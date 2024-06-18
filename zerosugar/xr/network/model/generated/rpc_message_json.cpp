#include "rpc_message_json.h"

namespace zerosugar::xr::network
{
    void from_json(const nlohmann::json& j, RequestRegisterRPCClient& item)
    {
        j.at("serviceName").get_to(item.serviceName);
        j.at("ip").get_to(item.ip);
        j.at("port").get_to(item.port);
    }

    void to_json(nlohmann::json& j, const RequestRegisterRPCClient& item)
    {
        j = nlohmann::json
            {
                { "serviceName", item.serviceName },
                { "ip", item.ip },
                { "port", item.port },
            };
    }

    void from_json(const nlohmann::json& j, ResultRegisterRPCClient& item)
    {
        j.at("errorCode").get_to(item.errorCode);
        j.at("serviceName").get_to(item.serviceName);
    }

    void to_json(nlohmann::json& j, const ResultRegisterRPCClient& item)
    {
        j = nlohmann::json
            {
                { "errorCode", item.errorCode },
                { "serviceName", item.serviceName },
            };
    }

    void from_json(const nlohmann::json& j, RequestRemoteProcedureCall& item)
    {
        j.at("rpcId").get_to(item.rpcId);
        j.at("serviceName").get_to(item.serviceName);
        j.at("rpcName").get_to(item.rpcName);
        j.at("parameter").get_to(item.parameter);
    }

    void to_json(nlohmann::json& j, const RequestRemoteProcedureCall& item)
    {
        j = nlohmann::json
            {
                { "rpcId", item.rpcId },
                { "serviceName", item.serviceName },
                { "rpcName", item.rpcName },
                { "parameter", item.parameter },
            };
    }

    void from_json(const nlohmann::json& j, ResultRemoteProcedureCall& item)
    {
        j.at("errorCode").get_to(item.errorCode);
        j.at("rpcId").get_to(item.rpcId);
        j.at("serviceName").get_to(item.serviceName);
        j.at("rpcResult").get_to(item.rpcResult);
    }

    void to_json(nlohmann::json& j, const ResultRemoteProcedureCall& item)
    {
        j = nlohmann::json
            {
                { "errorCode", item.errorCode },
                { "rpcId", item.rpcId },
                { "serviceName", item.serviceName },
                { "rpcResult", item.rpcResult },
            };
    }

    void from_json(const nlohmann::json& j, SendServerStreaming& item)
    {
        j.at("rpcId").get_to(item.rpcId);
        j.at("serviceName").get_to(item.serviceName);
        j.at("rpcResult").get_to(item.rpcResult);
    }

    void to_json(nlohmann::json& j, const SendServerStreaming& item)
    {
        j = nlohmann::json
            {
                { "rpcId", item.rpcId },
                { "serviceName", item.serviceName },
                { "rpcResult", item.rpcResult },
            };
    }

    void from_json(const nlohmann::json& j, SendClientSteaming& item)
    {
        j.at("rpcId").get_to(item.rpcId);
        j.at("serviceName").get_to(item.serviceName);
        j.at("parameter").get_to(item.parameter);
    }

    void to_json(nlohmann::json& j, const SendClientSteaming& item)
    {
        j = nlohmann::json
            {
                { "rpcId", item.rpcId },
                { "serviceName", item.serviceName },
                { "parameter", item.parameter },
            };
    }

    void from_json(const nlohmann::json& j, AbortClientStreamingRPC& item)
    {
        j.at("rpcId").get_to(item.rpcId);
        j.at("serviceName").get_to(item.serviceName);
    }

    void to_json(nlohmann::json& j, const AbortClientStreamingRPC& item)
    {
        j = nlohmann::json
            {
                { "rpcId", item.rpcId },
                { "serviceName", item.serviceName },
            };
    }

}
