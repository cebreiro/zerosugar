#pragma once
#include <vector>
#include "zerosugar/shared/execution/future/future.h"
#include "zerosugar/sl/database/model/account.h"

namespace zerosugar::sl
{
    class IAccountRepository
    {
    public:
        IAccountRepository(const IAccountRepository& other) = delete;
        IAccountRepository(IAccountRepository&& other) noexcept = delete;
        IAccountRepository& operator=(const IAccountRepository& other) = delete;
        IAccountRepository& operator=(IAccountRepository&& other) noexcept = delete;

        IAccountRepository() = default;
        virtual ~IAccountRepository() = default;

    public:
        virtual auto Add(db::Account account) -> Future<bool> = 0;
        virtual auto Remove(int64_t accountId) -> Future<bool> = 0;
        virtual auto Update(db::Account account) -> Future<bool> = 0;

        virtual auto Find(int64_t id) const -> Future<std::optional<db::Account>> = 0;
        virtual auto Find(std::string account) const -> Future<std::optional<db::Account>> = 0;
    };
}
