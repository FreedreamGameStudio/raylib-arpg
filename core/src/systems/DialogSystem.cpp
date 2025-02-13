﻿//
// Created by steve on 11/05/2024.
//

#include "DialogSystem.hpp"

#include "Camera.hpp"
#include "Cursor.hpp"
#include "Systems.hpp"

#include "systems/ControllableActorSystem.hpp"

#include "GameUiEngine.hpp"
#include "GameUiFactory.hpp"

namespace sage
{

    void DialogSystem::progressConversation(const dialog::Conversation* conversation)
    {
        dialogWindow->Remove();
        dialogWindow = GameUiFactory::CreateDialogWindow(sys->uiEngine.get(), conversation->owner);
    }

    void DialogSystem::StartConversation(const sgTransform& cutscenePose, entt::entity npc)
    {
        const auto& dialogComponent = registry->get<DialogComponent>(npc);
        if (dialogComponent.cameraPos.has_value())
        {
            sys->camera->CutscenePose(cutscenePose, dialogComponent.cameraPos.value());
        }
        sys->camera->LockInput();
        sys->cursor->DisableContextSwitching();

        dialogComponent.conversation->BindKeysToOptionSelect();

        dialogComponent.conversation->onConversationProgress.Subscribe(
            [this](const dialog::Conversation* conv) { progressConversation(conv); });

        dialogComponent.conversation->onConversationEnd.Subscribe([this, npc]() { endConversation(npc); });

        dialogWindow = GameUiFactory::CreateDialogWindow(sys->uiEngine.get(), npc);
    }

    void DialogSystem::endConversation(entt::entity npc) const
    {
        sys->camera->UnlockInput();
        sys->cursor->EnableContextSwitching();
        if (const auto& dialogComponent = registry->get<DialogComponent>(npc);
            dialogComponent.cameraPos.has_value())
        {
            sys->camera->CutsceneEnd();
        }
        dialogWindow->Remove();
    }

    dialog::Conversation* DialogSystem::GetConversation(entt::entity owner, ConversationID conversationId)
    {
        return nullptr;
    }

    DialogSystem::DialogSystem(entt::registry* registry, Systems* _sys) : BaseSystem(registry), sys(_sys)
    {
    }
} // namespace sage
