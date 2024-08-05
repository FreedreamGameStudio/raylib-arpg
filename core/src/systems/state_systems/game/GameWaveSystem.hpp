//
// Created by Steve Wheeler on 31/07/2024.
//

#pragma once

#include "entt/entt.hpp"
#include "TimerManager.hpp"

#include "components/states/GameStates.hpp"
#include "systems/state_systems/StateMachineSystem.hpp"

namespace sage
{
	class GameData; // forward dec
	class GameWaveSystem : public StateMachine<GameWaveSystem, StateGameWaveattack>
	{
		GameData* gameData;
		TimerManager* timerManager;
		void initWave();
		void OnTimerEnd();
		entt::delegate<void()> callback;
	public:
		GameWaveSystem(
				entt::registry* _registry,
				GameData* _gameData,
				entt::entity _gameEntity, 
				TimerManager* _timerManager);
		void Update() override;
		void Draw3D() override;
		void OnComponentAdded(entt::entity entity) override;
		void OnComponentRemoved(entt::entity entity) override;
	};
} // sage
