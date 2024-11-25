//
// Created by Steve Wheeler on 29/02/2024.
//

#pragma once

#include "raylib.h"
#include <entt/entt.hpp>
#include <memory>

namespace sage
{
    class TextureTerrainOverlay;

    class ControllableActor
    {
        entt::entity self;

      public:
        std::unique_ptr<TextureTerrainOverlay>
            selectedIndicator; // Initialised by ControllableActorSystem on creation
        entt::sigh<void(entt::entity, entt::entity)> onEnemyLeftClick{};  // Self, Clicked enemy
        entt::sigh<void(entt::entity, entt::entity)> onEnemyRightClick{}; // Self, Clicked enemy
        entt::sigh<void(entt::entity, entt::entity)> onFloorClick{};      // Self, object clicked (can discard)
        entt::sigh<void(entt::entity, entt::entity)> onNPCLeftClick{};

        ControllableActor(entt::entity _self);
    };
} // namespace sage
