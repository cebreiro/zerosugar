#include "game_repository.h"

#include "zerosugar/xr/server/game/instance/contents/inventory/inventory_change_transaction.h"
#include "zerosugar/xr/service/model/generated/database_item_log_message_json.h"

namespace zerosugar::xr
{
    GameRepository::GameRepository(execution::IExecutor& executor, service_locator_type locator,
        const std::function<void(int64_t)>& saveErrorHandler, int64_t concurrency)
        : _serviceLocator(std::move(locator))
        , _saveErrorHandler(saveErrorHandler)
        , _contexts(std::max<int64_t>(1, concurrency))
    {
        for (int64_t i = 0; i < std::ssize(_contexts); ++i)
        {
            _contexts[i].strand = std::make_shared<Strand>(executor.SharedFromThis());
        }
    }

    GameRepository::~GameRepository()
    {
    }

    auto GameRepository::PublishItemUniqueId() -> int64_t
    {
        assert(_snowflake);

        return _snowflake->Generate();
    }

    auto GameRepository::Find(int64_t characterId) -> Future<std::optional<service::DTOCharacter>>
    {
        LocalContext& context = SelectLocalContext(characterId);

        co_await *context.strand;

        service::GetCharacterParam param;
        param.characterId = characterId;

        service::GetCharacterResult result = co_await _serviceLocator.Get<service::IDatabaseService>().GetCharacterAsync(param);
        if (result.errorCode != service::DatabaseServiceErrorCode::DatabaseErrorNone)
        {
            ZEROSUGAR_LOG_CRITICAL(_serviceLocator,
                fmt::format("[{}] fail to get character data fro database service. character_id: {}, error: {}",
                    GetName(), characterId, GetEnumName(result.errorCode)));

            co_return std::nullopt;
        }

        co_return result.character;
    }

    auto GameRepository::FinalizeSaves(int64_t characterId) -> Future<void>
    {
        LocalContext& context = SelectLocalContext(characterId);

        if (!ExecutionContext::IsEqualTo(*context.strand))
        {
            co_await *context.strand;
        }

        assert(ExecutionContext::IsEqualTo(*context.strand));

        const auto iter = context.saves.find(characterId);
        if (iter == context.saves.end())
        {
            co_return;
        }

        CharacterSave& characterSave = iter->second;
        characterSave.shutdown = true;

        if (!characterSave.waits.empty() || characterSave.pending.IsValid())
        {
            Promise<void>& promise = characterSave.finalizeSignals.emplace_back();
            Future<void> future = promise.GetFuture();

            co_await future;
        }

        context.saves.erase(iter);

        co_return;
    }

    void GameRepository::SaveChanges(InventoryChangeTransaction change)
    {
        const int64_t characterId = change.GetCharacterId();
        LocalContext& context = SelectLocalContext(characterId);

        Dispatch(*context.strand,
            [self = shared_from_this(), id = characterId, change = transaction_variant_type(std::move(change))]() mutable
            {
                self->SaveChanges(id, std::move(change));
            });
    }

    auto GameRepository::GetName() const -> std::string_view
    {
        return "game_repository";
    }

    void GameRepository::SetSnowFlake(std::unique_ptr<SharedSnowflake<>> snowflake)
    {
        _snowflake = std::move(snowflake);
    }

    void GameRepository::SaveChanges(int64_t characterId, transaction_variant_type change)
    {
        LocalContext& context = SelectLocalContext(characterId);
        assert(ExecutionContext::IsEqualTo(*context.strand));

        CharacterSave& save = FindOrInsert(context, characterId);
        if (save.shutdown)
        {
            return;
        }

        save.waits.emplace(std::move(change));

        if (!save.pending.IsValid())
        {
            save.pending = Start(save);
            save.pending.Then(*context.strand, [strand = context.strand, self = shared_from_this(), save = &save]()
                {
                    self->OnCompleteSaveChanges(*strand, *save);
                });
        }
    }

    void GameRepository::OnCompleteSaveChanges(Strand& strand, CharacterSave& save)
    {
        save.pending = Future<void>();

        if (!save.waits.empty())
        {
            save.pending = Start(save);
            save.pending.Then(strand, [strand = strand.shared_from_this(), self = shared_from_this(), save = &save]()
                {
                    self->OnCompleteSaveChanges(*strand, *save);
                });
        }

        if (save.shutdown && !save.pending.IsValid() && save.waits.empty())
        {
            for (Promise<void>& promise : save.finalizeSignals)
            {
                promise.Set();
            }

            // delete self
            [[maybe_unused]] bool removed = Remove(save.characterId);
            assert(removed);
        }
    }

    auto GameRepository::Start(CharacterSave& save) -> Future<void>
    {
        assert(!save.pending.IsValid());
        assert(!save.waits.empty());

        transaction_variant_type va = std::move(save.waits.front());
        save.waits.pop();

        if (const InventoryChangeTransaction* transaction = std::get_if<InventoryChangeTransaction>(&va); transaction)
        {
            nlohmann::json jsonArray = nlohmann::json::array();

            for (const auto& logVariant : transaction->GetLogs())
            {
                std::visit([&jsonArray]<typename T>(const T & log)
                {
                    nlohmann::json result;

                    nlohmann::json data;
                    to_json(data, log);

                    result["type"] = T::opcode;
                    result["data"] = std::move(data);

                    jsonArray.push_back(std::move(result));

                }, logVariant);
            }

            service::CharacterItemChangeParam param;
            param.characterId = transaction->GetCharacterId();
            param.itemChangeLogs = jsonArray.dump();

            service::IDatabaseService& databaseService = _serviceLocator.Get<service::IDatabaseService>();

            service::CharacterItemChangeResult result =
                co_await databaseService.SaveCharacterItemChangeAsync(std::move(param));

            if (result.errorCode != service::DatabaseServiceErrorCode::DatabaseErrorNone)
            {
                ZEROSUGAR_LOG_CRITICAL(_serviceLocator,
                    fmt::format("fail to save item change. error: {}, character_id: {}, param: {}",
                        GetEnumName(result.errorCode), transaction->GetCharacterId(), jsonArray.dump()));

                assert(_saveErrorHandler);

                _saveErrorHandler(transaction->GetCharacterId());
            }
        }

        co_return;
    }

    auto GameRepository::SelectLocalContext(int64_t characterId) -> LocalContext&
    {
        return _contexts[characterId % std::ssize(_contexts)];
    }

    auto GameRepository::FindOrInsert(LocalContext& context, int64_t characterId) -> CharacterSave&
    {
        auto iter = context.saves.find(characterId);
        if (iter == context.saves.end())
        {
            [[maybe_unused]]
            bool inserted = false;

            std::tie(iter, inserted) = context.saves.try_emplace(characterId, CharacterSave{});
            assert(inserted);

            iter->second.characterId = characterId;
        }

        return iter->second;
    }

    bool GameRepository::Remove(int64_t characterId)
    {
        LocalContext& context = SelectLocalContext(characterId);

        return context.saves.erase(characterId);
    }
}
