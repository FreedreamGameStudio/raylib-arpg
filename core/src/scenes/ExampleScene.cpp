//
// Created by Steve Wheeler on 27/03/2024.
//

#include "ExampleScene.hpp"
#include "GameObjectFactory.hpp"

#include "GameData.hpp"

// TMP
#include "abilities/vfx/Explosion.hpp"
#include "abilities/vfx/SpiralFountainVFX.hpp"

#include "components/Collideable.hpp"
#include "components/sgTransform.hpp"

#include "Camera.hpp"
#include "Cursor.hpp"
#include "KeyMapping.hpp"
#include "Settings.hpp"

// Systems
#include "systems/ActorMovementSystem.hpp"
#include "systems/AnimationSystem.hpp"
#include "systems/CollisionSystem.hpp"
#include "systems/CombatSystem.hpp"
#include "systems/ControllableActorSystem.hpp"
#include "systems/dialogue/DialogueSystem.hpp"
#include "systems/HealthBarSystem.hpp"
#include "systems/LightSubSystem.hpp"
#include "systems/NavigationGridSystem.hpp"
#include "systems/PlayerAbilitySystem.hpp"
#include "systems/RenderSystem.hpp"
#include "systems/states/StateMachines.hpp"
#include "systems/TimerSystem.hpp"

#include "raylib.h"

namespace sage
{
    void ExampleScene::Update()
    {
        Scene::Update();
        data->actorMovementSystem->Update();
        data->controllableActorSystem->Update();
        data->animationSystem->Update();
        data->dialogueSystem->Update();
        data->healthBarSystem->Update();
        data->stateMachines->Update();
        data->playerAbilitySystem->Update();
        data->timerSystem->Update();
        data->collisionSystem->Update();
    }

    void ExampleScene::Draw2D()
    {
        data->dialogueSystem->Draw2D();
        Scene::Draw2D();
    }

    void ExampleScene::Draw3D()
    {
        Scene::Draw3D();
        data->healthBarSystem->Draw3D();
        data->playerAbilitySystem->Draw3D();
        data->stateMachines->Draw3D();
    }

    void ExampleScene::DrawDebug()
    {
        data->collisionSystem->DrawDebug();
        data->navigationGridSystem->DrawDebug();
        data->actorMovementSystem->DrawDebug();
    }

    ExampleScene::~ExampleScene()
    {
    }

    ExampleScene::ExampleScene(
        entt::registry* _registry, KeyMapping* _keyMapping, Settings* _settings, const std::string& mapPath)
        : Scene(_registry, _keyMapping, _settings, mapPath)
    {
        lightSubSystem->lights[0] =
            CreateLight(LIGHT_POINT, {0, 50, 0}, Vector3Zero(), RAYWHITE, lightSubSystem->shader);
        // std::string mapPath = "resources/models/obj/level-basic.obj";
        auto playerId = GameObjectFactory::createPlayer(registry, data.get(), {30.0f, 0, 20.0f}, "Player");
        auto knight = GameObjectFactory::createKnight(registry, data.get(), {0.0f, 0, 20.0f}, "Knight");
        // GameObjectFactory::createPortal(registry, data.get(), Vector3{52, 0, -10});

        GameObjectFactory::createWizardTower(registry, data.get(), Vector3{52, 0, -30});

        // TODO: tmp
        const auto& col = registry->get<Collideable>(knight);
        data->navigationGridSystem->MarkSquareAreaOccupied(col.worldBoundingBox, true, knight);

        // fountain = std::make_unique<SpiralFountainVFX>(data.get());
        // fountain->InitSystem({30.0f, 4, 20.0f});
        // explosion = std::make_unique<Explosion>(registry);
        // explosion->SetOrigin({0, 3, 0});
    }
} // namespace sage
