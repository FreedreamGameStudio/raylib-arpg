#include "AbilityStateMachine.hpp"

#include "abilities/AbilityData.hpp"
#include "abilities/AbilityFunctions.hpp"
#include "abilities/AbilityIndicator.hpp"
#include "abilities/AbilityResourceManager.hpp"
#include "abilities/vfx/VisualFX.hpp"
#include "components/Animation.hpp"
#include "components/MoveableActor.hpp"
#include "components/sgTransform.hpp"
#include "Cursor.hpp"
#include "GameData.hpp"
#include "GameObjectFactory.hpp"
#include "systems/ControllableActorSystem.hpp"
#include "TextureTerrainOverlay.hpp"
#include "Timer.hpp"

#include "raylib.h"
#include <cassert>
#include <iostream>

namespace sage
{
    class AbilityStateController::IdleState : public StateMachine
    {

      public:
        entt::sigh<void(entt::entity)> onRestartTriggered;

        void Update(entt::entity abilityEntity) override
        {
            auto& ab = registry->get<Ability>(abilityEntity);
            ab.cooldownTimer.Update(GetFrameTime());
            if (ab.cooldownTimer.HasFinished() && ab.ad.base.repeatable)
            {
                onRestartTriggered.publish(abilityEntity);
            }
        }

        IdleState(entt::registry* _registry, GameData* _gameData) : StateMachine(_registry, _gameData)
        {
        }
    };

    // --------------------------------------------

    class AbilityStateController::CursorSelectState : public StateMachine
    {
        bool cursorActive = false; // Limits us to one cursor at once (I assume this is fine)

        void enableCursor(entt::entity abilityEntity)
        {
            auto& ab = registry->get<Ability>(abilityEntity);
            ab.abilityIndicator->Init(gameData->cursor->terrainCollision().point);
            ab.abilityIndicator->Enable(true);
            gameData->cursor->Disable();
            gameData->cursor->Hide();
        }

        void disableCursor(entt::entity abilityEntity)
        {
            auto& ab = registry->get<Ability>(abilityEntity);
            gameData->cursor->Enable();
            gameData->cursor->Show();
            ab.abilityIndicator->Enable(false);
        }

        void toggleCursor(entt::entity abilityEntity)
        {
            if (cursorActive)
            {
                disableCursor(abilityEntity);
                cursorActive = false;
            }
            else
            {
                enableCursor(abilityEntity);
                cursorActive = true;
            }
        }

      public:
        entt::sigh<void(entt::entity)> onConfirm;
        void Update(entt::entity abilityEntity) override
        {
            auto& ab = registry->get<Ability>(abilityEntity);
            ab.abilityIndicator->Update(gameData->cursor->terrainCollision().point);
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
            {
                std::cout << "Mouse button down \n";
                onConfirm.publish(abilityEntity);
            }
        }

        void OnStateEnter(entt::entity abilityEntity) override
        {
            enableCursor(abilityEntity);
            cursorActive = true;
        }

        void OnStateExit(entt::entity abilityEntity) override
        {
            if (cursorActive)
            {
                disableCursor(abilityEntity);
                cursorActive = false;
            }
        }

        CursorSelectState(entt::registry* _registry, GameData* _gameData) : StateMachine(_registry, _gameData)
        {
        }
    };

    // --------------------------------------------

    class AbilityStateController::AwaitingExecutionState : public StateMachine
    {

        void signalExecute(entt::entity abilityEntity)
        {
            onExecute.publish(abilityEntity);
        }

      public:
        entt::sigh<void(entt::entity)> onExecute;

        void OnStateEnter(entt::entity abilityEntity) override
        {
            auto& ab = registry->get<Ability>(abilityEntity);
            ab.cooldownTimer.Start();
            ab.executionDelayTimer.Start();

            auto& ad = ab.ad;
            if (ad.base.behaviourPreHit == AbilityBehaviourPreHit::DETACHED_PROJECTILE)
            {
                GameObjectFactory::createProjectile(registry, ab.caster, abilityEntity, gameData);
                auto& moveable = registry->get<MoveableActor>(abilityEntity);
                entt::sink sink{moveable.onDestinationReached};
                sink.connect<&AwaitingExecutionState::signalExecute>(this);
            }
        }

        void Update(entt::entity abilityEntity) override
        {
            auto& ab = registry->get<Ability>(abilityEntity);
            ab.executionDelayTimer.Update(GetFrameTime());
            auto& ad = ab.ad;

            if (ab.executionDelayTimer.HasFinished() &&
                ad.base.behaviourPreHit != AbilityBehaviourPreHit::DETACHED_PROJECTILE)
            {
                onExecute.publish(abilityEntity);
            }
        }

        AwaitingExecutionState(entt::registry* _registry, GameData* _gameData) : StateMachine(_registry, _gameData)

        {
        }
    };

    // ----------------------------

    void AbilityStateController::CancelAbility(entt::entity abilityEntity)
    {
        auto& ab = registry->get<Ability>(abilityEntity);
        if (ab.vfx && ab.vfx->active)
        {
            ab.vfx->active = false;
        }
        ab.cooldownTimer.Stop();
        ab.executionDelayTimer.Stop();
        ChangeState(abilityEntity, AbilityStateEnum::IDLE);
    }

    void AbilityStateController::executeAbility(entt::entity abilityEntity)
    {
        auto& ab = registry->get<Ability>(abilityEntity);
        auto& ad = ab.ad;

        if (ad.base.behaviourOnHit == AbilityBehaviourOnHit::HIT_TARGETED_UNIT)
        {
            auto& executeFunc = getExecuteFunc<SingleTargetHit>(registry, ab.caster, abilityEntity, gameData);
            executeFunc.Execute();
        }
        else if (ad.base.behaviourOnHit == AbilityBehaviourOnHit::HIT_ALL_IN_RADIUS)
        {
            auto& executeFunc = getExecuteFunc<HitAllInRadius>(registry, ab.caster, abilityEntity, gameData);
            executeFunc.Execute();
        }

        ChangeState(abilityEntity, AbilityStateEnum::IDLE);
    }

