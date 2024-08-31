#include "AbilityFactory.hpp"

#include "AbilityFunctions.hpp"
#include "AbilityIndicator.hpp"
#include "AbilityResourceManager.hpp"

#include "components/Ability.hpp"
#include "GameData.hpp"
#include "vfx/VisualFX.hpp"

#include <memory>

#include <Serializer.hpp>

namespace sage
{
    // TODO: Can rename file to AbilityFactory, or just move to GameObjectFactory

    entt::entity CreatePlayerAutoAttack(entt::registry* registry, entt::entity caster, GameData* gameData)
    {
        AbilityData ad;
        ad.base.element = AttackElement::PHYSICAL;
        ad.base.cooldownDuration = 1;
        ad.base.baseDamage = 10;
        ad.base.range = 5;
        ad.base.repeatable = true;
        ad.base.behaviourOnHit = AbilityBehaviourOnHit::HIT_TARGETED_UNIT;
        ad.base.behaviourPreHit = AbilityBehaviourPreHit::FOLLOW_CASTER;
        ad.base.spawnBehaviour = AbilitySpawnBehaviour::AT_CASTER;

        ad.animationParams.animEnum = AnimationEnum::AUTOATTACK;
        ad.animationParams.animSpeed = 4;
        ad.animationParams.animationDelay = 0;

        auto entity = registry->create();
        auto& ability = registry->emplace<Ability>(entity);
        ability.self = entity;
        ability.caster = caster;
        ability.ad = ad;
        ability.cooldownTimer.SetMaxTime(ad.base.cooldownDuration);
        ability.executionDelayTimer.SetMaxTime(ad.animationParams.animationDelay);

        // ability.abilityIndicator = AbilityResourceManager::GetInstance().GetIndicator(ad.indicator, gameData);

        // Would much prefer emplacing the vfx with the above entity id, instead.
        ability.vfx = AbilityResourceManager::GetInstance().GetVisualFX(ad.vfx, entity, gameData);

        return entity;
    }

    // RainOfFire factory function
    entt::entity CreateRainOfFireAbility(entt::registry* registry, entt::entity caster, GameData* gameData)
    {
        AbilityData ad;

        ad.base.cooldownDuration = 3;
        ad.base.range = 15;
        ad.base.baseDamage = 25;
        ad.base.element = AttackElement::FIRE;
        ad.base.repeatable = false;
        ad.base.spawnBehaviour = AbilitySpawnBehaviour::AT_CURSOR;
        ad.base.behaviourOnHit = AbilityBehaviourOnHit::HIT_ALL_IN_RADIUS;
        ad.base.behaviourPreHit = AbilityBehaviourPreHit::DETACHED_STATIONARY;

        ad.animationParams.animEnum = AnimationEnum::SPIN;
        ad.animationParams.animSpeed = 1;
        ad.animationParams.oneShot = true;
        ad.animationParams.animationDelay = 0.75f;

        ad.vfx.name = "RainOfFire";
        ad.indicator.indicatorKey = "CircularCursor";
        ad.cursorBased = true;

        ad.cursor = gameData->cursor.get();

        auto entity = registry->create();

        auto& ability = registry->emplace<Ability>(entity);
        ability.self = entity;
        ability.caster = caster;
        ability.ad = ad;
        ability.cooldownTimer.SetMaxTime(ad.base.cooldownDuration);
        ability.executionDelayTimer.SetMaxTime(ad.animationParams.animationDelay);

        ability.vfx = AbilityResourceManager::GetInstance().GetVisualFX(ad.vfx, entity, gameData);
        ability.abilityIndicator = AbilityResourceManager::GetInstance().GetIndicator(ad.indicator, gameData);

        return entity;
    }

    // FloorFire factory function
    entt::entity CreateFloorFireAbility(entt::registry* registry, entt::entity caster, GameData* gameData)
    {
        AbilityData ad;

        ad.base.cooldownDuration = 3;
        ad.base.range = 5;
        ad.base.baseDamage = 25;
        ad.base.radius = 30;
        ad.base.element = AttackElement::FIRE;
        ad.base.repeatable = false;
        ad.base.spawnBehaviour = AbilitySpawnBehaviour::AT_CURSOR;
        ad.base.behaviourOnHit = AbilityBehaviourOnHit::HIT_ALL_IN_RADIUS;
        ad.base.behaviourPreHit = AbilityBehaviourPreHit::DETACHED_STATIONARY;

        ad.animationParams.animEnum = AnimationEnum::SPIN;
        ad.animationParams.animSpeed = 1;
        ad.animationParams.oneShot = true;
        ad.animationParams.animationDelay = 0.75f;

        ad.vfx.name = "Fireball";
        ad.indicator.indicatorKey = "CircularCursor";

        ad.cursor = gameData->cursor.get();
        ad.cursorBased = true;

        auto entity = registry->create();

        auto& ability = registry->emplace<Ability>(entity);
        ability.self = entity;
        ability.caster = caster;
        ability.ad = ad;
        ability.cooldownTimer.SetMaxTime(ad.base.cooldownDuration);
        ability.executionDelayTimer.SetMaxTime(ad.animationParams.animationDelay);

        ability.vfx = AbilityResourceManager::GetInstance().GetVisualFX(ad.vfx, entity, gameData);

        return entity;
    }

