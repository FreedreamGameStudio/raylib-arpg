//
// Created by Steve Wheeler on 04/06/2024.
//

#pragma once

#include <entt/entt.hpp>

#include "abilities/AbilityData.hpp"

#include <Event.hpp>

namespace sage
{
    static constexpr int MAX_ABILITY_NUMBER = 10;
    enum class CombatableActorType
    {
        PLAYER,
        WAVEMOB
    };

    struct AttackData
    {
        const entt::entity attacker{};
        const entt::entity hit{};
        const int damage{};
        const AbilityElement elements = AbilityElement::PHYSICAL;
    };

    struct CharacterStatistics
    {
        int agility = 5;
        int strength = 5;
        int intelligence = 5;
        int constitution = 5;
        int wits = 5;
        int memory = 5;
    };

    // BaseStatistics + Weapon stats etc?
    struct CombatData
    {
        int hp = 100;
        int maxHp = 100; // Move to baseStatistics
        CombatData(const CombatData&) = delete;
        CombatData& operator=(const CombatData&) = delete;
        CombatData() = default;
    };

    class CombatableActor
    {

      public:
        // Abilities are stored how they appear in the UI
        std::array<entt::entity, MAX_ABILITY_NUMBER> abilities{};
        CharacterStatistics baseStatistics;
        CombatData data;
        CombatableActorType actorType = CombatableActorType::WAVEMOB;
        bool dying = false;
        entt::entity target{};
        int attackRange = 5; // TODO: each ability has its own range

        std::unique_ptr<Event<AttackData>> onHit{}; // Self, attacker, damage
        std::unique_ptr<Event<entt::entity>> onDeath{};
        std::unique_ptr<Event<entt::entity, entt::entity>>
            onAttackCancelled{}; // Self, object clicked (can discard)
        Connection<entt::entity, entt::entity> onTargetDeathCnx;
        std::unique_ptr<Event<entt::entity, entt::entity>> onTargetDeath{}; // Self, target (that died)

        CombatableActor(const CombatableActor&) = delete;
        CombatableActor& operator=(const CombatableActor&) = delete;
        CombatableActor()
            : onHit(std::make_unique<Event<AttackData>>()),
              onDeath(std::make_unique<Event<entt::entity>>()),
              onAttackCancelled(std::make_unique<Event<entt::entity, entt::entity>>()),
              onTargetDeath(std::make_unique<Event<entt::entity, entt::entity>>())
        {
            for (unsigned int i = 0; i < MAX_ABILITY_NUMBER; ++i)
            {
                abilities[i] = entt::null;
                // TODO: Not sure if this is necessary.
            }
        }
    };
} // namespace sage
