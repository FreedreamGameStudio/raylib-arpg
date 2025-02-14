//
// Created by Steve Wheeler on 21/03/2024.
//

#include "GameObjectFactory.hpp"

#include "AbilityFactory.hpp"
#include "components/Animation.hpp"
#include "components/Collideable.hpp"
#include "components/CombatableActor.hpp"
#include "components/ControllableActor.hpp"
#include "components/DialogComponent.hpp"
#include "components/EquipmentComponent.hpp"
#include "components/HealthBar.hpp"
#include "components/InventoryComponent.hpp"
#include "components/ItemComponent.hpp"
#include "components/MoveableActor.hpp"
#include "components/PartyMemberComponent.hpp"
#include "components/Renderable.hpp"
#include "components/sgTransform.hpp"
#include "components/States.hpp"
#include "components/UberShaderComponent.hpp"
#include "ItemFactory.hpp"
#include "LightManager.hpp"
#include "ResourceManager.hpp"
#include "slib.hpp"
#include "Systems.hpp"
#include "systems/ControllableActorSystem.hpp"
#include "systems/NavigationGridSystem.hpp"
#include "systems/PartySystem.hpp"
#include "systems/PlayerAbilitySystem.hpp"
#include "Timer.hpp"

#include "raymath.h"

namespace sage
{
    BoundingBox createRectangularBoundingBox(float length, float height)
    {
        BoundingBox bb;
        // Calculate half dimensions
        float halfLength = length / 2.0f;
        // float halfHeight = height / 2.0f;

        // Set minimum bounds
        bb.min.x = -halfLength;
        bb.min.y = 0.0f;
        bb.min.z = -halfLength;

        // Set maximum bounds
        bb.max.x = halfLength;
        bb.max.y = height;
        bb.max.z = halfLength;

        return bb;
    }

    void placeActor(entt::registry* registry, entt::entity entity, Systems* sys, Vector3 position)
    {
        auto& transform = registry->get<sgTransform>(entity);
        GridSquare actorIdx{};
        sys->navigationGridSystem->WorldToGridSpace(position, actorIdx);
        auto gs = sys->navigationGridSystem->GetGridSquare(actorIdx.row, actorIdx.col);
        assert(gs);
        float height = gs->GetTerrainHeight();
        transform.SetPosition({position.x, height, position.z});
    }

    entt::entity GameObjectFactory::createDialogCutscene(
        entt::registry* registry, Vector3 position, const char* name)
    {
        entt::entity id = registry->create();

        auto& transform = registry->emplace<sgTransform>(id, id);
        transform.SetPosition(position);

        auto& renderable = registry->emplace<Renderable>(id);
        renderable.SetName(name);
        renderable.Disable();
        registry->emplace<DialogComponent>(id);

        return id;
    }

    entt::entity GameObjectFactory::createEnemy(
        entt::registry* registry, Systems* sys, Vector3 position, Vector3 rotation, const char* name)
    {
        entt::entity id = registry->create();

        auto& transform = registry->emplace<sgTransform>(id, id);
        placeActor(registry, id, sys, position);

        auto& moveable = registry->emplace<MoveableActor>(id);
        moveable.movementSpeed = 0.25f;

        Matrix modelTransform = MatrixScale(0.03f, 0.03f, 0.03f);
        auto& renderable = registry->emplace<Renderable>(
            id, ResourceManager::GetInstance().GetModelDeepCopy("MDL_ENEMY_GOBLIN"), modelTransform);
        renderable.SetName(name);
        auto& uber = registry->emplace<UberShaderComponent>(id, renderable.GetModel()->GetMaterialCount());
        uber.SetFlagAll(UberShaderComponent::Flags::Lit);
        uber.SetFlagAll(UberShaderComponent::Flags::Skinned);

        auto& animation = registry->emplace<Animation>(id, "MDL_ENEMY_GOBLIN");
        animation.animationMap[AnimationEnum::IDLE] = 1;
        animation.animationMap[AnimationEnum::DEATH] = 0;
        animation.animationMap[AnimationEnum::WALK] = 4;
        animation.animationMap[AnimationEnum::AUTOATTACK] = 2;
        animation.ChangeAnimationByEnum(AnimationEnum::IDLE);

        auto& combatable = registry->emplace<CombatableActor>(id);
        combatable.actorType = CombatableActorType::WAVEMOB;
        sys->abilityRegistry->RegisterAbility(id, AbilityEnum::ENEMY_AUTOATTACK);
        registry->emplace<HealthBar>(id);

        BoundingBox bb = createRectangularBoundingBox(3.0f, 7.0f);
        auto& collideable = registry->emplace<Collideable>(id, registry, id, bb);
        collideable.collisionLayer = CollisionLayer::ENEMY;

        transform.SetRotation(
            rotation); // TODO: Find out why this must be called after bounding box from collideable is created

        registry->emplace<WavemobState>(id);
        return id;
    }

