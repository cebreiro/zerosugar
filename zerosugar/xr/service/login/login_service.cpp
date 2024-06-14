#include "login_service.h"

#include <openssl/evp.h>
#include <openssl/sha.h>
#include "zerosugar/xr/network/rpc/rpc_client.h"
#include "zerosugar/xr/service/model/generated/login_service_message_json.h"

namespace zerosugar::xr
{
    LoginService::LoginService(SharedPtrNotNull<execution::IExecutor> executor)
        : _executor(std::move(executor))
        , _strand(std::make_shared<Strand>(_executor))
        , evpContext(EVP_MD_CTX_new())
    {
    }

    LoginService::~LoginService()
    {
        EVP_MD_CTX_free(static_cast<EVP_MD_CTX*>(evpContext));
    }

    void LoginService::Initialize(ServiceLocator& serviceLocator)
    {
        ILoginService::Initialize(serviceLocator);

        _serviceLocator = serviceLocator;

        ConfigureRemoteProcedureClient(serviceLocator.Get<RPCClient>());
    }

    void LoginService::Shutdown()
    {
        ILoginService::Shutdown();
    }

    void LoginService::Join(std::vector<boost::system::error_code>& errors)
    {
        ILoginService::Join(errors);
    }

    auto LoginService::LoginAsync(service::LoginParam param) -> Future<service::LoginResult>
    {
        (void)param;
        co_return{};
    }

    auto LoginService::CreateAccountAsync(service::CreateAccountParam param) -> Future<service::CreateAccountResult>
    {
        using namespace service;

        co_await *_strand;

        AddAccountParam addAccountParam{
            .account = param.account,
            .password = MakeSHA256(param.password),
        };

        AddAccountResult addAccountResult = co_await _serviceLocator.Get<IDatabaseService>()
            .AddAccountAsync(std::move(addAccountParam));

        CreateAccountResult result;
        result.errorCode = addAccountResult.errorCode == DatabaseServiceErrorCode::DatabaseErrorNone
            ? LoginServiceErrorCode::LoginErrorNone : LoginServiceErrorCode::LoginErrorFailInvalid;

        co_return result;
    }

    void LoginService::ConfigureRemoteProcedureClient(RPCClient& rpcClient)
    {
        // TODO: code-generation
        rpcClient.RegisterProcedure<LoginService>("LoginAsync",
            [self = shared_from_this()](service::LoginParam param) -> Future<service::LoginResult>
            {
                return self->LoginAsync(std::move(param));
            });

        rpcClient.RegisterProcedure<LoginService>("CreateAccountAsync",
            [self = shared_from_this()](service::CreateAccountParam param) -> Future<service::CreateAccountResult>
            {
                return self->CreateAccountAsync(std::move(param));
            });
    }

    auto LoginService::MakeSHA256(const std::string& str) -> std::string
    {
        assert(ExecutionContext::IsEqualTo(*_strand));

        EVP_MD_CTX* context = static_cast<EVP_MD_CTX*>(evpContext);
        const EVP_MD* md = EVP_sha256();
        assert(md);

        [[maybe_unused]]
        const int32_t initResult = EVP_DigestInit_ex(context, md, nullptr);
        assert(initResult == 1);

        [[maybe_unused]]
        const int32_t updateResult = EVP_DigestUpdate(context, str.c_str(), str.size());
        assert(updateResult == 1);

        std::ostringstream oss;

        unsigned char hash[SHA256_DIGEST_LENGTH] = {};
        uint32_t hashLength = 0;

        [[maybe_unused]]
        const int32_t finalResult = EVP_DigestFinal_ex(context, hash, &hashLength);
        assert(finalResult == 1);

        for (uint32_t i = 0; i < hashLength; i++)
        {
            oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int32_t>(hash[i]);
        }

        return oss.str();
    }
}