    // Fireball factory function
    entt::entity CreateFireballAbility(entt::registry* registry, entt::entity caster, GameData* gameData)
    {
        AbilityData ad;

        ad.base.cooldownDuration = 1;
        ad.base.range = 30;
        ad.base.baseDamage = 50;
        ad.base.radius = 10;
        ad.base.element = AttackElement::PHYSICAL;
        ad.base.repeatable = false;
        ad.base.spawnBehaviour = AbilitySpawnBehaviour::AT_CASTER;
        ad.base.behaviourOnHit = AbilityBehaviourOnHit::HIT_ALL_IN_RADIUS;
        ad.base.behaviourPreHit = AbilityBehaviourPreHit::DETACHED_PROJECTILE;

        ad.animationParams.animEnum = AnimationEnum::AUTOATTACK;
        ad.animationParams.animationDelay = 0;

        ad.vfx.name = "Fireball";

        auto entity = registry->create();

        auto& ability = registry->emplace<Ability>(entity);
        ability.self = entity;
        ability.caster = caster;
        ability.ad = ad;
        ability.cooldownTimer.SetMaxTime(ad.base.cooldownDuration);
        ability.executionDelayTimer.SetMaxTime(ad.animationParams.animationDelay);

        ability.vfx = AbilityResourceManager::GetInstance().GetVisualFX(ad.vfx, entity, gameData);

        return entity;
    }

    // LightningBall factory function
    entt::entity CreateLightningBallAbility(entt::registry* registry, entt::entity caster, GameData* gameData)
    {
        AbilityData ad;

        ad.base.cooldownDuration = 1;
        ad.base.range = 5;
        ad.base.baseDamage = 10;
        ad.base.element = AttackElement::PHYSICAL;
        ad.base.repeatable = false;
        ad.base.spawnBehaviour = AbilitySpawnBehaviour::AT_CASTER;
        ad.base.behaviourOnHit = AbilityBehaviourOnHit::HIT_ALL_IN_RADIUS;
        ad.base.behaviourPreHit = AbilityBehaviourPreHit::DETACHED_PROJECTILE;

        ad.animationParams.animEnum = AnimationEnum::AUTOATTACK;

        ad.vfx.name = "LightningBall";

        auto entity = registry->create();

        auto& ability = registry->emplace<Ability>(entity);
        ability.self = entity;
        ability.caster = caster;
        ability.ad = ad;
        ability.cooldownTimer.SetMaxTime(ad.base.cooldownDuration);
        ability.executionDelayTimer.SetMaxTime(ad.animationParams.animationDelay);

        ability.vfx = AbilityResourceManager::GetInstance().GetVisualFX(ad.vfx, entity, gameData);

        return entity;
    }

    // WavemobAutoAttack factory function
    entt::entity CreateWavemobAutoAttackAbility(entt::registry* registry, entt::entity caster, GameData* gameData)
    {
        AbilityData ad;

        ad.base.cooldownDuration = 1;
        ad.base.range = 5;
        ad.base.baseDamage = 10;
        ad.base.element = AttackElement::PHYSICAL;
        ad.base.repeatable = true;
        ad.base.spawnBehaviour = AbilitySpawnBehaviour::AT_CASTER;
        ad.base.behaviourOnHit = AbilityBehaviourOnHit::HIT_TARGETED_UNIT;
        ad.base.behaviourPreHit = AbilityBehaviourPreHit::FOLLOW_CASTER;

        ad.animationParams.animEnum = AnimationEnum::AUTOATTACK;

        auto entity = registry->create();

        auto& ability = registry->emplace<Ability>(entity);
        ability.self = entity;
        ability.caster = caster;
        ability.ad = ad;
        ability.cooldownTimer.SetMaxTime(ad.base.cooldownDuration);
        ability.executionDelayTimer.SetMaxTime(ad.animationParams.animationDelay);

        ability.vfx = AbilityResourceManager::GetInstance().GetVisualFX(ad.vfx, entity, gameData);

        return entity;
    }

    // WhirlwindAbility factory function
    entt::entity CreateWhirlwindAbility(entt::registry* registry, entt::entity caster, GameData* gameData)
    {
        AbilityData ad;

        ad.base.cooldownDuration = 0.15;
        ad.base.range = 15;
        ad.base.baseDamage = 10;
        ad.base.radius = 15;
        ad.base.element = AttackElement::PHYSICAL;
        ad.base.repeatable = false;
        ad.base.spawnBehaviour = AbilitySpawnBehaviour::AT_CASTER;
        ad.base.behaviourOnHit = AbilityBehaviourOnHit::HIT_ALL_IN_RADIUS;
        ad.base.behaviourPreHit = AbilityBehaviourPreHit::FOLLOW_CASTER;

        ad.animationParams.animEnum = AnimationEnum::SPIN;
        ad.animationParams.animSpeed = 5;
        ad.animationParams.oneShot = true;
        ad.animationParams.animationDelay = 0;

        ad.vfx.name = "360SwordSlash";

        auto entity = registry->create();

        auto& ability = registry->emplace<Ability>(entity);
        ability.self = entity;
        ability.caster = caster;
        ability.ad = ad;
        ability.cooldownTimer.SetMaxTime(ad.base.cooldownDuration);
        ability.executionDelayTimer.SetMaxTime(ad.animationParams.animationDelay);

        ability.vfx = AbilityResourceManager::GetInstance().GetVisualFX(ad.vfx, entity, gameData);

        return entity;
    }

} // namespace sage