#include "AbilityResourceManager.hpp"

#include "components/sgTransform.hpp"
#include "GameData.hpp"

#include "vfx/VisualFX.hpp"

#include "vfx/FireballVFX.hpp"
#include "vfx/FloorFireVFX.hpp"
#include "vfx/LightningBallVFX.hpp"
#include "vfx/RainOfFireVFX.hpp"
#include "vfx/WhirlwindVFX.hpp"

#include "AbilityFunctions.hpp"
#include "AbilityIndicator.hpp"
#include "components/Ability.hpp"

#include "Camera.hpp"

#include <vector>

namespace sage
{

    std::unique_ptr<AbilityIndicator> AbilityResourceManager::GetIndicator(
        AbilityData::IndicatorData data, GameData* _gameData)
    {
        // if (data.indicatorKey == "CircularCursor")
        // {
        //     return std::make_unique<AbilityIndicator>(
        //         registry,
        //         _gameData->navigationGridSystem.get(),
        //         "resources/textures/cursor/rainoffire_cursor.png");
        // }
        // return nullptr;

        return std::make_unique<AbilityIndicator>(
            _gameData->registry, _gameData->navigationGridSystem.get(), AssetID::IMG_RAINOFFIRE_CURSOR);
    }

    std::unique_ptr<VisualFX> AbilityResourceManager::GetVisualFX(GameData* _gameData, Ability* _ability)
    {
        std::unique_ptr<VisualFX> obj;

        // TODO: Replace "transform" with abilityEntity id and pass "caster" id and full abilityData, also.
        // TODO: Use magic enum here
        if (_ability->ad.vfx.name == "RainOfFire")
        {
            obj = std::make_unique<RainOfFireVFX>(_gameData, _ability);
        }
        else if (_ability->ad.vfx.name == "FloorFire")
        {
            obj = std::make_unique<FloorFireVFX>(_gameData, _ability);
        }
        else if (_ability->ad.vfx.name == "360SwordSlash")
        {
            obj = std::make_unique<WhirlwindVFX>(_gameData, _ability);
        }
        else if (_ability->ad.vfx.name == "LightningBall")
        {
            obj = std::make_unique<LightningBallVFX>(_gameData, _ability);
        }
        else if (_ability->ad.vfx.name == "Fireball")
        {
            obj = std::make_unique<FireballVFX>(_gameData, _ability);
        }
        return std::move(obj);
    }

    AbilityResourceManager& AbilityResourceManager::GetInstance()
    {
        static AbilityResourceManager instance;
        return instance;
    }

    AbilityResourceManager::AbilityResourceManager()
    {
    }
} // namespace sage