//
// Created by Steve Wheeler on 21/07/2024.
//

#include "ConeOfCold.hpp"
#include "raylib.h"
#include "components/sgTransform.hpp"
#include "components/CombatableActor.hpp"
#include "components/Animation.hpp"

namespace sage
{
	void ConeOfCold::Use(entt::entity actor)
	{
		if (cooldownTimer > 0)
		{
			std::cout << "Waiting for cooldown \n";
			return;
		}

		std::cout << "CoC ability used \n";
		cooldownTimer = cooldownLimit;
		active = true;
		windupTimer = 0.0f;
		auto& animation = registry->get<Animation>(actor);
		animation.ChangeAnimationByEnum(AnimationEnum::SPIN, true);
	}

	void ConeOfCold::Update(entt::entity actor)
	{
		cooldownTimer -= GetFrameTime();
		if (windupTimer < windupLimit)
		{
			windupTimer += GetFrameTime();
			return;
		}
		if (!active) return;
		auto& actorTransform = registry->get<sgTransform>(actor);
		const auto& actorCol = registry->get<Collideable>(actor);

		auto view = registry->view<CombatableActor>();

		for (auto& entity : view)
		{
			if (entity == actor) continue;
			//if (std::find(hitUnits.begin(), hitUnits.end(), entity) != hitUnits.end()) continue;

			const auto& targetTransform = registry->get<sgTransform>(entity);
			const auto& targetCol = registry->get<Collideable>(entity);

			if (CheckCollisionBoxSphere(targetCol.worldBoundingBox, actorTransform.position(), whirlwindRadius))
			{
				//hitUnits.push_back(entity);
				const auto& combatable = registry->get<CombatableActor>(entity);
				combatable.onHit.publish(entity, actor, attackData);
				std::cout << "Hit unit \n";
			}
		}
		active = false;
	}

	ConeOfCold::ConeOfCold(entt::registry* _registry, CollisionSystem* _collisionSystem, TimerManager* _timerManager) :
			Ability(_registry, _collisionSystem, _timerManager)
	{
		windupTimer = 0.0f;
		windupLimit = 0.75f;
		cooldownLimit = 3.0f;
		attackData.damage = 25.0f;
		attackData.element = AttackElement::ICE;
	}
}
