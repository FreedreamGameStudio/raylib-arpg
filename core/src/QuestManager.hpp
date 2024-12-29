//
// Created by Steve Wheeler on 29/11/2024.
//

#pragma once

#include <entt/entt.hpp>

#include <string>
#include <unordered_map>

namespace sage
{

    class QuestManager
    {
        entt::registry* registry;
        std::unordered_map<std::string, entt::entity> map{};
        entt::entity createQuest(const std::string& key);

      public:
        void RemoveQuest(const std::string& key);
        entt::entity GetQuest(const std::string& key);

        explicit QuestManager(entt::registry* _registry);
    };

} // namespace sage
