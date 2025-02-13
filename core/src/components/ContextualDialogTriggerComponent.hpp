//
// Created by Steve Wheeler on 03/01/2025.
//

#pragma once

#include "entt/entt.hpp"
#include "Event.hpp"

namespace sage
{

    class ContextualDialogTriggerComponent
    {
        // Could target a specific player or just the leader of the party
        bool triggered = false;
        bool loop = false;
        bool shouldRetrigger = false;

      public:
        Event<> onTrigger;
        entt::entity speaker{}; // Entity that speaks when triggered
        float distance = 75.0f; // Distance for the dialog to trigger

        [[nodiscard]] bool HasTriggered() const
        {
            return triggered;
        }

        [[nodiscard]] bool ShouldLoop() const
        {
            return loop;
        }

        void SetTriggered(bool _triggered = true)
        {
            if (_triggered)
            {
                onTrigger.Publish();
            }
            triggered = _triggered;
        }

        friend class ContextualDialogSystem;
    };

} // namespace sage
