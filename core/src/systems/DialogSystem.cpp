﻿//
// Created by steve on 11/05/2024.
//

#include "DialogSystem.hpp"

#include "Camera.hpp"
#include "Cursor.hpp"
#include "GameData.hpp"

#include "systems/ControllableActorSystem.hpp"

#include "GameUiEngine.hpp"
#include "GameUiFactory.hpp"

namespace sage
{

    void DialogSystem::progressConversation(const dialog::Conversation* conversation)
    {
        dialogWindow->Remove();
        dialogWindow = GameUiFactory::CreateDialogWindow(gameData->uiEngine.get(), conversation->owner);
    }

    void DialogSystem::StartConversation(const sgTransform& cutscenePose, entt::entity npc)
    {
        oldCamPos = gameData->camera->GetPosition();
        oldCamTarget = gameData->camera->getRaylibCam()->target;
        gameData->camera->CutscenePose(cutscenePose);
        gameData->camera->LockInput();
        gameData->cursor->DisableContextSwitching();

        auto& dialogComponent = registry->get<DialogComponent>(npc);
        dialogComponent.conversation->onConversationProgress->Subscribe(
            [this](const dialog::Conversation* conv) { progressConversation(conv); });

        dialogComponent.conversation->onConversationEnd->Subscribe([this]() { EndConversation(); });

        dialogWindow = GameUiFactory::CreateDialogWindow(gameData->uiEngine.get(), npc);
    }

    void DialogSystem::EndConversation()
    {
        gameData->camera->UnlockInput();
        gameData->cursor->EnableContextSwitching();
        gameData->camera->SetCamera(oldCamPos, oldCamTarget);
        oldCamPos = {};
        oldCamTarget = {};
        dialogWindow->Remove();
    }

    dialog::Conversation* DialogSystem::GetConversation(entt::entity owner, ConversationID conversationId)
    {
        return nullptr;
    }

    DialogSystem::DialogSystem(entt::registry* registry, GameData* _gameData)
        : BaseSystem(registry), gameData(_gameData)
    {
    }
} // namespace sage
