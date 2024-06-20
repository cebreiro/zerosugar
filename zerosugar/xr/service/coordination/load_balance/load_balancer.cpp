#include "load_balancer.h"

#include <boost/scope/scope_exit.hpp>
#include "zerosugar/xr/service/coordination/coordination_service.h"
#include "zerosugar/xr/service/coordination/node/node_container.h"

namespace zerosugar::xr::coordination
{
    LoadBalancer::LoadBalancer(CoordinationService& coordinationService)
        : _coordinationService(coordinationService)
    {
    }

    LoadBalancer::~LoadBalancer()
    {
    }

    bool LoadBalancer::Add(const game_server_id_type& id)
    {
        constexpr ServerStatus defaultStatus{
            .loadCPUPercentage = std::numeric_limits<int32_t>::min(),
            .freePhysicalMemoryGB = std::numeric_limits<double>::min(),
        };

        if (_statuses.insert(std::make_pair(id, defaultStatus)).second)
        {
            Add(GetCategory(defaultStatus), id);

            return true;
        }

        return false;
    }

    bool LoadBalancer::Update(const game_server_id_type& id, const ServerStatus& status)
    {
        const auto iter = _statuses.find(id);
        if (iter == _statuses.end())
        {
            return false;
        }

        const StatusCategory oldCategory = GetCategory(iter->second);
        const StatusCategory newCategory = GetCategory(status);

        if (oldCategory != newCategory)
        {
            Remove(oldCategory, id);
            Add(newCategory, id);
        }

        iter->second = status;

        return true;
    }

    bool LoadBalancer::Remove(const game_server_id_type& id)
    {
        const auto iter = _statuses.find(id);
        if (iter == _statuses.end())
        {
            return false;
        }

        const auto category = GetCategory(iter->second);
        Remove(category, id);

        _statuses.erase(id);

        return true;
    }

    auto LoadBalancer::Select() -> GameServer*
    {
        NodeContainer& nodeContainer = _coordinationService.GetNodeContainer();

        for (const auto category : { StatusCategory::Healthy, StatusCategory::Moderate , StatusCategory::Busy })
        {
            auto& [vector, rrCount] = _categorized[static_cast<int32_t>(category)];
            if (vector.empty())
            {
                continue;
            }

            const game_server_id_type selectedId = vector[rrCount++ % std::ssize(vector)];

            return nodeContainer.Find(selectedId);
        }

        return nullptr;
    }

    void LoadBalancer::Add(StatusCategory category, game_server_id_type id)
    {
        auto& vector = _categorized[static_cast<int32_t>(category)].first;
        assert(!std::ranges::contains(vector, id));

        vector.push_back(id);
    }

    void LoadBalancer::Remove(StatusCategory category, game_server_id_type id)
    {
        auto& vector = _categorized[static_cast<int32_t>(category)].first;
        auto iter = std::ranges::find(vector, id);

        if (iter != vector.end())
        {
            vector.erase(iter);
        }
        else
        {
            assert(false);
        }
    }

    bool LoadBalancer::IsInvalid(const ServerStatus& status)
    {
        if (status.loadCPUPercentage < 0 || status.freePhysicalMemoryGB < 0.f)
        {
            return true;
        }

        return false;
    }

    auto LoadBalancer::GetCategory(const ServerStatus& status) -> StatusCategory
    {
        if (IsInvalid(status))
        {
            return StatusCategory::Unusable;
        }

        if (status.loadCPUPercentage < 20)
        {
            return StatusCategory::Healthy;
        }
        else if (status.loadCPUPercentage < 40)
        {
            return StatusCategory::Moderate;
        }

        return StatusCategory::Busy;
    }
}
