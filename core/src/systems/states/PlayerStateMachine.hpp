// Created by Steve Wheeler on 30/06/2024.
#pragma once

#include "components/States.hpp"
#include "systems/states/StateMachine.hpp"

#include <entt/entt.hpp>
#include <memory>
#include <vector>

namespace sage
{
    class GameData;

    class PlayerStateController
        : public StateMachineController<PlayerStateController, PlayerState, PlayerStateEnum>
    {
        class DefaultState;
        class MovingToAttackEnemyState;
        class MovingToTalkToNPCState;
        class CombatState;

      public:
        void Update();
        void Draw3D();

        ~PlayerStateController();
        PlayerStateController(entt::registry* _registry, GameData* gameData);

        friend class StateMachineController; // Required for CRTP
    };
} // namespace sage