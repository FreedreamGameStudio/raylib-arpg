//
// Created by steve on 03/08/2024.
//

#pragma once

#include <entt/entt.hpp>

#include "components/CombatableActor.hpp"

namespace sage
{

	class CombatSystem
	{
		entt::registry* registry;
		// Have a central combat system instead of handling damage etc in individual systems.
		// Can have callbacks for certain types of damage being inflicted so that other systems can react and modify it (chain effects etc).
		void onComponentAdded(entt::entity entity);
		void onComponentRemoved(entt::entity entity);
	public:
		void RegisterAttack(entt::entity attacker, entt::entity target, AttackData attackData);
		explicit CombatSystem(entt::registry* _registry);
	};

} // sage