    void AbilityStateController::confirmAbility(entt::entity abilityEntity)
    {
        // Spawn target is either at cursor, at enemy, or at player
        // After spawned: Follow caster position, follow enemy position (maybe), follow the ability's detached
        // transform/collider (abilityEntity) and follow its position, or do nothing.
        auto& ab = registry->get<Ability>(abilityEntity);
        auto& ad = ab.ad;
        if (!registry->any_of<sgTransform>(abilityEntity))
        {
            registry->emplace<sgTransform>(abilityEntity, abilityEntity);
        }
        auto& trans = registry->get<sgTransform>(abilityEntity);

        if (ad.base.behaviourPreHit == AbilityBehaviourPreHit::DETACHED_PROJECTILE ||
            ad.base.behaviourPreHit == AbilityBehaviourPreHit::DETACHED_STATIONARY)
        {
            auto& casterPos = registry->get<sgTransform>(ab.caster).GetWorldPos();
            auto point = gameData->cursor->terrainCollision().point;
            if (Vector3Distance(point, casterPos) > ad.base.range)
            {
                std::cout << "Out of range. \n";
                return;
            }
        }

        auto& animation = registry->get<Animation>(ab.caster);
        animation.ChangeAnimationByParams(ad.animationParams);

        if (ab.vfx)
        {
            if (ad.base.spawnBehaviour == AbilitySpawnBehaviour::AT_CASTER)
            {
                auto& casterTrans = registry->get<sgTransform>(ab.caster);
                auto& casterBB = registry->get<Collideable>(ab.caster).worldBoundingBox;
                // TODO: Below doesn't work as intended
                float heightOffset = Vector3Subtract(casterBB.max, casterBB.min).y;
                Vector3 pos = {casterTrans.GetWorldPos().x, heightOffset, casterTrans.GetWorldPos().z};
                trans.SetPosition(pos);
                trans.SetParent(&casterTrans);
                ab.vfx->InitSystem();
            }
            else if (ad.base.spawnBehaviour == AbilitySpawnBehaviour::AT_CURSOR)
            {
                trans.SetPosition(gameData->cursor->terrainCollision().point);
                ab.vfx->InitSystem();
            }
        }

        ChangeState(abilityEntity, AbilityStateEnum::AWAITING_EXECUTION);
    }

    // Determines if we need to display an indicator or not
    void AbilityStateController::InitAbility(entt::entity abilityEntity)
    {

        auto& ab = registry->get<Ability>(abilityEntity);

        if (ab.ad.cursorBased) // Toggle indicator
        {
            auto state = registry->get<AbilityState>(abilityEntity).GetCurrentState();
            if (state == AbilityStateEnum::CURSOR_SELECT)
            {
                ChangeState(abilityEntity, AbilityStateEnum::IDLE);
            }
            else
            {
                ChangeState(abilityEntity, AbilityStateEnum::CURSOR_SELECT);
            }
        }
        else
        {
            confirmAbility(abilityEntity);
        }
    }

    void AbilityStateController::Update()
    {
        auto view = registry->view<AbilityState, Ability>();
        for (auto abilityEntity : view)
        {
            auto& ab = registry->get<Ability>(abilityEntity);
            auto state = registry->get<AbilityState>(abilityEntity).GetCurrentState();
            if (!(ab.IsActive() || state == AbilityStateEnum::CURSOR_SELECT))
            {
                continue;
            }

            states.at(state)->Update(abilityEntity);
            if (ab.vfx && ab.vfx->active)
            {
                ab.vfx->Update(GetFrameTime());
            }
        }
    }

    void AbilityStateController::Draw3D()
    {
        auto view = registry->view<AbilityState, Ability>();
        for (auto abilityEntity : view)
        {
            auto& ab = registry->get<Ability>(abilityEntity);
            auto state = registry->get<AbilityState>(abilityEntity).GetCurrentState();
            if (!(ab.IsActive() || state == AbilityStateEnum::CURSOR_SELECT))
            {
                continue;
            }
            states.at(state)->Draw3D(abilityEntity);
            if (ab.vfx && ab.vfx->active)
            {
                ab.vfx->Draw3D();
            }
        }
    }

    AbilityStateController::~AbilityStateController()
    {
    }

    AbilityStateController::AbilityStateController(entt::registry* _registry, GameData* _gameData)
        : StateMachineController(_registry), gameData(_gameData)
    {

        auto idleState = std::make_unique<IdleState>(_registry, _gameData);
        entt::sink onRestartTriggeredSink{idleState->onRestartTriggered};
        onRestartTriggeredSink.connect<&AbilityStateController::InitAbility>(this);
        states[AbilityStateEnum::IDLE] = std::move(idleState);

        auto awaitingExecutionState = std::make_unique<AwaitingExecutionState>(_registry, _gameData);
        entt::sink onExecuteSink{awaitingExecutionState->onExecute};
        onExecuteSink.connect<&AbilityStateController::executeAbility>(this);
        states[AbilityStateEnum::AWAITING_EXECUTION] = std::move(awaitingExecutionState);

        auto cursorState = std::make_unique<CursorSelectState>(_registry, _gameData);
        entt::sink onConfirmSink{cursorState->onConfirm};
        onConfirmSink.connect<&AbilityStateController::confirmAbility>(this);
        states[AbilityStateEnum::CURSOR_SELECT] = std::move(cursorState);
    }

} // namespace sage