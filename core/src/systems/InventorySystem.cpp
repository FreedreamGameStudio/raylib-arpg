//
// Created by Steve Wheeler on 08/10/2024.
//

#include "InventorySystem.hpp"

#include "components/Collideable.hpp"
#include "components/InventoryComponent.hpp"
#include "components/Renderable.hpp"
#include "components/sgTransform.hpp"
#include "ControllableActorSystem.hpp"
#include "Cursor.hpp"
#include "GameData.hpp"

namespace sage
{

    void InventorySystem::inventoryUpdated() const
    {
        onInventoryUpdated.publish();
    }

    void InventorySystem::OnItemClicked(entt::entity entity) const
    {
        auto& inventoryComponent =
            registry->get<InventoryComponent>(gameData->controllableActorSystem->GetSelectedActor());

        if (inventoryComponent.AddItem(entity))
        {
            if (registry->any_of<Renderable>(entity))
            {
                registry->remove<Renderable>(entity);
            }
            if (registry->any_of<sgTransform>(entity))
            {
                registry->remove<sgTransform>(entity);
            }
            if (registry->any_of<Collideable>(entity))
            {
                registry->remove<Collideable>(entity);
            }
        }
        else
        {
            inventoryComponent.onInventoryFull.publish();
        }
    }

    void InventorySystem::onComponentAdded(entt::entity entity)
    {
        auto& component = registry->get<InventoryComponent>(entity);
        entt::sink sink1{component.onItemAdded};
        entt::sink sink2{component.onItemRemoved};
        sink1.connect<&InventorySystem::inventoryUpdated>(this);
        sink2.connect<&InventorySystem::inventoryUpdated>(this);
    }

    void InventorySystem::onComponentRemoved(entt::entity entity)
    {
        auto& component = registry->get<InventoryComponent>(entity);
        entt::sink sink1{component.onItemAdded};
        entt::sink sink2{component.onItemRemoved};
        sink1.disconnect<&InventorySystem::inventoryUpdated>(this);
        sink2.disconnect<&InventorySystem::inventoryUpdated>(this);
    }

    InventorySystem::InventorySystem(entt::registry* _registry, GameData* _gameData)
        : registry(_registry), gameData(_gameData)
    {
        entt::sink sink{_gameData->cursor->onItemClick};
        sink.connect<&InventorySystem::OnItemClicked>(this);

        registry->on_construct<InventoryComponent>().connect<&InventorySystem::onComponentAdded>(this);
        registry->on_destroy<InventoryComponent>().connect<&InventorySystem::onComponentRemoved>(this);
    }

} // namespace sage