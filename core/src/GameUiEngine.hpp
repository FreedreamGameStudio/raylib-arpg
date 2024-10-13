//
// Created by steve on 02/10/2024.
//

#pragma once

#include "GameUiElements.hpp"

#include "raylib.h"
#include "Timer.hpp"
#include <iostream>
#include <optional>
#include <vector>

namespace sage
{
    class GameUIEngine;
    class UIState;

    struct Settings;
    class UserInput;
    class Cursor;

    class UIState
    {
      protected:
        CellElement* element{};
        GameUIEngine* engine{};

      public:
        virtual void Update(){};
        virtual void Draw(){};
        virtual void Enter(){};
        virtual void Exit(){};
        virtual ~UIState() = default;
        explicit UIState(CellElement* _element, GameUIEngine* _engine);
    };

    class IdleState : public UIState
    {
      public:
        void Update() override;
        ~IdleState() override = default;
        explicit IdleState(CellElement* _element, GameUIEngine* _engine);
    };

    class HoveredState : public UIState
    {
        Timer dragTimer;

      public:
        void Enter() override;
        void Exit() override;
        void Update() override;
        ~HoveredState() override = default;
        explicit HoveredState(CellElement* _element, GameUIEngine* _engine);
    };

    class PreDraggingState : public UIState
    {
        Timer dragTimer;

      public:
        void Enter() override;
        void Exit() override;
        void Update() override;
        ~PreDraggingState() override = default;
        explicit PreDraggingState(CellElement* _element, GameUIEngine* _engine);
    };

    class DraggingState : public UIState
    {
      public:
        Rectangle originalPosition{};
        Vector2 mouseOffset{};
        void Enter() override;
        void Exit() override;
        void Update() override;
        void Draw() override;
        ~DraggingState() override = default;
        explicit DraggingState(CellElement* _element, GameUIEngine* _engine);
    };

    class GameUIEngine
    {
        std::vector<std::unique_ptr<Window>> windows;
        UserInput* userInput;

        std::optional<CellElement*> draggedObject;
        std::optional<CellElement*> hoveredDraggableCellElement;

        void pruneWindows();
        void processWindows() const;

      public:
        Cursor* cursor;
        Settings* settings;

        Window* CreateWindow(
            Texture _nPatchTexture,
            float x,
            float y,
            float _widthPercent,
            float _heightPercent,
            WindowTableAlignment _alignment = WindowTableAlignment::STACK_HORIZONTAL);

        WindowDocked* CreateWindowDocked(
            Texture _nPatchTexture,
            float _xOffsetPercent,
            float _yOffsetPercent,
            float _widthPercent,
            float _heightPercent,
            WindowTableAlignment _alignment = WindowTableAlignment::STACK_HORIZONTAL);

        [[nodiscard]] bool ObjectBeingDragged() const;

        [[nodiscard]] CellElement* GetCellUnderCursor() const;
        void DrawDebug2D() const;
        void Draw2D() const;
        void Update();

        GameUIEngine(Settings* _settings, UserInput* _userInput, Cursor* _cursor);
        friend class UIState;
        friend class PreDraggingState;
        friend class DraggingState;
        friend class HoveredState;
    };
} // namespace sage
