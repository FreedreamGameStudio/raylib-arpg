//
// Created by Steve Wheeler on 29/02/2024.
//

#include "ControllableActorSystem.hpp"

#include "components/ControllableActor.hpp"
#include "components/sgTransform.hpp"
#include "Cursor.hpp"
#include "GameData.hpp"
#include "systems/ActorMovementSystem.hpp"

namespace sage
{
    void ControllableActorSystem::Update() const
    {
        auto view = registry->view<ControllableActor>();
        for (auto& entity : view)
        {
            auto& selectedActor = registry->get<ControllableActor>(entity);
            if (selectedActor.targetActor != entt::null)
            {
                selectedActor.checkTargetPosTimer.Update(GetFrameTime());
            }
        }
    }

    bool ControllableActorSystem::ReachedDestination(entt::entity entity) const
    {
        return gameData->actorMovementSystem->ReachedDestination(entity);
    }

    void ControllableActorSystem::onTargetUpdate(entt::entity target)
    {
        auto& selectedActor = registry->get<ControllableActor>(selectedActorId);
        if (selectedActor.checkTargetPosTimer.HasFinished())
        {
            selectedActor.checkTargetPosTimer.Restart();
            auto& targetTrans = registry->get<sgTransform>(target);
            PathfindToLocation(selectedActorId, targetTrans.GetWorldPos());
        }
    }

    void ControllableActorSystem::CancelMovement(entt::entity entity)
    {
        auto& selectedActor = registry->get<ControllableActor>(entity);
        if (selectedActor.targetActor != entt::null && registry->valid(selectedActor.targetActor))
        {
            auto& target = registry->get<sgTransform>(selectedActor.targetActor);
            {
                entt::sink sink{target.onPositionUpdate};
                sink.disconnect<&ControllableActorSystem::onTargetUpdate>(this);
            }
        }

        gameData->actorMovementSystem->CancelMovement(entity);
    }

    void ControllableActorSystem::PathfindToLocation(entt::entity id, Vector3 location) const
    {
        gameData->actorMovementSystem->PathfindToLocation(id, location);
    }

    void ControllableActorSystem::MoveToLocation(entt::entity id)
    {
        gameData->actorMovementSystem->PathfindToLocation(id, {gameData->cursor->getFirstCollision().point});
    }

    void ControllableActorSystem::PatrolLocations(entt::entity id, const std::vector<Vector3>& patrol)
    {
        // actorMovementSystem->PathfindToLocation(id, patrol);
    }

    void ControllableActorSystem::SetSelectedActor(entt::entity id)
    {
        selectedActorId = id;
        onSelectedActorChange.publish(id);
    }

    entt::entity ControllableActorSystem::GetSelectedActor()
    {
        return selectedActorId;
    }

    ControllableActorSystem::ControllableActorSystem(entt::registry* _registry, GameData* _gameData)
        : BaseSystem(_registry), gameData(_gameData)

    {
    }
} // namespace sage
