#include "PlayerAutoAttack.hpp"

#include "components/Animation.hpp"
#include "components/CombatableActor.hpp"
#include "components/sgTransform.hpp"

#include <raymath.h>

namespace sage
{
    void PlayerAutoAttack::Execute(entt::entity self)
    {
        auto& c = registry->get<CombatableActor>(self);
        if (c.target == entt::null)
            return;

        auto& t = registry->get<sgTransform>(self);
        // TODO: Check if target is present
        auto& enemyPos = registry->get<sgTransform>(c.target).position();
        Vector3 direction = Vector3Subtract(enemyPos, t.position());
        float angle = atan2f(direction.x, direction.z) * RAD2DEG;
        t.SetRotation({0, angle, 0}, self);
        c.autoAttackTick = 0;

        auto& animation = registry->get<Animation>(self);
        animation.ChangeAnimationByEnum(AnimationEnum::AUTOATTACK, 4);
        if (registry->any_of<CombatableActor>(c.target))
        {
            auto& enemyCombatable = registry->get<CombatableActor>(c.target);
            AttackData attack = attackData;
            attack.attacker = self;
            attack.hit = c.target;
            enemyCombatable.onHit.publish(attack);
        }
    }

    void PlayerAutoAttack::Init()
    {
        active = true;
    }

    void PlayerAutoAttack::Cancel()
    {
        active = false;
        m_cooldownTimer = 0;
    }

    void PlayerAutoAttack::Update(entt::entity self)
    {
        if (!active)
            return;
        m_cooldownTimer += GetFrameTime();
        if (m_cooldownTimer >= m_cooldownLimit)
        {
            m_cooldownTimer = 0;
            Execute(self);
        }
    }

    PlayerAutoAttack::PlayerAutoAttack(entt::registry* _registry, CollisionSystem* _collisionSystem,
                                       TimerManager* _timerManager)
        : Ability(_registry, _collisionSystem, _timerManager)
    {
        attackData.element = AttackElement::PHYSICAL;
        attackData.damage = 10;
        m_cooldownLimit = 1.0f;
    }
} // namespace sage