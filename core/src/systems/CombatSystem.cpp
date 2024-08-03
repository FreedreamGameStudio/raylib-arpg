//
// Created by steve on 03/08/2024.
//

#include "CombatSystem.hpp"
#include "components/HealthBar.hpp"

namespace sage
{
	
	void CombatSystem::onComponentAdded(entt::entity entity)
	{
		auto& c = registry->get<CombatableActor>(entity);
		{
			entt::sink sink { c.onHit };
			sink.connect<&CombatSystem::RegisterAttack>(*this);
		}
	}

	void CombatSystem::onComponentRemoved(entt::entity entity)
	{
	}

	void CombatSystem::RegisterAttack(entt::entity hit, entt::entity attacker, AttackData attackData)
	{
		// Register the attack with the target.
		// This could be used to apply damage, status effects, etc.
		auto& targetCombat = registry->get<CombatableActor>(hit);
		targetCombat.hp -= attackData.damage;
		if (targetCombat.hp <= 0)
		{
			targetCombat.hp = 0;
			targetCombat.target = entt::null;
			targetCombat.onDeath.publish(hit);
		}
		if (registry->any_of<HealthBar>(hit))
		{
			auto& healthbar = registry->get<HealthBar>(hit);
			healthbar.Decrement(attackData.damage);
		}
	}

	CombatSystem::CombatSystem(entt::registry* _registry) : registry(_registry)
	{
		registry->on_construct<CombatableActor>().connect<&CombatSystem::onComponentAdded>(this);
		registry->on_destroy<CombatableActor>().connect<&CombatSystem::onComponentRemoved>(this);
	}
} // sage