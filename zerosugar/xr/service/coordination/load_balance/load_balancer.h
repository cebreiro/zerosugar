#pragma once
#include <boost/container/flat_set.hpp>
#include <boost/unordered/unordered_flat_map.hpp>
#include "zerosugar/xr/service/coordination/load_balance/load_balancer_interface.h"
#include "zerosugar/xr/service/coordination/load_balance/server_status.h"

namespace zerosugar::xr
{
    class CoordinationService;
}

namespace zerosugar::xr::coordination
{
    class LoadBalancer final : public ILoadBalancer
    {
        enum class StatusCategory
        {
            Healthy = 0,
            Moderate,
            Busy,
            Invalid,

            Count,
        };

    public:
        explicit LoadBalancer(CoordinationService& coordinationService);
        ~LoadBalancer();

        bool Add(const game_server_id_type& id) override;
        bool Update(const game_server_id_type& id, const ServerStatus& status) override;
        bool Remove(const game_server_id_type& id) override;

        auto Select() -> GameServer* override;

    private:
        void Add(StatusCategory category, game_server_id_type id);
        void Remove(StatusCategory category, game_server_id_type id);

        static bool IsInvalid(const ServerStatus& status);
        static auto GetCategory(const ServerStatus& status) -> StatusCategory;

    private:
        CoordinationService& _coordinationService;

        boost::unordered::unordered_flat_map<game_server_id_type, ServerStatus> _statuses;
        std::array<std::pair<std::vector<game_server_id_type>, int64_t>, static_cast<int32_t>(StatusCategory::Count)> _categorized;
    };
}
