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

    static constexpr unsigned int PARTY_MEMBER_MAX = 4;

    class PartySystem
    {
        entt::registry* registry;
        GameData* gameData;
        std::vector<entt::entity> party;

      public:
        void AddMember(entt::entity member);
        void RemoveMember(entt::entity entity);
        [[nodiscard]] entt::entity GetMember(unsigned int memberNumber) const;
        [[nodiscard]] unsigned int GetSize() const;
        PartySystem(entt::registry* _registry, GameData* _gameData);
    };
} // namespace sage
