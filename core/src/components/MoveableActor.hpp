//
// Created by Steve Wheeler on 16/06/2024.
//

#pragma once

#include "NavigationGridSquare.hpp"
#include "raylib.h"

#include <entt/entt.hpp>

#include <deque>
#include <optional>
#include <vector>

namespace sage
{

    // Params we can use to transition from one state to another
    struct FollowTargetParams
    {
        entt::entity targetActor;
        FollowTargetParams(entt::entity _targetActor) : targetActor(_targetActor)
        {
        }
    };

    class FollowTarget
    {
        entt::registry* registry;
        entt::entity self{};
        entt::connection onTargetPosUpdateCnx{};

        Vector3 targetPrevPos{};
        double timeStarted{};
        float timerThreshold = 0.25;
        void changePath() const;

      public:
        const entt::entity targetActor = entt::null;
        entt::sigh<void(entt::entity, entt::entity)> onPathChanged{}; // Self, target

        ~FollowTarget();
        FollowTarget(entt::registry* _registry, entt::entity _self, entt::entity _targetActor);
    };

    struct MoveableActorCollision
    {
        entt::entity hitEntityId = entt::null;
        Vector3 hitLastPos{};
    };

    struct MoveableActor
    {
        float movementSpeed = 0.35f;
        // The max range the actor can pathfind at one time.
        int pathfindingBounds = 50;

        // std::optional<MoveableActorCollision> moveableActorCollision;
        entt::entity hitEntityId = entt::null;
        Vector3 hitLastPos{};

        std::optional<FollowTarget> followTarget;

        std::deque<Vector3> path{};
        [[nodiscard]] bool IsMoving() const
        {
            return !path.empty();
        }

        [[nodiscard]] Vector3 GetDestination() const
        {
            assert(IsMoving()); // Check this independently before calling this function.
            return path.back();
        }

        std::vector<GridSquare> debugRay;

        entt::sigh<void(entt::entity)> onStartMovement{};
        entt::sigh<void(entt::entity)> onDestinationReached{};
        entt::sigh<void(entt::entity)> onDestinationUnreachable{};
        entt::sigh<void(entt::entity)> onPathChanged{};    // Was previously moving, now moving somewhere else
        entt::sigh<void(entt::entity)> onMovementCancel{}; // Was previously moving, now cancelled
    };
} // namespace sage
