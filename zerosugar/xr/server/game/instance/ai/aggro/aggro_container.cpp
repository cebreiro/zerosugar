#include "aggro_container.h"

#include "zerosugar/xr/network/model/generated/game_sc_message.h"
#include "zerosugar/xr/server/game/instance/game_instance.h"
#include "zerosugar/xr/server/game/instance/ai/ai_controller.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_container.h"

namespace zerosugar::xr::ai
{
    AggroContainer::AggroContainer(AIController& aiController)
        : _aiController(aiController)
    {
    }

    void AggroContainer::Update()
    {
        if (_descSortedArray.empty())
        {
            return;
        }

        GameSnapshotContainer& snapshotContainer = _aiController.GetGameInstance().GetSnapshotContainer();

        std::erase_if(_descSortedArray, [&snapshotContainer](const Aggro& aggro)
            {
                const GamePlayerSnapshot* player = snapshotContainer.FindPlayer(aggro.GetSourceId());
                if (!player)
                {
                    return true;
                }

                // TODO: check player death 

                return false;
            });
    }

    bool AggroContainer::Empty() const
    {
        return _descSortedArray.empty();
    }

    void AggroContainer::Add(game_entity_id_type id, int64_t value)
    {
        if (value == 0)
        {
            assert(false);

            return;
        }

        const auto iter = std::ranges::find_if(_descSortedArray, [id](const Aggro& aggro)
            {
                return id == aggro.GetSourceId();
            });
        if (iter != _descSortedArray.end())
        {
            iter->Add(value);

            if (value > 0)
            {
                auto next = iter;
                for (auto prev = std::prev(next); prev != _descSortedArray.end(); next = prev--)
                {
                    if (CompareAggroValue(*next, *prev))
                    {
                        std::iter_swap(next, prev);
                    }
                    else
                    {
                        break;
                    }
                }
            }
            else
            {
                auto prev = iter;
                for (auto next = std::next(prev); next != _descSortedArray.end(); prev = next++)
                {
                    if (CompareAggroValue(*prev, *next))
                    {
                        std::iter_swap(prev, next);
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
        else
        {
            if (std::ssize(_descSortedArray) == max_aggro_manage_count)
            {
                _descSortedArray.pop_back();
            }

            const Aggro aggro(id, value);

            const auto pos = std::ranges::lower_bound(_descSortedArray, aggro, CompareAggroValue);
            _descSortedArray.insert(pos, aggro);
        }
    }

    void AggroContainer::Remove(game_entity_id_type id)
    {
        const auto iter = std::ranges::find_if(_descSortedArray, [id](const Aggro& aggro)
            {
                return id == aggro.GetSourceId();
            });
        if (iter != _descSortedArray.end())
        {
            _descSortedArray.erase(iter);
        }
    }

    auto AggroContainer::GetPrimaryTarget() const -> GamePlayerSnapshot*
    {
        if (_descSortedArray.empty())
        {
            return nullptr;
        }

        return _aiController.GetGameInstance().GetSnapshotContainer().FindPlayer(_descSortedArray.begin()->GetSourceId());
    }

    auto AggroContainer::SelectPrimaryTarget() -> GamePlayerSnapshot*
    {
        GameSnapshotContainer& snapshotContainer = _aiController.GetGameInstance().GetSnapshotContainer();
        GamePlayerSnapshot* player = nullptr;

        auto iter = _descSortedArray.begin();
        while (iter != _descSortedArray.end())
        {
            player = snapshotContainer.FindPlayer(iter->GetSourceId());
            if (player)
            {
                break;
            }

            ++iter;
        }

        if (iter != _descSortedArray.begin())
        {
            _descSortedArray.erase(_descSortedArray.begin(), iter);
        }

        return player;
    }

    bool AggroContainer::CompareAggroValue(const Aggro& lhs, const Aggro& rhs)
    {
        return lhs.GetValue() > rhs.GetValue();
    }
}
