#include "Scene.hpp"

#include "GameData.hpp"

#include "Camera.hpp"
#include "Cursor.hpp"
#include "UserInput.hpp"

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

#include <GameObjectFactory.hpp>

namespace sage
{

    void Scene::Update()
    {
        data->renderSystem->Update();
        data->camera->Update();
        data->userInput->ListenForInput();
        data->cursor->Update();
    }

    void Scene::Draw3D()
    {
        data->renderSystem->Draw();
        // If we hit something, draw the cursor at the hit point
        data->cursor->Draw3D();
    };

    void Scene::Draw2D()
    {
        data->cursor->Draw2D();
    }

    void Scene::DrawDebug()
    {
        data->cursor->DrawDebug();
        lightSubSystem->DrawDebugLights();
    }

    Scene::~Scene()
    {
        // delete data;
        // delete lightSubSystem;
    }

    Scene::Scene(
        entt::registry* _registry, KeyMapping* _keyMapping, Settings* _settings, const std::string& mapPath)
        : registry(_registry),
          data(std::make_unique<GameData>(_registry, _keyMapping, _settings)),
          lightSubSystem(std::make_unique<LightSubSystem>(_registry))
    {
        data->Load();
        float slices = 500;
        if (!FileExists("resources/output.bin"))
        {
            GameObjectFactory::loadMap(registry, this, slices, mapPath);
        }
        else
        {
            lightSubSystem->LinkAllRenderablesToLight();
        }
        data->navigationGridSystem->Init(slices, 1.0f, mapPath);
        data->navigationGridSystem->PopulateGrid();
    };

} // namespace sage