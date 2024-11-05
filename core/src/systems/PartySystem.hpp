//
// Created by Steve Wheeler on 29/02/2024.
//

#pragma once

#include "entt/entt.hpp"
#include "raylib.h"

#include <vector>

namespace sage
{
    class GameData;
    struct PartyMemberComponent;

    class PartySystem
    {
        entt::registry* registry;
        GameData* gameData;
        std::vector<entt::entity> party;
        entt::entity leader{};

      public:
        void AddMember(entt::entity member);
        void RemoveMember(entt::entity entity);
        PartyMemberComponent GetMember(unsigned int memberNumber);
        void SetLeader(entt::entity entity);
        [[nodiscard]] entt::entity GetLeader() const;
        [[nodiscard]] unsigned int GetSize() const;
        PartySystem(entt::registry* _registry, GameData* _gameData);
    };
} // namespace sage
