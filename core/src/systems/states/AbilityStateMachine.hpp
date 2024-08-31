#pragma once

#include "abilities/AbilityData.hpp"
#include "components/Ability.hpp"
#include "components/States.hpp"
#include "StateMachine.hpp"
#include "Timer.hpp"

#include <entt/entt.hpp>
#include <memory>
#include <unordered_map>

namespace sage
{
    class GameData;
    class VisualFX;

    class AbilityStateController : StateMachineController<AbilityStateController, AbilityState, AbilityStateEnum>
    {
        class IdleState;
        class AwaitingExecutionState;
        class CursorSelectState;

        GameData* gameData;

        template <typename AbilityFunc>
        AbilityFunc& getExecuteFunc(
            entt::registry* _registry, entt::entity caster, entt::entity _abilityDataEntity, GameData* _gameData)
        {
            if (_registry->any_of<AbilityFunc>(_abilityDataEntity))
            {
                return _registry->get<AbilityFunc>(_abilityDataEntity);
            }
            else
            {
                return _registry->emplace<AbilityFunc>(
                    _abilityDataEntity, _registry, caster, _abilityDataEntity, _gameData);
            }
        }

        void executeAbility(entt::entity abilityEntity);
        bool checkRange(entt::entity abilityEntity);
        void spawnAbility(entt::entity abilityEntity);

      public:
        void CancelCast(entt::entity abilityEntity);
        void StartCast(entt::entity abilityEntity);
        void Update();
        void Draw3D();

        entt::sigh<void(const char*)> castFailed; // Reason as a string. (Should change to a flag)

        ~AbilityStateController();
        AbilityStateController(const AbilityStateController&) = delete;
        AbilityStateController& operator=(const AbilityStateController&) = delete;
        AbilityStateController(entt::registry* _registry, GameData* _gameData);

        friend class StateMachineController; // Required for CRTP
    };

} // namespace sage