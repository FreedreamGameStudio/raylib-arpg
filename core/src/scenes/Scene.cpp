#include "Scene.hpp"

#include "GameData.hpp"

#include "../../utils/EntityReflectionSignalRouter.hpp"

#include "Camera.hpp"
#include "Cursor.hpp"
#include "UserInput.hpp"

#include "components/Renderable.hpp"

// NB: We have to include all the headers required to build GameData
#include "AbilityFactory.hpp"
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
        // ResourceManager::GetInstance().UnloadAll();
    }

    Scene::Scene(
        entt::registry* _registry, KeyMapping* _keyMapping, Settings* _settings, const std::string& mapPath)
        : registry(_registry),
          lightSubSystem(std::make_unique<LightSubSystem>(_registry)),
          data(std::make_unique<GameData>(_registry, _keyMapping, _settings, lightSubSystem.get()))
    {

        float slices = 500;

        // ----
        data->Load();
        data->navigationGridSystem->Init(slices, 1.0f, "resources/output.bin");

        // Dependent on only the map/static meshes having been loaded at this point
        auto view = registry->view<Renderable>();
        for (auto entity : view)
            data->lightSubSystem->LinkRenderableToLight(entity);

        // ----
        // GameObjectFactory::loadMap(registry, this, slices, "resources/models/obj/level-basic.obj");
        // data->navigationGridSystem->Init(slices, 1.0f, "resources/models/obj/level-basic.obj");
        // ----

        // ----
        // if (!FileExists("resources/output.bin"))
        // {
        //     GameObjectFactory::loadMap(registry, this, slices, "resources/models/obj/level-basic.obj");
        // }
        // ----

        data->navigationGridSystem->PopulateGrid();
        ResourceManager::GetInstance().UnloadImages();
        ResourceManager::GetInstance().UnloadShaderFileText();
    };

} // namespace sage