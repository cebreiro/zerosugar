#include "repository_service.h"

#include "zerosugar/sl/database/connection/connection_pool.h"
#include "zerosugar/sl/database/helper/transaction.h"
#include "zerosugar/sl/database/generated/account_table.h"
#include "zerosugar/sl/database/generated/character_table.h"
#include "zerosugar/sl/database/generated/item_table.h"
#include "zerosugar/sl/database/generated/skill_table.h"
#include "zerosugar/sl/service/repository/repository_model_translator.h"

namespace zerosugar::sl
{
    auto Convert(const db::Account& account) -> service::Account
    {
        return service::Account{
            .id = account.id,
            .account = account.account,
            .password = account.password,
            .gmLevel = account.gm_level,
            .banned = static_cast<bool>(account.banned),
            .banReason = account.ban_reason ? *account.ban_reason : std::string(),
        };
    }

    RepositoryService::RepositoryService(execution::IExecutor& executor, SharedPtrNotNull<db::ConnectionPool> connectionPool)
        : _connectionPool(std::move(connectionPool))
    {
        auto ex = executor.SharedFromThis();

        _master = std::make_shared<Strand>(ex);

        for (int64_t i = 0; i < std::ssize(_worker); ++i)
        {
            _worker[i] = std::make_shared<Strand>(ex);
        }
    }

    void RepositoryService::Initialize(ServiceLocator& serviceLocator)
    {
        _locator = serviceLocator;
        if (!_locator.ContainsAll())
        {
            throw std::runtime_error("[sl_repository_service] dependency is not satisfied");
        }
    }

    auto RepositoryService::FindAccountAsync(service::FindAccountParam param) -> Future<service::FindAccountResult>
    {
        co_await *_master;

        if (auto iter = _accountNameIndexer.find(param.account); iter != _accountNameIndexer.end())
        {
            co_return service::FindAccountResult{
                .account = Convert(*iter->second),
            };
        }

        execution::IExecutor& executor = SelectWorker(param.account);
        co_await executor;
        

        std::optional<db::Account> account = std::nullopt;
        try
        {
            db::ConnectionPool::BorrowedConnection connection = co_await _connectionPool->Lend();
            
            assert(connection.IsValid());

            db::AccountTable table(*connection);
            account =  table.FindByACCOUNT(param.account);
        }
        catch (const boost::mysql::error_with_diagnostics& e)
        {
            LogDatabaseError(e.get_diagnostics().server_message());
        }
        catch (const std::exception& e)
        {
            LogDatabaseError(e.what());
        }

        if (!account.has_value())
        {
            co_return service::FindAccountResult{
                .errorCode = service::RepositoryServiceErrorCode::RepositoryAccountFailToFindKey,
            };
        }

        co_await *_master;

        auto accountCache = std::make_shared<db::Account>(*account);
        _accounts[account->id] = accountCache;
        _accountNameIndexer[account->account] = std::move(accountCache);

        co_return service::FindAccountResult{
            .account = Convert(*account),
        };
    }

    auto RepositoryService::UpdateAccountAsync(
        service::UpdateAccountParam param) -> Future<service::UpdateAccountResult>
    {
        co_await *_master;
        assert(ExecutionContext::GetExecutor() == _master.get());

        auto iter = _accounts.find(param.accountUpdate.id);
        if (iter == _accounts.end())
        {
            co_return service::UpdateAccountResult{
                .errorCode = service::RepositoryServiceErrorCode::RepositoryAccountFailToFindKey,
            };
        }

        std::shared_ptr<db::Account> oldOne = iter->second;
        db::Account newOne = *oldOne;
        {
            if (param.accountUpdate.password.has_value())
            {
                newOne.password = *param.accountUpdate.password;
            }

            if (param.accountUpdate.gmLevel.has_value())
            {
                newOne.gm_level = static_cast<int8_t>(*param.accountUpdate.gmLevel);
            }

            if (param.accountUpdate.banned.has_value())
            {
                newOne.banned = *param.accountUpdate.banned;
            }

            if (param.accountUpdate.banReason.has_value())
            {
                newOne.ban_reason = *param.accountUpdate.banReason;
            }
        }

        execution::IExecutor& executor = SelectWorker(oldOne->account);
        co_await executor;
        

        bool success = false;
        try
        {
            db::ConnectionPool::BorrowedConnection connection = co_await _connectionPool->Lend();
            
            assert(connection.IsValid());

            db::AccountTable table(*connection);
            table.UpdateDifference(*oldOne, newOne);

            success = true;
        }
        catch (const boost::mysql::error_with_diagnostics& e)
        {
            LogDatabaseError(e.get_diagnostics().server_message());
        }
        catch (const std::exception& e)
        {
            LogDatabaseError(e.what());
        }

        if (!success)
        {
            co_return service::UpdateAccountResult{
                .errorCode = service::RepositoryServiceErrorCode::RepositoryInternalDbError,
            };
        }

        co_await *_master;
        assert(ExecutionContext::GetExecutor() == _master.get());

        *oldOne = newOne;

        co_return service::UpdateAccountResult{
            .errorCode = service::RepositoryServiceErrorCode::RepositoryErrorNone,
        };
    }

