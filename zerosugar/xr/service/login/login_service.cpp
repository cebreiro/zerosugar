#include "login_service.h"

#include <boost/lexical_cast.hpp>
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
        , _mt(std::random_device{}())
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

        Configure(shared_from_this(), serviceLocator.Get<RPCClient>());
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
        [[maybe_unused]]
        auto self = shared_from_this();

        using namespace service;

        co_await *_strand;
        assert(ExecutionContext::IsEqualTo(*_strand));

        IDatabaseService& databaseService = _serviceLocator.Get<IDatabaseService>();

        GetAccountParam getAccountParam;
        getAccountParam.account = param.account;

        GetAccountResult getAccountResult = co_await databaseService.GetAccountAsync(std::move(getAccountParam));
        assert(ExecutionContext::IsEqualTo(*_strand));


        LoginResult result;

        do
        {
            if (getAccountResult.errorCode != DatabaseServiceErrorCode::DatabaseErrorNone)
            {
                break;
            }

            const DTOAccount& dto = getAccountResult.account;

            if (dto.banned || dto.deleted)
            {
                break;
            }

            const std::string& encoded = Encode(param.password);
            if (encoded != dto.password)
            {
                break;
            }

            std::string token = [this, &param, &dto]()
                {
                    while (true)
                    {
                        std::uniform_int_distribution<int64_t> dist;

                        const std::string& token = Encode(std::format("{}{}", param.account, dist(_mt)));

                        if (_authenticationTokens.try_emplace(token, dto.accountId).second)
                        {
                            return token;
                        }
                    }
                }();

            result.errorCode = LoginServiceErrorCode::LoginErrorNone;
            result.authenticationToken = std::move(token);

            co_return result;
            
        } while (false);

        result.errorCode = LoginServiceErrorCode::LoginErrorFailInvalid;

        co_return result;
    }

    auto LoginService::CreateAccountAsync(service::CreateAccountParam param) -> Future<service::CreateAccountResult>
    {
        [[maybe_unused]]
        auto self = shared_from_this();

        using namespace service;

        co_await *_strand;
        assert(ExecutionContext::IsEqualTo(*_strand));

        IDatabaseService& databaseService = _serviceLocator.Get<IDatabaseService>();

        AddAccountParam addAccountParam{
            .account = param.account,
            .password = Encode(param.password),
        };

        AddAccountResult addAccountResult = co_await databaseService.AddAccountAsync(std::move(addAccountParam));

        CreateAccountResult result;
        result.errorCode = addAccountResult.errorCode == DatabaseServiceErrorCode::DatabaseErrorNone
            ? LoginServiceErrorCode::LoginErrorNone : LoginServiceErrorCode::LoginErrorFailInvalid;

        co_return result;
    }

    auto LoginService::AuthenticateAsync(service::AuthenticateParam param) -> Future<service::AuthenticateResult>
    {
        [[maybe_unused]]
        auto self = shared_from_this();

        using namespace service;

        co_await *_strand;
        assert(ExecutionContext::IsEqualTo(*_strand));

        AuthenticateResult result;

        auto iter = _authenticationTokens.find(param.token);
        if (iter != _authenticationTokens.end())
        {
            result.accountId = iter->second;
        }
        else
        {
            result.errorCode = LoginServiceErrorCode::AuthenticateErrorFail;
        }

        co_return result;
    }

    auto LoginService::Test1Async(AsyncEnumerable<service::TestParam> param) -> Future<service::TestResult>
    {
        [[maybe_unused]]
        auto self = shared_from_this();

        co_await *_strand;
        assert(ExecutionContext::IsEqualTo(*_strand));

        int64_t i = 0;

        try
        {
            while (param.HasNext())
            {
                service::TestParam asd = co_await param;
                ++i;

                if (i > 50)
                {
                    throw std::runtime_error("test");
                }

                ZEROSUGAR_LOG_INFO(_serviceLocator, std::format("{}, {}, {}", __FUNCTION__, asd.token, i));
            }
        }
        catch (const std::exception& e)
        {
            ZEROSUGAR_LOG_WARN(_serviceLocator, std::format("{}, exception: {}", __FUNCTION__, e.what()));

            throw;
        }

        service::TestResult result;
        result.token = std::format("{} END", __FUNCTION__);

        ZEROSUGAR_LOG_INFO(_serviceLocator, std::format("{}, {} END", __FUNCTION__, i));

        co_return result;
    }

    auto LoginService::Test2Async(service::TestParam param) -> AsyncEnumerable<service::TestResult>
    {
        [[maybe_unused]]
        auto self = shared_from_this();

        co_await *_strand;
        assert(ExecutionContext::IsEqualTo(*_strand));

        const int64_t count = boost::lexical_cast<int64_t>(param.token);

        for (int64_t i = 0; i < count; ++i)
        {
            co_await Delay(std::chrono::milliseconds(1));

            service::TestResult result;
            result.token = std::format("{}:{}", __FUNCTION__, i);

            co_yield result;
        }

        ZEROSUGAR_LOG_DEBUG(_serviceLocator, std::format("{} END", __FUNCTION__));

        co_return;
    }

    auto LoginService::Test3Async(AsyncEnumerable<service::TestParam> param) -> AsyncEnumerable<service::TestResult>
    {
        [[maybe_unused]]
        auto self = shared_from_this();

        co_await *_strand;
        assert(ExecutionContext::IsEqualTo(*_strand));

        try
        {
            while (param.HasNext())
            {
                service::TestParam asd = co_await param;

                service::TestResult result;
                result.token = asd.token;

                ZEROSUGAR_LOG_DEBUG(_serviceLocator, std::format("{} : {}", __FUNCTION__, asd.token));

                co_yield result;
            }
        }
        catch (const std::exception& e)
        {
            ZEROSUGAR_LOG_WARN(_serviceLocator, std::format("{}, exception: {}", __FUNCTION__, e.what()));
        }

        ZEROSUGAR_LOG_DEBUG(_serviceLocator, std::format("{} END", __FUNCTION__));

        co_return;
    }

    auto LoginService::Encode(const std::string& str) -> std::string
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
