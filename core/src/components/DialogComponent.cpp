﻿//
// Created by steve on 11/05/2024.
//

#include "DialogComponent.hpp"

#include <utility>

#include "QuestComponents.hpp"

namespace sage
{
    namespace dialog
    {

        [[nodiscard]] bool Option::HasNextIndex() const
        {
            return nextNode.has_value();
        }

        bool Option::ShouldShow()
        {
            return true;
        }

        void Option::OnSelected()
        {
            std::cout << "Calling base class. \n";
        }

        Option::Option(ConversationNode* _parent) : parent(_parent)
        {
        }

        bool ConditionalOption::ShouldShow()
        {
            return condition();
        }

        ConditionalOption::ConditionalOption(ConversationNode* _parent, std::function<bool()> _condition)
            : Option(_parent), condition(std::move(_condition))
        {
        }

        bool QuestOption::ShouldShow()
        {
            auto& quest = parent->parent->registry->get<Quest>(questId);
            return quest.HasStarted();
        }

        void QuestOption::OnSelected()
        {
            auto& task = parent->parent->registry->get<QuestTaskComponent>(parent->parent->owner);
            auto& quest = parent->parent->registry->get<Quest>(questId);
            if (quest.HasStarted())
            {
                task.MarkComplete();
            }
        }

        QuestOption::QuestOption(ConversationNode* _parent, entt::entity _questId)
            : Option(_parent), questId(_questId)
        {
        }

        void QuestStartOption::OnSelected()
        {
            auto& quest = parent->parent->registry->get<Quest>(questId);
            if (!quest.HasStarted())
            {
                quest.StartQuest();
                // TODO: Should starting the quest be a task? If so, add a task component to the quest giver and
                // uncomment below.
                // QuestOption::OnSelected();
            }
        }

        bool QuestStartOption::ShouldShow()
        {
            auto& quest = parent->parent->registry->get<Quest>(questId);
            return !quest.HasStarted();
        }

        QuestStartOption::QuestStartOption(ConversationNode* _parent, entt::entity _questId)
            : QuestOption(_parent, _questId)
        {
        }

        bool QuestHandInOption::ShouldShow()
        {
            auto& quest = parent->parent->registry->get<Quest>(questId);
            return QuestOption::ShouldShow() && quest.GetTaskCompleteCount() == quest.GetTaskCount() - 1;
        }

        QuestHandInOption::QuestHandInOption(ConversationNode* _parent, entt::entity _questId)
            : QuestOption(_parent, _questId)
        {
        }

        ConversationNode::ConversationNode(Conversation* _parent) : parent(_parent)
        {
        }

    } // namespace dialog

} // namespace sage
