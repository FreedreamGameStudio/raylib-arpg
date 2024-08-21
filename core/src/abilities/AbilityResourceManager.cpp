#include "AbilityResourceManager.hpp"

#include "GameData.hpp"

#include "vfx/FloorFireVFX.hpp"
#include "vfx/RainOfFireVFX.hpp"
#include "vfx/VisualFX.hpp"

#include "AbilityFunctions.hpp"
#include "AbilityIndicator.hpp"

#include "Camera.hpp"

namespace sage
{

    void AbilityResourceManager::InitializeAbilities()
    {
        abilityFunctions.emplace(AbilityFunctionEnum::SingleTargetHit, std::make_unique<SingleTargetHitFunc>());
        abilityFunctions.emplace(
            AbilityFunctionEnum::MultihitRadiusFromCursor, std::make_unique<MultihitRadiusFromCursor>());
        abilityFunctions.emplace(
            AbilityFunctionEnum::MultihitRadiusFromCaster, std::make_unique<MultihitRadiusFromCaster>());
    }

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
            _gameData->registry,
            _gameData->navigationGridSystem.get(),
            "resources/textures/cursor/rainoffire_cursor.png");
    }

    std::unique_ptr<VisualFX> AbilityResourceManager::GetVisualFX(
        AbilityData::VisualFXData data, GameData* _gameData)
    {
        if (data.name == "RainOfFire")
        {
            auto obj = std::make_unique<RainOfFireVFX>(_gameData);
            data.ptr = obj.get();
            return std::move(obj);
        }
        else if (data.name == "FloorFire")
        {
            auto obj = std::make_unique<FloorFireVFX>(_gameData);
            data.ptr = obj.get();
            return std::move(obj);
        }

        return nullptr;
    }

    AbilityFunctionEnum AbilityResourceManager::StringToExecuteFuncEnum(const std::string& name)
    {
        if (name == "SingleTargetHit")
        {
            return AbilityFunctionEnum::SingleTargetHit;
        }
        else if (name == "MultihitRadiusFromCaster")
        {
            return AbilityFunctionEnum::MultihitRadiusFromCaster;
        }
        else if (name == "MultihitRadiusFromCursor")
        {
            return AbilityFunctionEnum::MultihitRadiusFromCursor;
        }
        return AbilityFunctionEnum::SingleTargetHit; // TODO: Null?
    }

    AbilityFunction* AbilityResourceManager::GetExecuteFunc(AbilityFunctionEnum name)
    {
        if (abilityFunctions.empty())
        {
            InitializeAbilities();
        }
        auto it = abilityFunctions.find(name);
        return (it != abilityFunctions.end()) ? it->second.get() : nullptr;
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