//
// Created by Steve Wheeler on 31/07/2024.
//

#include "GameStateSystem.hpp"
#include "components/states/GameStates.hpp"


namespace sage
{
	void GameStateSystem::Update()
	{
		for (auto& system : systems)
		{
			system->Update();
		}
	}
	GameStateSystem::GameStateSystem(
			entt::registry* _registry,
			GameData* _gameData,
			TimerManager* _timerManager)
	{
		gameEntity = _registry->create();
		defaultSystem = std::make_unique<GameDefaultSystem>(_registry, gameEntity, _timerManager);
		waveSystem = std::make_unique<GameWaveSystem>(_registry, _gameData, gameEntity, _timerManager);
		systems.push_back(defaultSystem.get());
		systems.push_back(waveSystem.get());
		_registry->emplace<StateGameDefault>(gameEntity);
	}
} // sage