#include "game_message.h"

namespace zerosugar::xr::network::game
{
    void Position::Deserialize(BufferReader& reader)
    {
        x = reader.Read<float>();
        y = reader.Read<float>();
        z = reader.Read<float>();
    }

    void Position::Serialize(BufferWriter& writer) const
    {
        writer.Write<float>(x);
        writer.Write<float>(y);
        writer.Write<float>(z);
    }

    void Rotation::Deserialize(BufferReader& reader)
    {
        pitch = reader.Read<float>();
        yaw = reader.Read<float>();
        roll = reader.Read<float>();
    }

    void Rotation::Serialize(BufferWriter& writer) const
    {
        writer.Write<float>(pitch);
        writer.Write<float>(yaw);
        writer.Write<float>(roll);
    }

    void Transform::Deserialize(BufferReader& reader)
    {
        reader.Read(position);
        reader.Read(rotation);
    }

    void Transform::Serialize(BufferWriter& writer) const
    {
        writer.Write(position);
        writer.Write(rotation);
    }

    void PlayerBase::Deserialize(BufferReader& reader)
    {
        hp = reader.Read<float>();
        maxHp = reader.Read<float>();
        attackMin = reader.Read<float>();
        attackMax = reader.Read<float>();
        attackRange = reader.Read<float>();
        attackSpeed = reader.Read<float>();
        speed = reader.Read<float>();
        defence = reader.Read<float>();
        name = reader.ReadString(reader.Read<int16_t>());
        level = reader.Read<int32_t>();
        gender = reader.Read<int32_t>();
        face = reader.Read<int32_t>();
        hair = reader.Read<int32_t>();
        str = reader.Read<int32_t>();
        dex = reader.Read<int32_t>();
        intell = reader.Read<int32_t>();
        stamina = reader.Read<float>();
        staminaMax = reader.Read<float>();
    }

    void PlayerBase::Serialize(BufferWriter& writer) const
    {
        writer.Write<float>(hp);
        writer.Write<float>(maxHp);
        writer.Write<float>(attackMin);
        writer.Write<float>(attackMax);
        writer.Write<float>(attackRange);
        writer.Write<float>(attackSpeed);
        writer.Write<float>(speed);
        writer.Write<float>(defence);
        writer.Write<int16_t>((int16_t)std::ssize(name) + 1);
        writer.WriteString(name);
        writer.Write<int32_t>(level);
        writer.Write<int32_t>(gender);
        writer.Write<int32_t>(face);
        writer.Write<int32_t>(hair);
        writer.Write<int32_t>(str);
        writer.Write<int32_t>(dex);
        writer.Write<int32_t>(intell);
        writer.Write<float>(stamina);
        writer.Write<float>(staminaMax);
    }

    void Equipment::Deserialize(BufferReader& reader)
    {
        id = reader.Read<int32_t>();
        type = reader.Read<int32_t>();
        count = reader.Read<int32_t>();
        attack = reader.Read<int32_t>();
        defence = reader.Read<int32_t>();
        str = reader.Read<int32_t>();
        dex = reader.Read<int32_t>();
        intell = reader.Read<int32_t>();
    }

    void Equipment::Serialize(BufferWriter& writer) const
    {
        writer.Write<int32_t>(id);
        writer.Write<int32_t>(type);
        writer.Write<int32_t>(count);
        writer.Write<int32_t>(attack);
        writer.Write<int32_t>(defence);
        writer.Write<int32_t>(str);
        writer.Write<int32_t>(dex);
        writer.Write<int32_t>(intell);
    }

    void PlayerEquipment::Deserialize(BufferReader& reader)
    {
        reader.Read(armor);
        reader.Read(gloves);
        reader.Read(shoes);
        reader.Read(weapon);
    }

    void PlayerEquipment::Serialize(BufferWriter& writer) const
    {
        writer.Write(armor);
        writer.Write(gloves);
        writer.Write(shoes);
        writer.Write(weapon);
    }

