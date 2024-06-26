#include "player_component.h"

#include "zerosugar/xr/service/model/generated/data_transfer_object_message.h"

namespace zerosugar::xr
{
    PlayerComponent::PlayerComponent(const service::DTOCharacter& dto)
        : _characterId(dto.characterId)
        , _name(dto.name)
        , _level(dto.level)
        , _job(dto.job)
        , _faceId(dto.faceId)
        , _hairId(dto.hairId)
    {
    }

    auto PlayerComponent::GetCharacterId() const -> int64_t
    {
        return _characterId;
    }

    auto PlayerComponent::GetName() const -> const std::string&
    {
        return _name;
    }

    auto PlayerComponent::GetLevel() const -> int32_t
    {
        return _level;
    }

    auto PlayerComponent::GetJob() const -> int32_t
    {
        return _job;
    }

    auto PlayerComponent::GetFaceId() const -> int32_t
    {
        return _faceId;
    }

    auto PlayerComponent::GetHairId() const -> int32_t
    {
        return _hairId;
    }
}
