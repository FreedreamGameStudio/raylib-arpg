//
// Created by Steve Wheeler on 27/03/2024.
//

#include "ExampleScene.hpp"
#include "GameObjectFactory.hpp"

#include "GameData.hpp"

#include "components/Collideable.hpp"
#include "components/sgTransform.hpp"

#include "Camera.hpp"
#include "Cursor.hpp"
#include "KeyMapping.hpp"
#include "Settings.hpp"

// Systems
#include "AudioManager.hpp"
#include "components/QuestComponents.hpp"
#include "DialogFactory.hpp"
#include "GameUiFactory.hpp"
#include "ItemFactory.hpp"
#include "LightManager.hpp"
#include "QuestManager.hpp"
#include "systems/ActorMovementSystem.hpp"
#include "systems/AnimationSystem.hpp"
#include "systems/CollisionSystem.hpp"
#include "systems/CombatSystem.hpp"
#include "systems/ControllableActorSystem.hpp"
#include "systems/DialogSystem.hpp"
#include "systems/DoorSystem.hpp"
#include "systems/HealthBarSystem.hpp"
#include "systems/NavigationGridSystem.hpp"
#include "systems/PlayerAbilitySystem.hpp"
#include "systems/RenderSystem.hpp"
#include "systems/states/StateMachines.hpp"
#include "systems/TimerSystem.hpp"

#include <Serializer.hpp>

#include "components/DoorBehaviorComponent.hpp"
#include "raylib.h"
#include "systems/PartySystem.hpp"
#include "UserInput.hpp"
#include "ViewSerializer.hpp"

namespace sage
{
    void ExampleScene::Update()
    {
        Scene::Update();
    }

    void ExampleScene::Draw2D()
    {
        Scene::Draw2D();
    }

    void ExampleScene::Draw3D()
    {
        Scene::Draw3D();
    }

    void ExampleScene::DrawDebug3D()
    {
        Scene::DrawDebug3D();
    }

    ExampleScene::ExampleScene(
        entt::registry* _registry, KeyMapping* _keyMapping, Settings* _settings, AudioManager* _audioManager)
        : Scene(_registry, _keyMapping, _settings, _audioManager)
    {
        {
            //            auto questId = data->questManager->GetQuest("LeverBaseQuest");
            //            auto& leverBaseQuest = registry->get<Quest>(questId);

            //            leverBaseQuest.onQuestStart->Subscribe(
            //                [this](entt::entity) { data->partySystem->RemoveItemFromParty("QUEST_BONE"); });

            auto leverId = data->renderSystem->FindRenderableByMeshName("QUEST_LEVER");
            auto& leverPickedUpTask = registry->get<QuestTaskComponent>(leverId);

            auto& leverCol = registry->get<Collideable>(leverId);
            auto& leverRenderable = registry->get<Renderable>(leverId);

            leverPickedUpTask.onTaskCompleted->Subscribe([this, &leverCol, &leverRenderable](QuestTaskComponent*) {
                leverCol.Disable();
                leverRenderable.Disable();
                data->partySystem->GiveItemToSelected("QUEST_LEVER");
            });
            //
            //            leverBaseQuest.onQuestCompleted->Subscribe(
            //                [this](entt::entity) { data->audioManager->PlaySFX("resources/audio/sfx/test.ogg");
            //                });
        }

        data->dialogFactory->LoadDialog(); // Must be called after all npcs are loaded
        data->camera->FocusSelectedActor();
        data->audioManager->PlayMusic("resources/audio/music/5 A Safe Space LOOP TomMusic.ogg");
        auto soundScape = data->audioManager->PlayMusic("resources/audio/bgs/Cave.ogg");
        SetMusicVolume(soundScape, 0.75);
    }
} // namespace sage
