//
// Created by Steve Wheeler on 31/07/2024.
//

#pragma once

#include "systems/state_systems/StateMachine.hpp"
#include "GameDefaultSystem.hpp"
#include "GameWaveSystem.hpp"

#include <entt/entt.hpp>

#include <vector>

namespace sage
{
	class GameData;
	class TimerManager;
	class BaseSystem;
	class GameStateSystem
	{
		entt::entity gameEntity;
		std::vector<BaseSystem*> systems;
		
	public:
		std::unique_ptr<GameDefaultSystem> defaultSystem;
		std::unique_ptr<GameWaveSystem> waveSystem;
		void Update();
		GameStateSystem(
				entt::registry* _registry, 
				GameData* _gameData,
				TimerManager* _timerManager);
	};

} // sage
