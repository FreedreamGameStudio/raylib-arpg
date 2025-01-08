//
// Created by Steve Wheeler on 27/03/2024.
//

#include "GameData.hpp"

#include "Serializer.hpp"

#include "system_includes.hpp"

namespace sage
{
    GameData::GameData(
        entt::registry* _registry, KeyMapping* _keyMapping, Settings* _settings, AudioManager* _audioManager)
        : registry(_registry),
          settings(_settings),
          audioManager(_audioManager),
          userInput(std::make_unique<UserInput>(_keyMapping, _settings)),
          cursor(std::make_unique<Cursor>(_registry, this)),
          camera(std::make_unique<Camera>(_registry, userInput.get(), this)),
          lightSubSystem(std::make_unique<LightManager>(_registry, camera.get())),
          uiEngine(std::make_unique<GameUIEngine>(_registry, this)),
          renderSystem(std::make_unique<RenderSystem>(_registry)),
          collisionSystem(std::make_unique<CollisionSystem>(_registry)),
          navigationGridSystem(std::make_unique<NavigationGridSystem>(_registry, collisionSystem.get())),
          actorMovementSystem(std::make_unique<ActorMovementSystem>(_registry, this)),
          controllableActorSystem(std::make_unique<ControllableActorSystem>(_registry, this)),
          animationSystem(std::make_unique<AnimationSystem>(_registry)),
          dialogSystem(std::make_unique<DialogSystem>(_registry, this)),
          dialogFactory(std::make_unique<DialogFactory>(_registry, this)),
          npcManager(std::make_unique<NPCManager>(_registry, this)),
          healthBarSystem(std::make_unique<HealthBarSystem>(_registry, camera.get())),
          stateMachines(std::make_unique<StateMachines>(_registry, this)),
          abilityRegistry(std::make_unique<AbilityFactory>(_registry, this)),
          itemFactory(std::make_unique<ItemFactory>(_registry)),
          playerAbilitySystem(std::make_unique<PlayerAbilitySystem>(_registry, this)),
          combatSystem(std::make_unique<CombatSystem>(_registry)),
          timerSystem(std::make_unique<TimerSystem>(_registry)),
          inventorySystem(std::make_unique<InventorySystem>(_registry, this)),
          partySystem(std::make_unique<PartySystem>(_registry, this)),
          equipmentSystem(std::make_unique<EquipmentSystem>(_registry, this)),
          uberShaderSystem(std::make_unique<UberShaderSystem>(_registry, this)),
          cursorClickIndicator(std::make_unique<CursorClickIndicator>(_registry, this)),
          questManager(std::make_unique<QuestManager>(_registry, this)),
          doorSystem(std::make_unique<DoorSystem>(_registry, this)),
          fullscreenTextOverlayFactory(std::make_unique<FullscreenTextOverlayFactory>(this)),
          contextualDialogSystem(std::make_unique<ContextualDialogSystem>(registry, this)),
          spatialAudioSystem(std::make_unique<SpatialAudioSystem>(registry, this))
    {
        // TODO: Move GameData out of Scene and into Application
        // Minus lights and timers, I'm not sure if anything would suffer from this
    }
} // namespace sage