    entt::entity GameObjectFactory::createGoblinNPC(
        entt::registry* registry, sage::Systems* sys, Vector3 position, Vector3 rotation, const char* name)
    {
        entt::entity id = registry->create();

        auto& transform = registry->emplace<sgTransform>(id, id);
        placeActor(registry, id, sys, position);

        Matrix modelTransform = MatrixScale(0.03f, 0.03f, 0.03f);
        auto& renderable = registry->emplace<Renderable>(
            id, ResourceManager::GetInstance().GetModelDeepCopy("MDL_ENEMY_GOBLIN"), modelTransform);
        renderable.SetName(name);
        auto& uber = registry->emplace<UberShaderComponent>(id, renderable.GetModel()->GetMaterialCount());
        uber.SetFlagAll(UberShaderComponent::Flags::Lit);
        uber.SetFlagAll(UberShaderComponent::Flags::Skinned);

        auto& animation = registry->emplace<Animation>(id, "MDL_ENEMY_GOBLIN");
        animation.animationMap[AnimationEnum::IDLE] = 1;
        animation.animationMap[AnimationEnum::DEATH] = 0;
        animation.animationMap[AnimationEnum::WALK] = 4;
        animation.animationMap[AnimationEnum::AUTOATTACK] = 2;
        animation.animationMap[AnimationEnum::TALK] = 1;
        animation.ChangeAnimationByEnum(AnimationEnum::IDLE);

        BoundingBox bb = createRectangularBoundingBox(3.0f, 7.0f); // Manually set bounding box dimensions
        auto& collideable = registry->emplace<Collideable>(id, registry, id, bb);
        collideable.collisionLayer = CollisionLayer::NPC;
        transform.SetRotation(rotation);
        sys->navigationGridSystem->MarkSquareAreaOccupied(collideable.worldBoundingBox, true, id);

        registry->emplace<DialogComponent>(id);

        return id;
    }

    entt::entity GameObjectFactory::createArissa(
        entt::registry* registry, Systems* sys, Vector3 position, Vector3 rotation)
    {
        entt::entity id = registry->create();

        auto& transform = registry->emplace<sgTransform>(id, id);
        placeActor(registry, id, sys, position);

        Matrix modelTransform = MatrixScale(0.035f, 0.035f, 0.035f);
        auto& renderable = registry->emplace<Renderable>(
            id, ResourceManager::GetInstance().GetModelDeepCopy("MDL_PLAYER_DEFAULT"), modelTransform);
        renderable.SetName("Arissa");
        auto& uber = registry->emplace<UberShaderComponent>(id, renderable.GetModel()->GetMaterialCount());
        uber.SetFlagAll(UberShaderComponent::Flags::Lit);
        uber.SetFlagAll(UberShaderComponent::Flags::Skinned);

        // Set animation hooks
        auto& animation = registry->emplace<Animation>(id, "MDL_PLAYER_DEFAULT");
        // TODO: I think we're going to need to move these elsewhere to make this function more generic
        animation.animationMap[AnimationEnum::WALK] = 1;
        animation.animationMap[AnimationEnum::TALK] = 2;
        animation.animationMap[AnimationEnum::AUTOATTACK] = 6;
        animation.animationMap[AnimationEnum::RUN] = 4;
        animation.animationMap[AnimationEnum::IDLE2] = 0;
        animation.animationMap[AnimationEnum::IDLE] = 10; // 11 is T-Pose, 10 is ninja idle
        animation.animationMap[AnimationEnum::SPIN] = 5;
        animation.animationMap[AnimationEnum::SLASH] = 6;
        animation.animationMap[AnimationEnum::SPELLCAST_UP] = 7;
        animation.animationMap[AnimationEnum::SPELLCAST_FWD] = 8;
        animation.animationMap[AnimationEnum::ROLL] = 9;
        animation.ChangeAnimationByEnum(AnimationEnum::IDLE);

        BoundingBox bb = createRectangularBoundingBox(3.0f, 7.0f); // Manually set bounding box dimensions
        auto& collideable = registry->emplace<Collideable>(id, registry, id, bb);
        collideable.collisionLayer = CollisionLayer::NPC;
        transform.SetRotation(rotation);
        sys->navigationGridSystem->MarkSquareAreaOccupied(collideable.worldBoundingBox, true, id);

        registry->emplace<DialogComponent>(id);

        return id;
    }

