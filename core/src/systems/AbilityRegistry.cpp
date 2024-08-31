#include "AbilityRegistry.hpp"

#include "abilities/AbilityFactory.hpp"
#include "GameData.hpp"
#include "systems/states/AbilityStateMachine.hpp"

namespace sage
{

    entt::entity AbilityRegistry::GetAbility(entt::entity entity, AbilityEnum abilityEnum)
    {
        if (!abilityMap.contains(entity))
        {
            return entt::null;
        }

        if (abilityMap[entity].contains(abilityEnum))
        {
            return abilityMap[entity][abilityEnum];
        }

        return entt::null;
    }

    entt::entity AbilityRegistry::RegisterAbility(entt::entity entity, AbilityEnum abilityEnum)
    {
        entt::entity out = entt::null;

        switch (abilityEnum)
        {
        case AbilityEnum::PLAYER_AUTOATTACK:
            out = CreatePlayerAutoAttack(registry, entity, gameData);
            break;
        case AbilityEnum::ENEMY_AUTOATTACK:
            out = CreateWavemobAutoAttackAbility(registry, entity, gameData);
            break;
        case AbilityEnum::FIREBALL:
            out = CreateFireballAbility(registry, entity, gameData);
            break;
        case AbilityEnum::LIGHTNINGBALL:
            out = CreateLightningBallAbility(registry, entity, gameData);
            break;
        case AbilityEnum::RAINFOFIRE:
            out = CreateRainOfFireAbility(registry, entity, gameData);
            break;
        case AbilityEnum::WHIRLWIND:
            out = CreateWhirlwindAbility(registry, entity, gameData);
            break;
        default:
            break;
        }

        assert(registry->any_of<AbilityState>(out));

        if (out != entt::null)
        {
            abilityMap[entity].emplace(abilityEnum, out);
        }

        return out;
    }

    AbilityRegistry::AbilityRegistry(entt::registry* _registry, GameData* _gameData)
        : registry(_registry), gameData(_gameData)
    {
    }
} // namespace sage