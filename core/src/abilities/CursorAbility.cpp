//
// Created by Steve Wheeler on 21/07/2024.
//

#include "CursorAbility.hpp"

#include "Camera.hpp"
#include "components/Animation.hpp"

#include "particle/RainOfFireVFX.hpp"

#include "AbilityFunctions.hpp"

namespace sage
{
    // --------------------------------------------

    void CursorAbility::CursorSelectState::enableCursor()
    {
        abilityIndicator->Init(cursor->terrainCollision().point);
        abilityIndicator->Enable(true);
        cursor->Disable();
        cursor->Hide();
    }

    void CursorAbility::CursorSelectState::disableCursor()
    {
        cursor->Enable();
        cursor->Show();
        abilityIndicator->Enable(false);
    }

    void CursorAbility::CursorSelectState::Update(entt::entity self)
    {
        abilityIndicator->Update(cursor->terrainCollision().point);
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            onConfirm.publish(self);
        }
    }

    void CursorAbility::CursorSelectState::toggleCursor(entt::entity self)
    {
        if (cursorActive)
        {
            disableCursor();
            cursorActive = false;
        }
        else
        {
            enableCursor();
            cursorActive = true;
        }
    }

    void CursorAbility::CursorSelectState::OnExit(entt::entity self)
    {
        if (cursorActive)
        {
            disableCursor();
            cursorActive = false;
        }
    }

    void CursorAbility::CursorSelectState::OnEnter(entt::entity self)
    {
        enableCursor();
        cursorActive = true;
    }

    // --------------------------------------------

    void CursorAbility::Init(entt::entity self)
    {
        if (state == states[AbilityStateEnum::CURSOR_SELECT].get())
        {
            ChangeState(self, AbilityStateEnum::IDLE);
        }
        else
        {
            ChangeState(self, AbilityStateEnum::CURSOR_SELECT);
        }
    }

    void CursorAbility::confirm(entt::entity self)
    {
        if (vfx)
        {
            vfx->InitSystem(cursor->terrainCollision().point); // TODO: Add a target parameter in abilityData
        }
        Ability::Init(self);
    }

    CursorAbility::CursorAbility(
        entt::registry* _registry,
        Camera* _camera,
        Cursor* _cursor,
        std::unique_ptr<AbilityIndicator> _abilityIndicator,
        AbilityData _abilityData)
        : Ability(_registry, _abilityData, _camera), cursor(_cursor)
    {
        auto cursorState = std::make_unique<CursorSelectState>(
            cooldownTimer, animationDelayTimer, _cursor, std::move(_abilityIndicator));
        entt::sink onConfirmSink{cursorState->onConfirm};
        onConfirmSink.connect<&CursorAbility::confirm>(this);
        states[AbilityStateEnum::CURSOR_SELECT] = std::move(cursorState);
    }
} // namespace sage
