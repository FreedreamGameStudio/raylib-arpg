#include "AbilitySystem.hpp"
#include "abilities/WhirlwindAbility.hpp"
#include "ControllableActorSystem.hpp"

namespace sage
{
	void AbilitySystem::abilityOnePressed()
	{
		std::cout << "Ability 1 pressed \n";
		if (currentAbilities[0] == -1 || abilityMap[currentAbilities[0]]->cooldownTimer > 0.0f)
		{
			std::cout << "Waiting for cooldown timer: " << abilityMap[currentAbilities[0]]->cooldownTimer << "\n";
			return;
		}
		abilityMap[currentAbilities[0]]->Use(controllableActorSystem->GetControlledActor());
	}

	void AbilitySystem::abilityTwoPressed()
	{
		std::cout << "Ability 2 pressed \n";
		if (currentAbilities[1] == -1 || abilityMap[currentAbilities[1]]->cooldownTimer > 0.0f)
		{
			return;
		}
		abilityMap[currentAbilities[1]]->Use(controlledActor);
	}

	void AbilitySystem::abilityThreePressed()
	{
		std::cout << "Ability 3 pressed \n";
		if (currentAbilities[2] == -1 || abilityMap[currentAbilities[2]]->cooldownTimer > 0.0f)
		{
			return;
		}
		abilityMap[currentAbilities[2]]->Use(controlledActor);
	}

	void AbilitySystem::abilityFourPressed()
	{
		std::cout << "Ability 4 pressed \n";
		if (currentAbilities[3] == -1 || abilityMap[currentAbilities[3]]->cooldownTimer > 0.0f)
		{
			return;
		}
		abilityMap[currentAbilities[3]]->Use(controlledActor);
	}

	void AbilitySystem::onActorChanged()
	{
		controlledActor = controllableActorSystem->GetControlledActor();
		// Change abilities based on the new actor
	}

	void AbilitySystem::ChangeAbility(int abilitySlot, int newAbilityIndex)
	{
		// More than likely should subscribe to an "ability end" event which then triggers the change
		currentAbilities[abilitySlot] = newAbilityIndex;
	}

	void AbilitySystem::Update()
	{
		for (auto& ability : abilityMap)
		{
			ability->Update(controllableActorSystem->GetControlledActor());
		}
	}

	void AbilitySystem::Draw2D()
	{
		// Draw GUI here (cooldowns etc)
	}

	AbilitySystem::AbilitySystem(entt::registry* _registry, Cursor* _cursor, UserInput* _userInput,
			ActorMovementSystem* _actorMovementSystem, CollisionSystem* _collisionSystem,
			ControllableActorSystem* _controllableActorSystem, TimerManager* _timerManager)
			:
			registry(_registry), cursor(_cursor), userInput(_userInput),
			actorMovementSystem(_actorMovementSystem), collisionSystem(_collisionSystem),
			controllableActorSystem(_controllableActorSystem)
	{
		{
			entt::sink sink{ userInput->keyOnePressed };
			sink.connect<&AbilitySystem::abilityOnePressed>(this);
		}
		{
			entt::sink sink{ userInput->keyTwoPressed };
			sink.connect<&AbilitySystem::abilityTwoPressed>(this);
		}
		{
			entt::sink sink{ userInput->keyThreePressed };
			sink.connect<&AbilitySystem::abilityThreePressed>(this);
		}
		{
			entt::sink sink{ userInput->keyFourPressed };
			sink.connect<&AbilitySystem::abilityFourPressed>(this);
		}
		onActorChanged();
		{
			entt::sink sink{ _controllableActorSystem->onControlledActorChange };
			sink.connect<&AbilitySystem::onActorChanged>(this);
		}

		currentAbilities.fill(-1);
		abilityMap.push_back(std::make_unique<WhirlwindAbility>(registry, collisionSystem, _timerManager));
		ChangeAbility(0, 0);
	}
}