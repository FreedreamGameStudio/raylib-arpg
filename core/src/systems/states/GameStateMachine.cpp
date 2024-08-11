//
// Created by Steve Wheeler on 31/07/2024.
//

#include "GameStateMachine.hpp"

#include "GameData.hpp"
#include "GameObjectFactory.hpp"
#include "systems/BaseSystem.hpp"
#include <ResourceManager.hpp>

#include <iostream>

#include "raylib.h"

namespace sage
{
    namespace gamestates
    {
        // ----------------------------
        void DefaultState::OnTimerEnd()
        {
            auto& gameState = registry->get<GameState>(gameEntity);
            gameState.ChangeState(gameEntity, GameStateEnum::Wave);
        }

        void DefaultState::Update(entt::entity entity)
        {
            timer.Update(GetFrameTime());
            if (timer.HasFinished())
            {
                OnTimerEnd();
            }
        }

        void DefaultState::Draw3D(entt::entity entity)
        {
        }

        void DefaultState::OnStateEnter(entt::entity entity)
        {
            timer.Start();
        }

        void DefaultState::OnStateExit(entt::entity entity)
        {
            timer.Stop();
        }

        DefaultState::DefaultState(entt::registry* _registry, entt::entity _gameEntity)
            : StateMachine(_registry), gameEntity(_gameEntity)
        {
            timer.SetMaxTime(5.0f);
        }

        // ----------------------------

        void WaveState::initWave()
        {
            GameObjectFactory::createEnemy(
                registry, gameData, {52.0f, 0, 10.0f}, "Enemy");
            GameObjectFactory::createEnemy(
                registry, gameData, {52.0f, 0, 20.0f}, "Enemy");
            GameObjectFactory::createEnemy(
                registry, gameData, {52.0f, 0, 30.0f}, "Enemy");
            GameObjectFactory::createEnemy(
                registry, gameData, {52.0f, 0, 40.0f}, "Enemy");
        }

        void WaveState::Update(entt::entity entity)
        {
        }

        void WaveState::Draw3D(entt::entity entity)
        {
        }

        void WaveState::OnStateEnter(entt::entity entity)
        {
            // Create waves here (enemies etc)
            std::cout << "Wave state entered! \n";
            initWave();
        }

        void WaveState::OnStateExit(entt::entity entity)
        {
        }

        WaveState::WaveState(
            entt::registry* _registry, GameData* _gameData, entt::entity _gameEntity)
            : StateMachine(_registry), gameData(_gameData)
        {
            // Preload model(s)
            ResourceManager::DynamicModelLoad("resources/models/gltf/goblin.glb");
        }

        // ----------------------------
    } // namespace gamestates

    StateMachine* GameStateController::GetSystem(GameStateEnum state)
    {
        switch (state)
        {
        case GameStateEnum::Default:
            return defaultState.get();
        case GameStateEnum::Wave:
            return waveState.get();
        default:
            return defaultState.get();
        }
    }

    void GameStateController::OnComponentRemoved(entt::entity entity)
    {
        auto& gameState = registry->get<GameState>(entity);
        entt::sink sink{gameState.onStateChanged};
        sink.disconnect<&GameStateController::ChangeState>(this);
        GetSystem(gameState.GetCurrentState())
            ->OnStateExit(entity); // Might not be a good idea if destroyed
    }

    void GameStateController::OnComponentAdded(entt::entity entity)
    {
        auto& gameState = registry->get<GameState>(entity);
        entt::sink sink{gameState.onStateChanged};
        sink.connect<&GameStateController::ChangeState>(this);
        GetSystem(gameState.GetCurrentState())->OnStateEnter(entity);
    }

    void GameStateController::Update()
    {
        auto view = registry->view<GameState>();
        for (const auto& entity : view)
        {
            auto state = registry->get<GameState>(entity).GetCurrentState();
            GetSystem(state)->Update(entity);
        }
    }

    void GameStateController::Draw3D()
    {
        auto view = registry->view<GameState>();
        for (const auto& entity : view)
        {
            auto state = registry->get<GameState>(entity).GetCurrentState();
            GetSystem(state)->Draw3D(entity);
        }
    }

    GameStateController::GameStateController(
        entt::registry* _registry, GameData* _gameData)
        : StateMachineController(_registry),
          gameEntity(_registry->create()),
          defaultState(std::make_unique<gamestates::DefaultState>(_registry, gameEntity)),
          waveState(
              std::make_unique<gamestates::WaveState>(_registry, _gameData, gameEntity))
    {
        systems.push_back(defaultState.get());
        systems.push_back(waveState.get());

        _registry->emplace<GameState>(gameEntity);
    }
} // namespace sage