    entt::entity GameObjectFactory::createPlayer(
        entt::registry* registry, Systems* sys, Vector3 position, Vector3 rotation, const char* name)
    {
        entt::entity id = registry->create();

        auto& transform = registry->emplace<sgTransform>(id, id);
        placeActor(registry, id, sys, position);

        Matrix modelTransform = MatrixScale(0.035f, 0.035f, 0.035f);
        auto& renderable = registry->emplace<Renderable>(
            id, ResourceManager::GetInstance().GetModelDeepCopy("MDL_PLAYER_DEFAULT"), modelTransform);
        renderable.SetName(name);
        auto& uber = registry->emplace<UberShaderComponent>(id, renderable.GetModel()->GetMaterialCount());
        uber.SetFlagAll(UberShaderComponent::Flags::Lit);
        uber.SetFlagAll(UberShaderComponent::Flags::Skinned);

        auto& moveable = registry->emplace<MoveableActor>(id);
        moveable.movementSpeed = 0.35f;
        moveable.pathfindingBounds = 100;

        BoundingBox bb = createRectangularBoundingBox(3.0f, 6.5f); // Manually set bounding box dimensions
        auto& collideable = registry->emplace<Collideable>(id, registry, id, bb);
        collideable.collisionLayer = CollisionLayer::PLAYER;
        transform.SetRotation(rotation);

        // Set animation hooks
        auto& animation = registry->emplace<Animation>(id, "MDL_PLAYER_DEFAULT");
        // TODO: I think we're going to need to move these elsewhere to make this function more generic
        animation.animationMap[AnimationEnum::WALK] = 1;
        animation.animationMap[AnimationEnum::TALK] = 2;
        animation.animationMap[AnimationEnum::AUTOATTACK] = 6;
        animation.animationMap[AnimationEnum::RUN] = 4;
        animation.animationMap[AnimationEnum::IDLE2] = 0;
        animation.animationMap[AnimationEnum::IDLE] = 10; // 11 is T-Pose, 10 is ninja idle
        animation.animationMap[AnimationEnum::SPIN] = 5;
        animation.animationMap[AnimationEnum::SLASH] = 6;
        animation.animationMap[AnimationEnum::SPELLCAST_UP] = 7;
        animation.animationMap[AnimationEnum::SPELLCAST_FWD] = 8;
        animation.animationMap[AnimationEnum::ROLL] = 9;
        animation.ChangeAnimationByEnum(AnimationEnum::IDLE);

        registry->emplace<PartyMemberComponent>(id, id);
        sys->partySystem->AddMember(id);
        registry->emplace<ControllableActor>(id);
        sys->controllableActorSystem->SetSelectedActor(id);
        registry->emplace<DialogComponent>(id);

        // Combat
        auto& combatable = registry->emplace<CombatableActor>(id);
        combatable.actorType = CombatableActorType::PLAYER;

        // TODO: Move elsewhere/read from save file
        // Initialise starting abilities
        sys->playerAbilitySystem->SetSlot(0, sys->abilityRegistry->RegisterAbility(id, AbilityEnum::WHIRLWIND));
        // sys->playerAbilitySystem->SetSlot(1, sys->abilityRegistry->RegisterAbility(id,
        // AbilityEnum::RAINFOFIRE)); sys->playerAbilitySystem->SetSlot(
        //     2, sys->abilityRegistry->RegisterAbility(id, AbilityEnum::LIGHTNINGBALL));
        sys->abilityRegistry->RegisterAbility(id, AbilityEnum::PLAYER_AUTOATTACK);

        registry->emplace<InventoryComponent>(id);
        registry->emplace<EquipmentComponent>(id);

        return id;
    }

