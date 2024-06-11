#pragma once

namespace zerosugar::xr::network::service
{
    struct RequestRemoteProcedureCall;
    struct ResultRemoteProcedureCall;
}

namespace zerosugar::xr
{
    class ServicePacketBuilder
    {
    public:
        ServicePacketBuilder() = delete;


        static auto MakePacket(const network::service::RequestRemoteProcedureCall& item) -> Buffer;
        static auto MakePacket(const network::service::ResultRemoteProcedureCall& item) -> Buffer;

    };
}
