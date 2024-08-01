// Created by Steve Wheeler on 30/06/2024.

#pragma once

#include "entt/entt.hpp"

#include "systems/state_systems/StateMachineSystem.hpp"
#include "systems/ActorMovementSystem.hpp"
#include "components/states/StatePlayerDefault.hpp"

namespace sage
{
	class PlayerDefaultStateSystem : public StateMachineSystem<PlayerDefaultStateSystem, StatePlayerDefault>
	{
		ActorMovementSystem* actorMovementSystem;

	public:
		void Update() override;
		void Draw3D() override;
		void OnStateEnter(entt::entity entity) override;
		void OnStateExit(entt::entity entity) override;
		PlayerDefaultStateSystem(entt::registry* _registry,
			ActorMovementSystem* _actorMovementSystem);
	};
} // sage
