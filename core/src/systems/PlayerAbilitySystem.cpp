#include "PlayerAbilitySystem.hpp"

#include "AbilityRegistry.hpp"

#include "abilities/Abilities.hpp"
#include "GameData.hpp"
#include "systems/ControllableActorSystem.hpp"
#include "systems/states/AbilitySystem.hpp"
#include "UserInput.hpp"

#include <cassert>

namespace sage
{
    void PlayerAbilitySystem::abilityOnePressed()
    {
        std::cout << "Ability 1 pressed \n";

        auto& ab = registry->get<Ability>(abilitySlots[0]);

        if (!ab.CooldownReady())
        {
            std::cout << "Waiting for cooldown timer: " << ab.GetRemainingCooldownTime() << "\n";
            return;
        }
        gameData->abilityStateMachine->InitAbility(abilitySlots[0]);
    }

    void PlayerAbilitySystem::abilityTwoPressed()
    {
        std::cout << "Ability 2 pressed \n";

        auto& ab = registry->get<Ability>(abilitySlots[1]);

        if (!ab.CooldownReady())
        {
            std::cout << "Waiting for cooldown timer: " << ab.GetRemainingCooldownTime() << "\n";
            return;
        }
        gameData->abilityStateMachine->InitAbility(abilitySlots[1]);
    }

    void PlayerAbilitySystem::abilityThreePressed()
    {
        std::cout << "Ability 3 pressed \n";

        auto& ab = registry->get<Ability>(abilitySlots[2]);

        if (!ab.CooldownReady())
        {
            std::cout << "Waiting for cooldown timer: " << ab.GetRemainingCooldownTime() << "\n";
            return;
        }
        gameData->abilityStateMachine->InitAbility(abilitySlots[2]);
    }

    void PlayerAbilitySystem::abilityFourPressed()
    {
        std::cout << "Ability 4 pressed \n";

        auto& ab = registry->get<Ability>(abilitySlots[3]);

        if (!ab.CooldownReady())
        {
            std::cout << "Waiting for cooldown timer: " << ab.GetRemainingCooldownTime() << "\n";
            return;
        }
        gameData->abilityStateMachine->InitAbility(abilitySlots[3]);
    }

    void PlayerAbilitySystem::onActorChanged()
    {
        controlledActor = gameData->controllableActorSystem->GetControlledActor();
        // TODO: Change abilities based on the new actor
    }

    void PlayerAbilitySystem::SetSlot(int slot, entt::entity abilityEntity)
    {
        assert(slot < 4);
        assert(abilityEntity != entt::null);
        abilitySlots[slot] = abilityEntity;
    }

    void PlayerAbilitySystem::Update()
    {
    }

    void PlayerAbilitySystem::Draw2D()
    {
        // Draw GUI here (cooldowns etc)
    }

    void PlayerAbilitySystem::Draw3D()
    {
    }

    PlayerAbilitySystem::PlayerAbilitySystem(entt::registry* _registry, GameData* _gameData)
        : registry(_registry), gameData(_gameData)
    {
        onActorChanged();
        {
            entt::sink sink{gameData->userInput->keyOnePressed};
            sink.connect<&PlayerAbilitySystem::abilityOnePressed>(this);
        }
        {
            entt::sink sink{gameData->userInput->keyTwoPressed};
            sink.connect<&PlayerAbilitySystem::abilityTwoPressed>(this);
        }
        {
            entt::sink sink{gameData->userInput->keyThreePressed};
            sink.connect<&PlayerAbilitySystem::abilityThreePressed>(this);
        }
        {
            entt::sink sink{gameData->userInput->keyFourPressed};
            sink.connect<&PlayerAbilitySystem::abilityFourPressed>(this);
        }

        {
            entt::sink sink{gameData->controllableActorSystem->onControlledActorChange};
            sink.connect<&PlayerAbilitySystem::onActorChanged>(this);
        }
    }
} // namespace sage