
//
// Created by Steve Wheeler on 08/06/2024.
//
#include "WaveMobDefaultStateSystem.hpp"
#include "components/Animation.hpp"
#include "components/sgTransform.hpp"

namespace sage
{
	void WaveMobDefaultStateSystem::OnComponentAdded(entt::entity entity)
	{
		Vector3 target = { 52, 0, -10 };
		auto& a = registry->get<MoveableActor>(entity);
		auto& t = registry->get<sgTransform>(entity);
		auto& animation = registry->get<Animation>(entity);
		animation.ChangeAnimationByEnum(AnimationEnum::MOVE);
		actorMovementSystem->PathfindToLocation(entity, target);
	}

	void WaveMobDefaultStateSystem::OnComponentRemoved(entt::entity entity)
	{
		//actorMovementSystem->CancelMovement(entity);
	}

	void WaveMobDefaultStateSystem::Update()
	{
	}

	void WaveMobDefaultStateSystem::Draw3D()
	{
	}

	WaveMobDefaultStateSystem::WaveMobDefaultStateSystem(
			entt::registry* _registry,
			ActorMovementSystem* _actorMovementSystem)
			:
			StateMachine(_registry),
			actorMovementSystem(_actorMovementSystem)
	{
	}
} // sage
