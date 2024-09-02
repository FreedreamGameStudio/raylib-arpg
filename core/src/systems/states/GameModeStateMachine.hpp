//
// Created by Steve Wheeler on 31/07/2024.
//

#pragma once

#include "components/States.hpp"
#include "systems/states/StateMachine.hpp"
#include <Timer.hpp>

#include <entt/entt.hpp>

#include <vector>

namespace sage
{
    class GameData;

    class GameModeStateController
        : public StateMachineController<GameModeStateController, GameState, GameStateEnum>
    {
        entt::entity gameEntity;

        class DefaultState;
        class WaveState;

      public:
        void Update();
        void Draw3D();

        GameModeStateController(entt::registry* _registry, GameData* gameData);

        friend class StateMachineController; // Required for CRTP
    };

} // namespace sage
