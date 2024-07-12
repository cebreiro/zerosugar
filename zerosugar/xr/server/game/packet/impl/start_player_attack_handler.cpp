#include "start_player_attack_handler.h"

#include <numbers>
#include <Eigen/Core>
#include <Eigen/Geometry>

#include "zerosugar/shared/collision/shape/obb.h"
#include "zerosugar/shared/network/session/session.h"
#include "zerosugar/xr/navigation/navigation_service.h"
#include "zerosugar/xr/network/model/generated/game_cs_message_json.h"
#include "zerosugar/xr/server/game/game_server.h"
#include "zerosugar/xr/server/game/client/game_client.h"
#include "zerosugar/xr/server/game/instance/game_instance.h"
#include "zerosugar/xr/server/game/instance/task/impl/player_attack.h"

namespace zerosugar::xr
{
    using network::game::cs::StartPlayerAttack;

    auto StartPlayerAttackHandler::HandlePacket(GameServer& server, Session& session,
        UniquePtrNotNull<StartPlayerAttack> packet) -> Future<void>
    {
        SharedPtrNotNull<GameClient> client = server.FindClient(session.GetId());
        if (!client)
        {
            session.Close();

            co_return;
        }

        const std::shared_ptr<GameInstance> instance = client->GetGameInstance();
        if (!instance)
        {
            session.Close();

            co_return;
        }

        if (auto navi = instance->GetNavigationService(); navi)
        {
            const auto& pos = packet->position;

            const Eigen::AlignedBox3d box(
                Eigen::Vector3d(0, -100, 0),
                Eigen::Vector3d(600.f, 100.f, 100.f));

            Eigen::AngleAxisd axis(packet->rotation.yaw * std::numbers::pi / 180.0, Eigen::Vector3d::UnitZ());

            const Eigen::Vector3d localCenter = box.center();
            const Eigen::Vector3d halfSize = box.sizes() * 0.5;
            const Eigen::Matrix3d rotation = axis.toRotationMatrix();

            Eigen::Vector3d center = rotation * localCenter;
            center.x() += pos.x;
            center.y() += pos.y;
            center.z() += pos.z;

            collision::OBB3d obb(center, halfSize, rotation);
            navi->DrawOBB(obb, std::chrono::milliseconds(2000));
        }

        auto task = std::make_unique<game_task::PlayerAttack>(std::move(packet), client->GetGameEntityId());

        instance->Summit(std::move(task), client->GetControllerId());
    }
}
