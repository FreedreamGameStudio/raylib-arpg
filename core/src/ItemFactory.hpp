//
// Created by Steve Wheeler on 30/11/2024.
//

#pragma once

#include "components/ItemComponent.hpp"
#include "Serializer.hpp"
#include "ViewSerializer.hpp"

#include "cereal/cereal.hpp"
#include "cereal/types/string.hpp"
#include "cereal/types/vector.hpp"
#include "entt/core/type_traits.hpp"
#include <cereal/archives/json.hpp>

#include <magic_enum.hpp>
#include <unordered_map>
#include <vector>

#include <entt/entt.hpp>

namespace sage
{
    class GameData;

    class ItemFactory
    {
        entt::registry* registry;
        GameData* gameData;
        std::unordered_map<std::string, ItemComponent> itemMap{};

      public:
        void CreateSword();
        void CreateDagger();
        [[nodiscard]] entt::entity GetItem(const std::string& name) const;

        template <class Archive>
        void save(Archive& archive) const
        {
            std::vector<ItemComponent> items;
            items.reserve(itemMap.size());
            for (auto& [k, v] : itemMap)
            {
                items.emplace_back(v);
            }

            // Saves all current item components
            archive(items);
        }

        template <class Archive>
        void load(Archive& archive)
        {
            std::vector<ItemComponent> items;
            items.reserve(itemMap.size());
            // Saves all current item components
            archive(items);

            for (auto& item : items)
            {
                itemMap.emplace(item.name, item);
            }
        }

        ItemFactory(entt::registry* _registry, GameData* _gameData);
    };

} // namespace sage