    auto RepositoryService::GetCharacterListAsync(service::GetCharacterListParam param)
        -> Future<service::GetCharacterListResult>
    {
        execution::IExecutor& executor = SelectWorker(param.accountId);
        co_await executor;

        bool success = false;
        std::vector<service::Character> result;
        try
        {
            db::ConnectionPool::BorrowedConnection connection = co_await _connectionPool->Lend();
            
            assert(connection.IsValid());

            db::CharacterTable characterTable(*connection);
            db::CharacterStatTable statTable(*connection);
            db::CharacterJobTable jobTable(*connection);
            db::ItemTable itemTable(*connection);
            db::SkillTable skillTable(*connection);

            const std::vector<db::Character>& characters = characterTable.FindRangeByAID(param.accountId);
            for (const db::Character& character : characters)
            {
                if (character.deleted)
                {
                    continue;
                }

                std::optional<db::CharacterStat> stat = statTable.FindByCID(character.id);
                if (!stat.has_value())
                {
                    throw std::runtime_error("character stat is null");
                }

                std::optional<db::CharacterJob> job = jobTable.FindByCID(character.id);
                if (!job.has_value())
                {
                    throw std::runtime_error("character job is null");
                }

                std::vector<db::Item> items = itemTable.FindRangeByOWNER_ID(character.id);
                std::vector<db::Skill> skills = skillTable.FindRangeByOWNER_ID(character.id);

                result.emplace_back(RepositoryModelTranslator::ToServiceModel(
                    DatabaseModelRef{
                        .character = character,
                        .stat = *stat,
                        .job = *job,
                        .items = items,
                        .skills = skills,
                    }));
            }

            success = true;
        }
        catch (const boost::mysql::error_with_diagnostics& e)
        {
            LogDatabaseError(e.get_diagnostics().server_message());
        }
        catch (const std::exception& e)
        {
            LogDatabaseError(e.what());
        }

        if (success)
        {
            co_return service::GetCharacterListResult{
                .errorCode = service::RepositoryServiceErrorCode::RepositoryErrorNone,
                .character = std::move(result),
            };
        }
        else
        {
            co_return service::GetCharacterListResult{
                .errorCode = service::RepositoryServiceErrorCode::RepositoryInternalDbError,
            };
        }
    }

    auto RepositoryService::CreateCharacterAsync(service::CreateCharacterParam param)
        -> Future<service::CreateCharacterResult>
    {
        execution::IExecutor& executor = SelectWorker(param.character.id);
        co_await executor;

        bool success = false;
        try
        {
            db::ConnectionPool::BorrowedConnection connection = co_await _connectionPool->Lend();
            assert(connection.IsValid());

            db::Transaction transaction(*connection);
            transaction.Start();

            try
            {
                DatabaseModel model = RepositoryModelTranslator::ToDatabaseModel(param.character);

                db::CharacterTable characterTable(*connection);
                characterTable.Add(model.character);

                db::CharacterStatTable statTable(*connection);
                statTable.Add(model.stat);

                db::CharacterJobTable jobTable(*connection);
                jobTable.Add(model.job);

                if (!model.items.empty())
                {
                    db::ItemTable itemTable(*connection);

                    // TODO: add range
                    for (const db::Item& item : model.items)
                    {
                        itemTable.Add(item);
                    }
                }

                if (!model.skills.empty())
                {
                    db::SkillTable skillTable(*connection);

                    for (const db::Skill& skill : model.skills)
                    {
                        skillTable.Add(skill);
                    }
                }

                transaction.Commit();
                success = true;
            }
            catch (...)
            {
                transaction.Rollback();
                throw;
            }
        }
        catch (const boost::mysql::error_with_diagnostics& e)
        {
            LogDatabaseError(e.get_diagnostics().server_message());
        }
        catch (const std::exception& e)
        {
            LogDatabaseError(e.what());
        }

        co_return service::CreateCharacterResult{
            .errorCode = success ?
            service::RepositoryServiceErrorCode::RepositoryErrorNone :
            service::RepositoryServiceErrorCode::RepositoryInternalDbError,
        };
    }

