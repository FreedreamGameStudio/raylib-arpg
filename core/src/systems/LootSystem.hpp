//
// Created by Steve Wheeler on 15/01/2025.
//

#pragma once

#include "entt/entt.hpp"

namespace sage
{

    class Systems;
    class Window;

    static constexpr auto LOOT_DISTANCE = 30;

    class LootSystem
    {
        entt::registry* registry;
        Systems* sys;
        entt::entity chest;
        Window* openLootWindow;

        void onChestClick(entt::entity clickedChest);

      public:
        [[nodiscard]] bool InLootRange() const;
        void Update();
        LootSystem(entt::registry* _registry, Systems* _sys);
    };

} // namespace sage
