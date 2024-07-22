#pragma once

#include <entt/entt.hpp>

#include <vector>
#include "systems/CollisionSystem.hpp"

namespace sage
{
	struct Ability
	{
        entt::registry* registry;
        CollisionSystem* collisionSystem;
        std::vector<entt::entity> hitUnits;
        float cooldownLimit;
		float cooldownTimer;
		float duration;
		float initialDamage;
		float damageOverTime; // 0 if no damage over time
		// A DoT should probably push a function to the affected unit and the ability's "DoT update" should be called every frame
		virtual void Use() = 0;
        virtual void Update();
		virtual void Draw3D();
        virtual ~Ability() = default;
        Ability(entt::registry* _registry, CollisionSystem* _collisionSystem);
	};
}