    void PlayerQuickSlot::Deserialize(BufferReader& reader)
    {
        index = reader.Read<int32_t>();
        type = reader.Read<int32_t>();
        id = reader.Read<int32_t>();
    }

    void PlayerQuickSlot::Serialize(BufferWriter& writer) const
    {
        writer.Write<int32_t>(index);
        writer.Write<int32_t>(type);
        writer.Write<int32_t>(id);
    }

    void PlayerSkill::Deserialize(BufferReader& reader)
    {
        id = reader.Read<int32_t>();
    }

    void PlayerSkill::Serialize(BufferWriter& writer) const
    {
        writer.Write<int32_t>(id);
    }

    void PlayerInventoryItem::Deserialize(BufferReader& reader)
    {
        slot = reader.Read<int32_t>();
        id = reader.Read<int32_t>();
        count = reader.Read<int32_t>();
        attack = reader.Read<int32_t>();
        defence = reader.Read<int32_t>();
        str = reader.Read<int32_t>();
        dex = reader.Read<int32_t>();
        intell = reader.Read<int32_t>();
    }

    void PlayerInventoryItem::Serialize(BufferWriter& writer) const
    {
        writer.Write<int32_t>(slot);
        writer.Write<int32_t>(id);
        writer.Write<int32_t>(count);
        writer.Write<int32_t>(attack);
        writer.Write<int32_t>(defence);
        writer.Write<int32_t>(str);
        writer.Write<int32_t>(dex);
        writer.Write<int32_t>(intell);
    }

    void RemotePlayer::Deserialize(BufferReader& reader)
    {
        id = reader.Read<int64_t>();
        reader.Read(transform);
        reader.Read(base);
        reader.Read(equipment);
    }

    void RemotePlayer::Serialize(BufferWriter& writer) const
    {
        writer.Write<int64_t>(id);
        writer.Write(transform);
        writer.Write(base);
        writer.Write(equipment);
    }

    void Player::Deserialize(BufferReader& reader)
    {
        id = reader.Read<int64_t>();
        reader.Read(transform);
        reader.Read(base);
        reader.Read(equipment);
        gold = reader.Read<int32_t>();
        itemsCount = reader.Read<int32_t>();
        reader.Read(items, itemsCount);
        quickSlotCount = reader.Read<int32_t>();
        reader.Read(quickSlots, quickSlotCount);
        skillsCount = reader.Read<int32_t>();
        reader.Read(skills, skillsCount);
        exp = reader.Read<int32_t>();
        expMax = reader.Read<int32_t>();
    }

    void Player::Serialize(BufferWriter& writer) const
    {
        writer.Write<int64_t>(id);
        writer.Write(transform);
        writer.Write(base);
        writer.Write(equipment);
        writer.Write<int32_t>(gold);
        writer.Write<int32_t>(itemsCount);
        writer.Write(items);
        writer.Write<int32_t>(quickSlotCount);
        writer.Write(quickSlots);
        writer.Write<int32_t>(skillsCount);
        writer.Write(skills);
        writer.Write<int32_t>(exp);
        writer.Write<int32_t>(expMax);
    }

    void Monster::Deserialize(BufferReader& reader)
    {
        dataId = reader.Read<int32_t>();
        id = reader.Read<int64_t>();
        reader.Read(transform);
        hp = reader.Read<float>();
        maxHp = reader.Read<float>();
        attackMin = reader.Read<float>();
        attackMax = reader.Read<float>();
        attackRange = reader.Read<float>();
        attackSpeed = reader.Read<float>();
        speed = reader.Read<float>();
        defence = reader.Read<float>();
    }

    void Monster::Serialize(BufferWriter& writer) const
    {
        writer.Write<int32_t>(dataId);
        writer.Write<int64_t>(id);
        writer.Write(transform);
        writer.Write<float>(hp);
        writer.Write<float>(maxHp);
        writer.Write<float>(attackMin);
        writer.Write<float>(attackMax);
        writer.Write<float>(attackRange);
        writer.Write<float>(attackSpeed);
        writer.Write<float>(speed);
        writer.Write<float>(defence);
    }

}
