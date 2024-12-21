#include "Scene.hpp"

// NB: We have to include all the headers required to build GameData
#include "AbilityFactory.hpp"
#include "Camera.hpp"
#include "components/EquipmentComponent.hpp"
#include "components/PartyMemberComponent.hpp"
#include "components/Renderable.hpp"
#include "components/Spawner.hpp"
#include "components/UberShaderComponent.hpp"
#include "Cursor.hpp"
#include "CursorClickIndicator.hpp"
#include "DialogFactory.hpp"
#include "EntityReflectionSignalRouter.hpp"
#include "GameData.hpp"
#include "GameObjectFactory.hpp"
#include "GameUiEngine.hpp"
#include "GameUiFactory.hpp"
#include "ItemFactory.hpp"
#include "LightManager.hpp"
#include "NpcManager.hpp"
#include "Serializer.hpp"
#include "systems/ActorMovementSystem.hpp"
#include "systems/AnimationSystem.hpp"
#include "systems/CollisionSystem.hpp"
#include "systems/CombatSystem.hpp"
#include "systems/ControllableActorSystem.hpp"
#include "systems/DialogSystem.hpp"
#include "systems/EquipmentSystem.hpp"
#include "systems/HealthBarSystem.hpp"
#include "systems/InventorySystem.hpp"
#include "systems/NavigationGridSystem.hpp"
#include "systems/PartySystem.hpp"
#include "systems/PlayerAbilitySystem.hpp"
#include "systems/RenderSystem.hpp"
#include "systems/states/StateMachines.hpp"
#include "systems/TimerSystem.hpp"
#include "systems/UberShaderSystem.hpp"
#include "UserInput.hpp"

#include "abilities/vfx/SpiralFountainVFX.hpp"

namespace sage
{

    void Scene::Update()
    {
        data->renderSystem->Update();
        data->camera->Update();
        data->userInput->ListenForInput();
        data->cursor->Update();
        data->lightSubSystem->Update();
        data->uiEngine->Update();
        spiral->Update(GetFrameTime());
        data->cursorClickIndicator->Update();
    }

    void Scene::DrawDebug3D()
    {
        data->cursor->DrawDebug();
        data->camera->DrawDebug();
        data->lightSubSystem->DrawDebugLights();
    }

    void Scene::Draw3D()
    {
        data->renderSystem->Draw();
        data->cursor->Draw3D();
        // spiral->Draw3D();
    };

    void Scene::DrawDebug2D()
    {
        data->uiEngine->DrawDebug2D();
    }

    void Scene::Draw2D()
    {
        data->uiEngine->Draw2D();
        data->cursor->Draw2D();
    }

    Scene::~Scene()
    {
        // ResourceManager::GetInstance().UnloadAll();
    }

    BoundingBox calculateFloorSize(entt::registry* registry)
    {
        auto view = registry->view<Collideable>();
        std::vector<Collideable*> floorMeshes;

        for (const auto& entity : view)
        {
            auto& col = registry->get<Collideable>(entity);
            if (col.collisionLayer == CollisionLayer::FLOORSIMPLE ||
                col.collisionLayer == CollisionLayer::FLOORCOMPLEX)
            {
                floorMeshes.push_back(&col);
            }
        }

        BoundingBox mapBB{Vector3{0, 0, 0}, Vector3{0, 0, 0}}; // min, max
        for (const auto& col : floorMeshes)
        {
            if (col->worldBoundingBox.min.x <= mapBB.min.x && col->worldBoundingBox.min.z <= mapBB.min.z)
            {
                mapBB.min = col->worldBoundingBox.min;
            }
            if (col->worldBoundingBox.max.x >= mapBB.max.x && col->worldBoundingBox.max.z >= mapBB.max.z)
            {
                mapBB.max = col->worldBoundingBox.max;
            }
        }
        mapBB.min.y = 0.1f;
        mapBB.max.y = 0.1f;
        return mapBB;
    }

