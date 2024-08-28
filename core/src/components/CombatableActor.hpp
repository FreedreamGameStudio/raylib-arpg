//
// Created by Steve Wheeler on 04/06/2024.
//

#pragma once

#include <entt/entt.hpp>
#include <memory>
#include <vector>

namespace sage
{

    enum class CombatableActorType
    {
        PLAYER,
        WAVEMOB
    };

    enum class AttackElement
    {
        PHYSICAL,
        FIRE,
        ICE,
        LIGHTNING,
        POISON
    };

    struct AttackData
    {
        const entt::entity attacker;
        const entt::entity hit;
        const int damage;
        const AttackElement element = AttackElement::PHYSICAL;
    };

    struct CombatData
    {
        int hp = 100;
        int maxHp = 100;
        CombatData(const CombatData&) = delete;
        CombatData& operator=(const CombatData&) = delete;
        CombatData() = default;
    };

    class CombatableActor
    {

      public:
        // int hp = 100;
        CombatData data;
        CombatableActorType actorType = CombatableActorType::WAVEMOB;
        bool dying = false;
        entt::entity target{};
        int attackRange = 5; // TODO: each ability has its own range

        entt::sigh<void(AttackData)> onHit{}; // Self, attacker, damage
        entt::sigh<void(entt::entity)> onDeath{};
        entt::sigh<void(entt::entity, entt::entity)> onAttackCancelled{}; // Self, object clicked (can discard)
        entt::sigh<void(entt::entity, entt::entity)> onTargetDeath{};     // Self, target (that died)

        CombatableActor(const CombatableActor&) = delete;
        CombatableActor& operator=(const CombatableActor&) = delete;
        CombatableActor() = default;
    };
} // namespace sage
