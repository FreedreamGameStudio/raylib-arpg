#include "AbilityDefinitions.hpp"

#include "components/Animation.hpp"
#include "components/CombatableActor.hpp"
#include "components/sgTransform.hpp"

#include "utils/AbilityFunctions.hpp"

#include "raylib.h"

namespace sage
{

    static constexpr AbilityData playerAutoAbilityData{
        .element = AttackElement::PHYSICAL,
        .cooldownDuration = 1,
        .baseDamage = 10,
        .range = 5,
        .animationDelay = 0,
        .repeatable = true};

    PlayerAutoAttack::PlayerAutoAttack(entt::registry* _registry, Cursor* _cursor)
        : AutoAttackAbility(_registry, playerAutoAbilityData, _cursor)
    {
    }

    static constexpr AbilityData rainoffireAbilityData{
        .cooldownDuration = 3,
        .range = 5,
        .baseDamage = 25,
        .element = AttackElement::FIRE,
        .animationDelay = 0.75f};

    RainOfFire::RainOfFire(
        entt::registry* _registry,
        Camera* _camera,
        Cursor* _cursor,
        NavigationGridSystem* _navigationGridSystem)
        : CursorAbility(
              _registry,
              _camera,
              _cursor,
              std::make_unique<TextureTerrainOverlay>(
                  _registry,
                  _navigationGridSystem,
                  "resources/textures/cursor/rainoffire_cursor.png",
                  Color{255, 215, 0, 255},
                  "resources/shaders/glsl330/bloom.fs"),
              rainoffireAbilityData)
    {
    }

    static constexpr AbilityData wavemobAutoAbilityData{
        .cooldownDuration = 1,
        .range = 5,
        .baseDamage = 10,
        .element = AttackElement::PHYSICAL,
        .animationDelay = 0,
        .repeatable = true};

    WavemobAutoAttack::WavemobAutoAttack(entt::registry* _registry, Cursor* _cursor)
        : AutoAttackAbility(_registry, wavemobAutoAbilityData, _cursor)
    {
    }

    static constexpr AbilityData whirlwindAbilityData{
        .element = AttackElement::PHYSICAL,
        .cooldownDuration = 3,
        .baseDamage = 25,
        .range = 5,
        .animationDelay = 0.65f,
        .repeatable = false};

    void WhirlwindAbility::Execute(entt::entity self)
    {
        auto& actorTransform = registry->get<sgTransform>(self);
        Hit360AroundPoint(
            registry, self, abilityData, actorTransform.position(), whirlwindRadius);
        ChangeState(self, AbilityStateEnum::IDLE);
    }

    void WhirlwindAbility::Init(entt::entity self)
    {
        auto& animation = registry->get<Animation>(self);
        animation.ChangeAnimationByEnum(AnimationEnum::SPIN, true);
        ChangeState(self, AbilityStateEnum::AWAITING_EXECUTION);
    }

    WhirlwindAbility::WhirlwindAbility(entt::registry* _registry, Cursor* _cursor)
        : AutoAttackAbility(_registry, whirlwindAbilityData, _cursor)
    {
    }

} // namespace sage