    auto RepositoryService::DeleteCharacterAsync(service::DeleteCharacterParam param)
        -> Future<service::DeleteCharacterResult>
    {
        execution::IExecutor& executor = SelectWorker(param.characterId);
        co_await executor;

        bool success = false;
        try
        {
            db::ConnectionPool::BorrowedConnection connection = co_await _connectionPool->Lend();
            assert(connection.IsValid());

            db::CharacterTable characterTable(*connection);
            const std::optional<db::Character>& character = characterTable.Find(param.characterId);

            if (character.has_value() && !character->deleted)
            {
                db::Character newCharacter(*character);

                newCharacter.name = std::format("deleted_{}", std::chrono::steady_clock::now().time_since_epoch());
                newCharacter.deleted = true;

                characterTable.UpdateDifference(*character, newCharacter);
            }

            success = true;
        }
        catch (const boost::mysql::error_with_diagnostics& e)
        {
            LogDatabaseError(e.get_diagnostics().server_message());
        }
        catch (const std::exception& e)
        {
            LogDatabaseError(e.what());
        }

        co_return service::DeleteCharacterResult{
            .errorCode = success ?
            service::RepositoryServiceErrorCode::RepositoryErrorNone :
            service::RepositoryServiceErrorCode::RepositoryCharacterError,
        };
    }

    auto RepositoryService::NameCheckCharacterAsync(service::NameCheckCharacterParam param)
        -> Future<service::NameCheckCharacterResult>
    {
        execution::IExecutor& executor = SelectWorker(param.name);
        co_await executor;

        bool success = false;

        try
        {
            db::ConnectionPool::BorrowedConnection connection = co_await _connectionPool->Lend();
            assert(connection.IsValid());

            db::CharacterTable characterTable(*connection);
            success = !characterTable.FindByNAME(param.name).has_value();
        }
        catch (const boost::mysql::error_with_diagnostics& e)
        {
            LogDatabaseError(e.get_diagnostics().server_message());
        }
        catch (const std::exception& e)
        {
            LogDatabaseError(e.what());
        }

        co_return service::NameCheckCharacterResult{
            .errorCode = success ?
            service::RepositoryServiceErrorCode::RepositoryErrorNone :
            service::RepositoryServiceErrorCode::RepositoryCharacterError,
        };
    }

    auto RepositoryService::LoadCharacterAsync(service::LoadCharacterParam param)
        -> Future<service::LoadCharacterResult>
    {
        execution::IExecutor& executor = SelectWorker(param.characterId);
        co_await executor;

        std::optional<service::Character> result;
        try
        {
            db::ConnectionPool::BorrowedConnection connection = co_await _connectionPool->Lend();
            
            assert(connection.IsValid());

            db::CharacterTable characterTable(*connection);
            db::CharacterStatTable statTable(*connection);
            db::CharacterJobTable jobTable(*connection);
            db::ItemTable itemTable(*connection);
            db::SkillTable skillTable(*connection);

            std::optional<db::Character> character = characterTable.Find(param.characterId);
            if (character.has_value())
            {
                std::optional<db::CharacterStat> stat = statTable.FindByCID(character->id);
                if (!stat.has_value())
                {
                    throw std::runtime_error("character stat is null");
                }

                std::optional<db::CharacterJob> job = jobTable.FindByCID(character->id);
                if (!job.has_value())
                {
                    throw std::runtime_error("character job is null");
                }

                std::vector<db::Item> items = itemTable.FindRangeByOWNER_ID(character->id);
                std::vector<db::Skill> skills = skillTable.FindRangeByOWNER_ID(character->id);

                result.emplace(RepositoryModelTranslator::ToServiceModel(
                    DatabaseModelRef{
                        .character = *character,
                        .stat = *stat,
                        .job = *job,
                        .items = items,
                        .skills = skills,
                    }));
            }
        }
        catch (const boost::mysql::error_with_diagnostics& e)
        {
            LogDatabaseError(e.get_diagnostics().server_message());
        }
        catch (const std::exception& e)
        {
            LogDatabaseError(e.what());
        }

        if (!result.has_value())
        {
            co_return service::LoadCharacterResult{
                .errorCode = service::RepositoryServiceErrorCode::RepositoryInternalDbError,
            };
        }

        co_return service::LoadCharacterResult{
            .errorCode = service::RepositoryServiceErrorCode::RepositoryErrorNone,
            .character = std::move(*result),
        };
    }

    void RepositoryService::LogDatabaseError(std::string_view diagnosticMessage)
    {
        ZEROSUGAR_LOG_ERROR(_locator, std::format("[{}] db error. error: {}",
            GetName(), diagnosticMessage));
    }

    void RepositoryService::LogException(std::string_view message)
    {
        ZEROSUGAR_LOG_ERROR(_locator, std::format("[{}] throws an exception. exception: {}",
            GetName(), message));
    }

    auto RepositoryService::GetName() const -> std::string_view
    {
        return "repository_service";
    }

    auto RepositoryService::SelectWorker(const std::string& account) const -> execution::IExecutor&
    {
        return SelectWorker(std::hash<std::string>()(account));
    }

    auto RepositoryService::SelectWorker(int64_t hash) const -> execution::IExecutor&
    {
        return *_worker[hash % std::ssize(_worker)];
    }
}
