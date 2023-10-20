#pragma once
#include "zerosugar/shared/type/not_null_pointer.h"
#include "zerosugar/shared/app/app_intance.h"
#include "zerosugar/shared/execution/executor/strand.h"
#include "zerosugar/shared/network/server/server.h"

using zerosugar::AppInstance;
using zerosugar::Server;
using zerosugar::Strand;
using zerosugar::ServiceLocator;
using zerosugar::SharedPtrNotNull;

class TestApp final : public AppInstance
{
public:
    static constexpr uint16_t PORT = 52342; // magic number

public:
    TestApp();

    auto GetServer() -> Server&;
    auto GetStrands() const -> const std::vector<SharedPtrNotNull<Strand>>&;

private:
    void OnStartUp(ServiceLocator& serviceLocator) override;
    void OnShutdown() override;

private:
    SharedPtrNotNull<Server> _server;
    std::vector<SharedPtrNotNull<Strand>> _strands;
};
