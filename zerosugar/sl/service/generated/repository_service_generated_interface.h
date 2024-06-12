#pragma once
#include <cstdint>
#include "zerosugar/sl/service/generated/repository_service_generated.h"
#include "zerosugar/sl/service/generated/shared_generated.h"
#include "zerosugar/shared/service/service_interface.h"
#include "zerosugar/shared/execution/future/future.h"
#include "zerosugar/shared/execution/channel/channel.h"

namespace zerosugar::sl::service
{
    class IRepositoryService : public IService
    {
    public:
        virtual ~IRepositoryService() = default;

        virtual auto FindAccountAsync(FindAccountParam param) -> Future<FindAccountResult> = 0;
        virtual auto UpdateAccountAsync(UpdateAccountParam param) -> Future<UpdateAccountResult> = 0;
        virtual auto GetCharacterListAsync(GetCharacterListParam param) -> Future<GetCharacterListResult> = 0;
        virtual auto CreateCharacterAsync(CreateCharacterParam param) -> Future<CreateCharacterResult> = 0;
        virtual auto DeleteCharacterAsync(DeleteCharacterParam param) -> Future<DeleteCharacterResult> = 0;
        virtual auto NameCheckCharacterAsync(NameCheckCharacterParam param) -> Future<NameCheckCharacterResult> = 0;
        virtual auto LoadCharacterAsync(LoadCharacterParam param) -> Future<LoadCharacterResult> = 0;
    };
}
