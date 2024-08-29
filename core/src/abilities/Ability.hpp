#pragma once

#include "AbilityData.hpp"
#include "Timer.hpp"

#include <entt/entt.hpp>
#include <memory>
#include <unordered_map>

namespace sage
{
    class GameData;
    class VisualFX;
    class AbilityState;

    enum class AbilityStateEnum
    {
        IDLE,
        CURSOR_SELECT,
        AWAITING_EXECUTION
    };

    class Ability
    {
        class IdleState;
        class AwaitingExecutionState;

        GameData* gameData;

      protected:
        entt::registry* registry;
        entt::entity self;
        AbilityData abilityData; // Make component
        Timer cooldownTimer;
        Timer animationDelayTimer;
        std::unique_ptr<VisualFX> vfx;

        AbilityState* state;
        std::unordered_map<AbilityStateEnum, std::unique_ptr<AbilityState>> states;
        void ChangeState(AbilityStateEnum newState);

      public:
        virtual void ResetCooldown();
        virtual bool IsActive();
        float GetRemainingCooldownTime() const;
        float GetCooldownDuration() const;
        bool CooldownReady() const;

        virtual void Cancel();
        virtual void Execute();
        virtual void Update();
        virtual void Draw3D();
        virtual void Init();

        virtual ~Ability();
        Ability(const Ability&) = delete;
        Ability& operator=(const Ability&) = delete;
        Ability(entt::registry* registry, entt::entity _self, const AbilityData& abilityData, GameData* _gameData);
    };

} // namespace sage