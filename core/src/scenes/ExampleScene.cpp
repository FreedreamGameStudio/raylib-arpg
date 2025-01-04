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
            auto boneId = data->renderSystem->FindRenderableByMeshName("QUEST_BONE");
            data->questManager->AddTaskToQuest("LeverQuest", boneId);
            auto& quest = registry->get<Quest>(data->questManager->GetQuest("LeverQuest"));
            quest.StartQuest();

            auto& event = registry->emplace<QuestEventReactionComponent>(boneId, boneId, quest);
            entt::sink sink{event.onQuestCompleted};
            sink.connect<[](PartySystem& partySystem, entt::entity entity) {
                partySystem.RemoveItemFromParty("QUEST_BONE");
            }>(*data->partySystem);
        }

        {
            auto leverBaseId = data->renderSystem->FindRenderableByMeshName("QUEST_LEVER_BASE");
            registry->emplace<QuestTaskComponent>(leverBaseId, "LeverBaseQuest");
            data->questManager->AddTaskToQuest("LeverBaseQuest", leverBaseId);
            auto& quest = registry->get<Quest>(data->questManager->GetQuest("LeverBaseQuest"));
            auto leverId = data->renderSystem->FindRenderableByMeshName("QUEST_LEVER");
            auto& event = registry->emplace<QuestEventReactionComponent>(leverId, leverId, quest);
            entt::sink sink{event.onQuestStart};
            auto& renderable = registry->get<Renderable>(leverId);
            sink.connect<&Renderable::Disable>(renderable);
            auto& col = registry->get<Collideable>(leverId);
            sink.connect<&Collideable::Disable>(col);
            sink.connect<[](PartySystem& partySystem, entt::entity entity) {
                partySystem.GiveItemToSelected("QUEST_LEVER");
            }>(*data->partySystem);

            auto& finishEvent = registry->emplace<QuestEventReactionComponent>(leverBaseId, leverBaseId, quest);
            entt::sink sink2{finishEvent.onQuestCompleted};
            sink2.connect<[](AudioManager& audioManager, entt::entity entity) {
                audioManager.PlaySFX("resources/audio/sfx/test.ogg");
            }>(*data->audioManager);
        }

        data->dialogFactory->LoadDialog(); // Must be called after all npcs are loaded
        data->camera->FocusSelectedActor();
        data->audioManager->PlayMusic("resources/audio/music/5 A Safe Space LOOP TomMusic.ogg");
        auto soundScape = data->audioManager->PlayMusic("resources/audio/bgs/Cave.ogg");
        SetMusicVolume(soundScape, 0.75);
    }
} // namespace sage
