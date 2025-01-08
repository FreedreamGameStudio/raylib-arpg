﻿//
// Created by steve on 18/02/2024.
//

#include "Application.hpp"

#include "AssetManager.hpp"
#include "AudioManager.hpp"
#include "Camera.hpp"
#include "KeyMapping.hpp"
#include "scenes/ExampleScene.hpp"
#include "scenes/Scene.hpp"
#include "Serializer.hpp"
#include "Settings.hpp"
#include "Systems.hpp"
#include "systems/CleanupSystem.hpp"
#include "UserInput.hpp"

namespace sage
{

    void Application::init()
    {
        SetConfigFlags(FLAG_MSAA_4X_HINT);
        InitWindow(
            static_cast<int>(settings->GetScreenSize().x),
            static_cast<int>(settings->GetScreenSize().y),
            "Baldur's Raylib");
        settings->UpdateViewport();

        audioManager = std::make_unique<AudioManager>();

        AssetManager::GetInstance().LoadPaths(); // Init asset paths
        serializer::LoadAssetBinFile(registry.get(), "resources/assets.bin");
        serializer::LoadMap(registry.get(), "resources/dungeon-map.bin");
        // serializer::LoadMap(registry.get(), "resources/cave.bin");

        auto icon = ResourceManager::GetInstance().GetImage("IMG_APPLICATIONICON");
        SetWindowIcon(icon.GetImage());
        ResourceManager::GetInstance().ImageUnload("IMG_APPLICATIONICON");
        HideCursor();
        SetExitKey(KEY_NULL); // Disable KEY_ESCAPE to close window, X-button still works

        scene =
            std::make_unique<ExampleScene>(registry.get(), keyMapping.get(), settings.get(), audioManager.get());
        const auto viewport = settings->GetViewPort();
        renderTexture = LoadRenderTexture(static_cast<int>(viewport.x), static_cast<int>(viewport.y));
        renderTexture2d = LoadRenderTexture(static_cast<int>(viewport.x), static_cast<int>(viewport.y));
    }

    void Application::handleScreenUpdate()
    {
        if (settings->toggleFullScreenRequested)
        {
            const auto prev = settings->GetViewPort();

#ifdef __APPLE__
            if (!IsWindowFullscreen())
            {
                const int monitor = GetCurrentMonitor();
                SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
                settings->SetScreenSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));

                ToggleFullscreen();
            }
            else
            {
                ToggleFullscreen();
                settings->ResetToUserDefined();
                const auto screen = settings->GetScreenSize();
                SetWindowSize(static_cast<int>(screen.x), static_cast<int>(screen.y));
            }
#else
            bool maximized = GetScreenWidth() == GetMonitorWidth(GetCurrentMonitor()) &&
                             GetScreenHeight() == GetMonitorHeight(GetCurrentMonitor());
            if (!maximized)
            {
                int monitor = GetCurrentMonitor();
                SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
                settings->SetScreenSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
                ToggleBorderlessWindowed();
            }
            else
            {
                ToggleBorderlessWindowed();
                settings->ResetToUserDefined();
                const auto screen = settings->GetScreenSize();
                SetWindowSize(static_cast<int>(screen.x), static_cast<int>(screen.y));
            }
#endif
            settings->toggleFullScreenRequested = false;
            const auto viewport = settings->GetViewPort();
            scene->data->userInput->onWindowUpdate.Publish(prev, viewport);
            UnloadTexture(renderTexture.texture);
            UnloadTexture(renderTexture2d.texture);
            renderTexture = LoadRenderTexture(static_cast<int>(viewport.x), static_cast<int>(viewport.y));
            renderTexture2d = LoadRenderTexture(static_cast<int>(viewport.x), static_cast<int>(viewport.y));
        }
    }

    void Application::Update()
    {
        init();
        // scene->Init();
        SetTargetFPS(60);
        while (!exitWindow) // Detect window close button or ESC key
        {

            if (WindowShouldClose() || IsKeyPressed(KEY_ESCAPE)) exitWindowRequested = true;

            if (exitWindowRequested)
            {
                // A request for close window has been issued, we can save data before closing
                // or just show a message asking for confirmation

                if (IsKeyPressed(KEY_Y))
                    exitWindow = true;
                else if (IsKeyPressed(KEY_N))
                    exitWindowRequested = false;
            }

            scene->Update();
            cleanupSystem->Execute();
            draw();
            handleScreenUpdate();
        }
    }

    void Application::draw()
    {

        BeginTextureMode(renderTexture);
        ClearBackground(BLANK);
        BeginMode3D(*scene->data->camera->getRaylibCam());
        scene->Draw3D();
        // scene->DrawDebug3D();
        EndMode3D();
        EndTextureMode();

        BeginTextureMode(renderTexture2d);
        ClearBackground(BLANK);
        scene->Draw2D();
        // scene->DrawDebug2D();
        EndTextureMode();

        BeginDrawing();

        ClearBackground(BLACK);
        auto letterbox = Vector2Subtract(settings->GetScreenSize(), settings->GetViewPort());

        // BeginShaderMode(ResourceManager::GetInstance().ShaderLoad(nullptr, nullptr));
        const auto [width, height] = settings->GetViewPort();
        DrawTextureRec(renderTexture.texture, {0, 0, width, -height}, {0, letterbox.y / 2}, WHITE);
        // EndShaderMode();

        DrawTextureRec(renderTexture2d.texture, {0, 0, width, -height}, {0, letterbox.y / 2}, WHITE);

        if (exitWindowRequested)
        {
            DrawRectangle(0, letterbox.y / 2 + 100, width, 200, BLACK);
            auto textSize = MeasureText("Are you sure you want to exit program? [Y/N]", 30);
            DrawText("Are you sure you want to exit program? [Y/N]", (width - textSize) / 2, 180, 30, WHITE);
        }
        DrawFPS(settings->GetScreenSize().x - settings->ScaleValueWidth(120), 10);
        EndDrawing();
    };

    void Application::cleanup()
    {
        CloseWindow();
    }

    Application::~Application()
    {
        cleanup();
    }

    Application::Application()
        : registry(std::make_unique<entt::registry>()),
          cleanupSystem(std::make_unique<CleanupSystem>(registry.get())),
          settings(std::make_unique<Settings>()),
          keyMapping(std::make_unique<KeyMapping>())
    {
    }
} // namespace sage