    void GameObjectFactory::createPortal(entt::registry* registry, Systems* sys, Vector3 position)
    {
        {
            entt::entity id = registry->create();

            auto& transform = registry->emplace<sgTransform>(id, id);
            GridSquare actorIdx{};
            sys->navigationGridSystem->WorldToGridSpace(position, actorIdx);
            float height =
                sys->navigationGridSystem->GetGridSquare(actorIdx.row, actorIdx.col)->GetTerrainHeight();
            transform.SetPosition({position.x, 12, position.z});
            transform.SetScale(1.0f);
            transform.SetRotation({0, 0, 0});

            auto& timer = registry->emplace<Timer>(id);
            timer.SetMaxTime(1000000);
            timer.Start();

            Texture texture = ResourceManager::GetInstance().TextureLoad("IMG_NOISE50");
            Texture texture2 = ResourceManager::GetInstance().TextureLoad("IMG_NOISE45");

            Matrix modelTransform = MatrixRotateX(90 * DEG2RAD);

            Model tmp_model = LoadModelFromMesh(GenMeshPlane(20, 20, 1, 1));
            ModelSafe model(tmp_model);
            auto& renderable = registry->emplace<Renderable>(id, std::move(model), modelTransform);
            renderable.SetName("Portal");

            Shader shader =
                ResourceManager::GetInstance().ShaderLoad(nullptr, "resources/shaders/custom/portal.fs");
            int secondsLoc = GetShaderLocation(shader, "seconds");
            renderable.GetModel()->SetTexture(texture, 0, MATERIAL_MAP_DIFFUSE);
            renderable.GetModel()->SetTexture(texture2, 0, MATERIAL_MAP_EMISSION);
            shader.locs[SHADER_LOC_MAP_EMISSION] = GetShaderLocation(shader, "texture1");
            renderable.GetModel()->SetShader(shader, 0);

            renderable.reqShaderUpdate = [sys, secondsLoc](entt::entity entity) -> void {
                auto& r = sys->registry->get<Renderable>(entity);
                auto& t = sys->registry->get<Timer>(entity);
                auto time = t.GetCurrentTime();
                SetShaderValue(r.GetModel()->GetShader(0), secondsLoc, &time, SHADER_UNIFORM_FLOAT);
            };

            BoundingBox bb = createRectangularBoundingBox(3.0f, 7.0f); // Manually set bounding box dimensions
            auto& collideable = registry->emplace<Collideable>(id, registry, id, bb);
            collideable.collisionLayer = CollisionLayer::BUILDING;
        }

        entt::entity id = registry->create();

        auto& transform = registry->emplace<sgTransform>(id, id);
        GridSquare actorIdx{};
        sys->navigationGridSystem->WorldToGridSpace(position, actorIdx);
        float height = sys->navigationGridSystem->GetGridSquare(actorIdx.row, actorIdx.col)->GetTerrainHeight();
        transform.SetPosition({position.x, height, position.z});
        transform.SetScale(10.0f);
        transform.SetRotation({0, 0, 0});

        Matrix modelTransform = MatrixIdentity();

        auto& renderable = registry->emplace<Renderable>(
            id, ResourceManager::GetInstance().GetModelCopy("MDL_BUILDING_PORTAL"), modelTransform);
        renderable.SetName("Portal Outer");
        sys->lightSubSystem->LinkRenderableToLight(id);

        BoundingBox bb = createRectangularBoundingBox(3.0f, 7.0f); // Manually set bounding box dimensions
        auto& collideable = registry->emplace<Collideable>(id, registry, id, bb);
        collideable.collisionLayer = CollisionLayer::BUILDING;
    }

    void GameObjectFactory::createWizardTower(entt::registry* registry, Systems* sys, Vector3 position)
    {
        entt::entity id = registry->create();

        auto& transform = registry->emplace<sgTransform>(id, id);
        GridSquare actorIdx{};
        sys->navigationGridSystem->WorldToGridSpace(position, actorIdx);
        float height = sys->navigationGridSystem->GetGridSquare(actorIdx.row, actorIdx.col)->GetTerrainHeight();
        transform.SetPosition({position.x, height, position.z});
        transform.SetScale(1.0f);
        transform.SetRotation({0, 0, 0});

        Matrix modelTransform = MatrixIdentity();
        auto& renderable = registry->emplace<Renderable>(
            id, ResourceManager::GetInstance().GetModelCopy("MDL_BUILDING_WIZARDTOWER1"), modelTransform);
        renderable.SetName("Wizard Tower");
        sys->lightSubSystem->LinkRenderableToLight(id);

        BoundingBox bb = renderable.GetModel()->CalcLocalBoundingBox();
        auto& collideable = registry->emplace<Collideable>(id, registry, id, bb);
        collideable.collisionLayer = CollisionLayer::BUILDING;
    }

    bool GameObjectFactory::spawnItemInWorld(
        entt::registry* registry, Systems* sys, entt::entity itemId, Vector3 position)
    {
        auto& item = registry->get<ItemComponent>(itemId);
        if (item.HasFlag(ItemFlags::QUEST)) return false;
        auto model = ResourceManager::GetInstance().GetModelCopy(item.model);
        // TODO: Need a way to store the matrix scale? Maybe in the resource packer we should store the transform
        auto& renderable =
            registry->emplace<Renderable>(itemId, std::move(model), MatrixScale(0.035, 0.035, 0.035));
        auto& transform = registry->emplace<sgTransform>(itemId, itemId);
        transform.SetPosition(position);

        // TODO: Uber shader? Lit?

        auto& collideable = registry->emplace<Collideable>(
            itemId, createRectangularBoundingBox(2.0, 2.0), transform.GetMatrixNoRot());
        collideable.collisionLayer = CollisionLayer::ITEM;
        return true;
    }

} // namespace sage
