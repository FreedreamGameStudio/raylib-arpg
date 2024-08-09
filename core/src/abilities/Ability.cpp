#include "Ability.hpp"

#include "raylib.h"

namespace sage
{
    void Ability::ChangeState(entt::entity self, AbilityState newState)
    {
        state->OnExit(self);
        state = states[newState].get();
        state->OnEnter(self);
    }

    void Ability::ResetCooldown()
    {
        cooldownTimer.Reset();
    }

    bool Ability::IsActive() const
    {
        return cooldownTimer.IsRunning();
    }

    float Ability::GetRemainingCooldownTime() const
    {
        return cooldownTimer.GetRemainingTime();
    }

    float Ability::GetCooldownDuration() const
    {
        return cooldownTimer.GetMaxTime();
    }

    bool Ability::CooldownReady() const
    {
        return cooldownTimer.HasFinished() || !cooldownTimer.IsRunning();
    }

    void Ability::Update(entt::entity self)
    {
    }

    void Ability::Draw3D(entt::entity self)
    {
        // Draw the ability in 3D space
    }

    void Ability::Init(entt::entity self)
    {
        Execute(self);
    }

    Ability::Ability(entt::registry* _registry, const AbilityData& _abilityData)
        : registry(_registry), abilityData(_abilityData)
    {
        cooldownTimer.SetMaxTime(abilityData.cooldownDuration);
    }
} // namespace sage