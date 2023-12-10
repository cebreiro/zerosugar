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
using zerosugar::execution::AsioExecutor;
using zerosugar::execution::IExecutor;

class TestApp final : public AppInstance
{
public:
    static constexpr uint16_t PORT = 52342; // magic number

public:
    TestApp();

    auto GetServer() -> class TestServer&;
    auto GetExecutor() -> IExecutor&;
    auto GetStrands() const -> const std::vector<SharedPtrNotNull<Strand>>&;

    auto GetName() const -> std::string_view override;

private:
    void OnStartUp(std::span<char*> args) override;
    void OnShutdown() override;

private:
    SharedPtrNotNull<AsioExecutor> _executor;
    SharedPtrNotNull<class TestServer> _server;
    std::vector<SharedPtrNotNull<Strand>> _strands;
};
