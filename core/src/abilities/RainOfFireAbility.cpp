//
// Created by Steve Wheeler on 21/07/2024.
//

#include "RainOfFireAbility.hpp"
#include "raylib.h"
#include "components/sgTransform.hpp"
#include "components/CombatableActor.hpp"
#include "components/Animation.hpp"

namespace sage
{
	void RainOfFireAbility::Init(entt::entity self)
	{
		if (cooldownTimer() > 0)
		{
			std::cout << "Waiting for cooldown \n";
			return;
		}
		if (!spellCursor->active())
		{
			spellCursor->Init(cursor->terrainCollision().point);
			spellCursor->Enable(true);
			cursor->Disable();
			cursor->Hide();
			return;
		}
		else
		{
			cursor->Enable();
			cursor->Show();
			spellCursor->Enable(false); // Cancel move
		}
	}

	void RainOfFireAbility::Execute(entt::entity self)
	{
		const auto& actorCol = registry->get<Collideable>(self);

		auto view = registry->view<CombatableActor>();

		vfx->InitSystem(cursor->collision().point);

		for (auto& entity : view)
		{
			if (entity == self) continue;
			const auto& targetCol = registry->get<Collideable>(entity);

			if (CheckCollisionBoxSphere(targetCol.worldBoundingBox, cursor->collision().point, whirlwindRadius))
			{
				//hitUnits.push_back(entity);
				const auto& combatable = registry->get<CombatableActor>(entity);
				AttackData _attackData = attackData;
				_attackData.hit = entity;
				_attackData.attacker = self;
				combatable.onHit.publish(_attackData);
				std::cout << "Hit unit \n";
			}
		}
		active = false;
		timerManager->RemoveTimer(windupTimerId);
		windupTimerId = -1;
	}
	
	void RainOfFireAbility::Confirm(entt::entity self)
	{
		std::cout << "Rain of fire ability used \n";

		cooldownTimerId = timerManager->AddTimer(m_cooldownLimit, &Ability::ResetCooldown, this);
		windupTimerId = timerManager->AddTimer(m_windupLimit, &RainOfFireAbility::Execute, this, self);

		active = true;
		auto& animation = registry->get<Animation>(self);
		animation.ChangeAnimationByEnum(AnimationEnum::SPIN, true);
		spellCursor->Enable(false);
		cursor->Enable();
		cursor->Show();
	}

	void RainOfFireAbility::Draw3D(entt::entity self)
	{
		if (vfx->active)
		{
			vfx->Draw3D();
		}
		
		Ability::Draw3D(self);
	}

	void RainOfFireAbility::Update(entt::entity self)
	{
		if (vfx->active)
		{
			vfx->Update(GetFrameTime());
		}
		if (spellCursor->active())
		{
			spellCursor->Update(cursor->terrainCollision().point);
			if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !active)
			{
				Confirm(self);
			}
			return;
		}
	}

	RainOfFireAbility::RainOfFireAbility(
			entt::registry* _registry,
			Camera* _camera,
			Cursor* _cursor,
			CollisionSystem* _collisionSystem,
			NavigationGridSystem* _navigationGridSystem,
			ControllableActorSystem* _controllableActorSystem,
			TimerManager* _timerManager)
			:
			Ability(_registry, _collisionSystem, _timerManager),
			cursor(_cursor),
			controllableActorSystem(_controllableActorSystem)
	{
		m_windupLimit = 0.75f;
		m_cooldownLimit = 3.0f;
		attackData.damage = 25.0f;
		attackData.element = AttackElement::FIRE;
		spellCursor = std::make_unique<TextureTerrainOverlay>(
				registry, 
				_navigationGridSystem,
				"resources/textures/cursor/rainoffire_cursor.png",
				Color{ 255, 215, 0, 255},
				"resources/shaders/glsl330/bloom.fs");
		vfx = std::make_unique<RainOfFireVFX>(_camera->getRaylibCam());
	}
}
