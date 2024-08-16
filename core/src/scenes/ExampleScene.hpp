//
// Created by Steve Wheeler on 27/03/2024.
//

#pragma once

#include "GameData.hpp"
#include "Scene.hpp"

#include "abilities/vfx/Explosion.hpp"
#include "abilities/vfx/SpiralFountainVFX.hpp"

#include <entt/entt.hpp>

#include <memory>
#include <string>
#include <vector>

namespace sage
{
    class ExampleScene : public Scene
    {
        std::unique_ptr<SpiralFountainVFX> fountain;
        std::unique_ptr<Explosion> explosion;

      public:
        void Update() override;
        void Draw2D() override;
        void Draw3D() override;
        void DrawDebug() override;
        ~ExampleScene() override;
        ExampleScene(entt::registry* _registry, std::unique_ptr<GameData> _data, const std::string& mapPath);
    };
} // namespace sage
