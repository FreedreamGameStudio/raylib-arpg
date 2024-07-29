#pragma once

#include <entt/entt.hpp>

#include "UserInput.hpp"
#include "Cursor.hpp"
#include "ActorMovementSystem.hpp"
#include "CollisionSystem.hpp"
#include "ControllableActorSystem.hpp"
#include "../abilities/Ability.hpp"

#include <memory>
#include <array>
#include <vector>

namespace sage
{
	class AbilitySystem // PlayerAbilitySystem (ControllableActorAbilitySystem?)
	{
		entt::registry* registry;
		entt::entity controlledActor;
		Cursor* cursor;
		UserInput* userInput;
		ActorMovementSystem* actorMovementSystem;
		CollisionSystem* collisionSystem;
		ControllableActorSystem* controllableActorSystem;
		std::vector<std::unique_ptr<Ability>> abilityMap;
		std::array<int, 4> currentAbilities{};
		void abilityOnePressed();
		void abilityTwoPressed();
		void abilityThreePressed();
		void abilityFourPressed();
		void onActorChanged();
	public:
		void ChangeAbility(int abilitySlot, int newAbilityIndex);
		void Update();
		void Draw2D();
		void Draw3D();
		AbilitySystem(entt::registry* _registry, Camera* _camera, Cursor* _cursor, UserInput* _userInput,
				ActorMovementSystem* _actorMovementSystem, CollisionSystem* _collisionSystem,
				ControllableActorSystem* _controllableActorSystem, NavigationGridSystem* _navigationGridSystem,
				TimerManager* _timerManager);

	};
}