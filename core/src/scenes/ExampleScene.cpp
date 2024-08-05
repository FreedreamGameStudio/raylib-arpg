//
// Created by Steve Wheeler on 27/03/2024.
//

#include "ExampleScene.hpp"
#include "../GameObjectFactory.hpp"

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
		data->stateSystems->Update();
        data->abilitySystem->Update();
		data->timerManager->Update();
		const auto& playerTransform = registry->get<sgTransform>(data->controllableActorSystem->GetControlledActor());
		fountain->SetOrigin(playerTransform.position());
		fountain->Update(GetFrameTime());
		if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
		{
			explosion->Restart();
			explosion->SetOrigin(data->cursor->terrainCollision().point);
		}
		explosion->Update();
	}

	void ExampleScene::Draw2D()
	{
		data->dialogueSystem->Draw2D();
		Scene::Draw2D();
	}

	void ExampleScene::Draw3D()
	{
		data->healthBarSystem->Draw3D();
		data->stateSystems->Draw3D();
		Scene::Draw3D();
		fountain->Draw3D();
		data->abilitySystem->Draw3D();
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

	ExampleScene::ExampleScene(entt::registry* _registry, std::unique_ptr<GameData> _data, const std::string& mapPath) :
		Scene(_registry, std::move(_data), mapPath)
	{       
        lightSubSystem->lights[0] = CreateLight(LIGHT_POINT, {0, 25, 0}, Vector3Zero(), WHITE, lightSubSystem->shader);
        // std::string mapPath = "resources/models/obj/level-basic.obj";
        auto playerId = GameObjectFactory::createPlayer(registry, data.get(), {30.0f, 0, 20.0f}, "Player");
        auto knight = GameObjectFactory::createKnight(registry, data.get(), {0.0f, 0, 20.0f}, "Knight");

		// TODO: tmp
		const auto& col = registry->get<Collideable>(knight);
		data->navigationGridSystem->MarkSquareAreaOccupied(col.worldBoundingBox, true, knight);
		
		fountain = std::make_unique<SpiralFountainVFX>(data->camera->getRaylibCam());
		explosion = std::make_unique<Explosion>(registry);
		explosion->SetOrigin({0, 3, 0});
	}
} // sage
