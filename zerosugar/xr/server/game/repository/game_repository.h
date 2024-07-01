#pragma once
#include "zerosugar/xr/server/game/repository/game_repository_interface.h"
#include "zerosugar/xr/service/model/generated/database_service.h"

namespace zerosugar::xr
{
    class GameRepository final
        : public IGameRepository
        , public std::enable_shared_from_this<GameRepository>
    {
    private:
        using transaction_variant_type = std::variant<InventoryChangeTransaction>;
        using transaction_queue_type = std::queue<transaction_variant_type>;

        struct CharacterSave
        {
            bool shutdown = false;
            std::vector<Promise<void>> finalizeSignals;

            int64_t characterId = 0;
            Future<void> pending;
            transaction_queue_type waits;
        };

        struct LocalContext
        {
            SharedPtrNotNull<Strand> strand;
            std::unordered_map<int64_t, CharacterSave> saves;
        };

    public:
        using service_locator_type = ServiceLocatorT<ILogService, service::IDatabaseService>;

    public:
        GameRepository() = delete;

        GameRepository(execution::IExecutor& executor, service_locator_type locator, int64_t concurrency = 16);

        auto Find(int64_t characterId) -> Future<std::optional<service::DTOCharacter>> override;

        auto FinalizeSaves(int64_t characterId) -> Future<void> override;
        void SaveChanges(InventoryChangeTransaction change) override;

        auto GetName() const -> std::string_view override;

    private:
        void SaveChanges(int64_t characterId, transaction_variant_type change);
        void OnCompleteSaveChanges(Strand& strand, CharacterSave& save);

        auto Start(CharacterSave& save) -> Future<void>;

        auto SelectLocalContext(int64_t characterId) -> LocalContext&;
        auto FindOrInsert(LocalContext& context, int64_t characterId) -> CharacterSave&;
        bool Remove(int64_t characterId);

    private:
        service_locator_type _serviceLocator;
        std::vector<LocalContext> _contexts = {};
    };
}
