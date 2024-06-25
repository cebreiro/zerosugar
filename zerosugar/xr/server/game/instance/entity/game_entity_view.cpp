#include "game_entity_view.h"

namespace zerosugar::xr
{
    GameEntityView::GameEntityView(int64_t id)
        : _id(id)
    {
    }

    auto GameEntityView::GetId() const -> game_entity_id_type
    {
        return _id;
    }

    auto GameEntityView::GetPosition() const -> const Eigen::Vector3d&
    {
        return _position;
    }

    void GameEntityView::SetPosition(const Eigen::Vector3d& position)
    {
        _position = position;
    }
}
