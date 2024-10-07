//
// Created by Steve Wheeler on 27/03/2024.
//

#pragma once

#include "Scene.hpp"

#include <entt/entt.hpp>

#include <string>

namespace sage
{
    struct Settings;
    struct KeyMapping;

    class ExampleScene : public Scene
    {

      public:
        void Update() override;
        void Draw2D() override;
        void Draw3D() override;
        void DrawDebug3D() override;
        ~ExampleScene() override = default;
        ExampleScene(entt::registry* _registry, KeyMapping* _keyMapping, Settings* _settings);
    };
} // namespace sage