    Scene::Scene(entt::registry* _registry, KeyMapping* _keyMapping, Settings* _settings)
        : registry(_registry), data(std::make_unique<GameData>(_registry, _keyMapping, _settings))
    {

        serializer::DeserializeJsonFile<ItemFactory>("resources/items.json", *data->itemFactory);

        BoundingBox mapBB = calculateFloorSize(registry);
        // int slices = std::ceil(std::max(mapBB.max.x, mapBB.max.z) - std::min(mapBB.min.x, mapBB.min.z)) + 1;
        int slices = 1500;
        data->navigationGridSystem->Init(slices, 1.0f);

        auto heightMap = ResourceManager::GetInstance().GetImage(AssetID::GEN_IMG_HEIGHTMAP);
        auto normalMap = ResourceManager::GetInstance().GetImage(AssetID::GEN_IMG_NORMALMAP);
        data->navigationGridSystem->PopulateGrid(heightMap, normalMap);

        // NB: Dependent on *only* the map/static meshes having been loaded at this point
        for (const auto view = registry->view<Renderable>(); auto entity : view)
        {
            auto& uber = registry->emplace<UberShaderComponent>(
                entity, registry->get<Renderable>(entity).GetModel()->GetMaterialCount());
            uber.SetFlagAll(UberShaderComponent::Flags::Lit);
        }

        entt::entity firstPlayer = entt::null;

        const auto view = registry->view<Spawner>();
        for (auto& entity : view)
        {
            auto& spawner = registry->get<Spawner>(entity);
            if (spawner.spawnerType == SpawnerType::PLAYER)
            {
                firstPlayer = GameObjectFactory::createPlayer(registry, data.get(), spawner.pos, "Player");
            }
            else if (spawner.spawnerType == SpawnerType::GOBLIN)
            {
                GameObjectFactory::createEnemy(registry, data.get(), spawner.pos, "Goblin");
            }
        }

        // auto p2 = GameObjectFactory::createPlayer(registry, data.get(), Vector3Zero(), "Player 2");
        // auto p3 = GameObjectFactory::createPlayer(registry, data.get(), {10, 0, 10}, "Player 3");
        data->controllableActorSystem->SetSelectedActor(firstPlayer);

        GameObjectFactory::createEmissiveCrystal(registry, data.get(), {30, 0, 20}, "Crystal");

        // GameObjectFactory::createPlayer(registry, data.get(), {25, 0, 10}, "Player 3");
        // data->controllableActorSystem->SetSelectedActor(data->partySystem->GetMember(1).entity);
        //  registry->erase<Spawner>(view.begin(), view.end());

        // Clear any CPU resources that are no longer needed
        // ResourceManager::GetInstance().UnloadImages();
        // ResourceManager::GetInstance().UnloadShaderFileText();

        ResourceManager::GetInstance().FontLoadFromFile(
            "resources/fonts/LibreBaskerville/LibreBaskerville-Bold.ttf");
        ResourceManager::GetInstance().ImageLoadFromFile("resources/icons/ui/equipment.png");
        ResourceManager::GetInstance().ImageLoadFromFile("resources/icons/ui/hammer.png");
        ResourceManager::GetInstance().ImageLoadFromFile("resources/icons/ui/inventory.png");
        ResourceManager::GetInstance().ImageLoadFromFile("resources/icons/ui/spellbook.png");
        ResourceManager::GetInstance().ImageLoadFromFile("resources/transpixel.png");
        ResourceManager::GetInstance().ImageLoadFromFile("resources/textures/9patch.png");
        ResourceManager::GetInstance().ImageLoadFromFile("resources/icons/ui/empty.png");
        ResourceManager::GetInstance().ImageLoadFromFile("resources/textures/ninepatch_button.png");
        ResourceManager::GetInstance().ImageLoadFromFile("resources/icon.png");
        ResourceManager::GetInstance().ImageLoadFromFile("resources/textures/ui/frame.png");
        ResourceManager::GetInstance().ImageLoadFromFile("resources/textures/ui/empty-inv_slot.png");
        ResourceManager::GetInstance().ImageLoadFromFile("resources/textures/ui/window_hud.png");
        ResourceManager::GetInstance().ImageLoadFromFile("resources/textures/ui/window_dialogue.png");
        ResourceManager::GetInstance().ImageLoadFromFile("resources/textures/ui/inventory-bg.png");
        ResourceManager::GetInstance().ImageLoadFromFile("resources/textures/ui/scroll-bg.png");

        const auto abilityUi = GameUiFactory::CreateAbilityRow(data->uiEngine.get());
        auto w = data->settings->TARGET_SCREEN_WIDTH * 0.3;
        auto h = data->settings->TARGET_SCREEN_HEIGHT * 0.6;
        auto* inventoryWindow =
            GameUiFactory::CreateInventoryWindow(registry, data->uiEngine.get(), {200, 50}, w, h);
        auto* equipmentWindow =
            GameUiFactory::CreateCharacterWindow(registry, data->uiEngine.get(), {700, 50}, w, h);
        entt::sink sink{data->userInput->keyIPressed};
        sink.connect<&Window::ToggleHide>(*inventoryWindow);
        entt::sink sink2{data->userInput->keyCPressed};
        sink2.connect<&Window::ToggleHide>(*equipmentWindow);

        auto* window3 = GameUiFactory::CreatePartyPortraitsColumn(data->uiEngine.get());
        GameUiFactory::CreateGameWindowButtons(data->uiEngine.get(), inventoryWindow, equipmentWindow);

        spiral = std::make_unique<SpiralFountainVFX>(data.get(), nullptr);
        spiral->InitSystem();
    };

} // namespace sage