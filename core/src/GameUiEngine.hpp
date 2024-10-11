//
// Created by steve on 02/10/2024.
//

#pragma once

#include "raylib.h"
#include "Timer.hpp"

#include <entt/entt.hpp>
#include <iostream>
#include <optional>
#include <variant>
#include <vector>

namespace sage
{
    class ControllableActorSystem;
}
namespace sage
{
    class GameUIEngine;
    class PlayerAbilitySystem;

    struct TableCell;
    struct Window;
    struct TableRow;
    struct Table;

    struct Settings;
    class UserInput;
    class Cursor;

    struct Dimensions
    {
        float width;
        float height;
    };

    enum class WindowTableAlignment
    {
        STACK_VERTICAL,
        STACK_HORIZONTAL
    };

    enum class HoriAlignment
    {
        LEFT,
        RIGHT,
        CENTER,
        WINDOW_CENTER
    };

    enum class VertAlignment
    {
        TOP,
        MIDDLE,
        BOTTOM
    };

    struct Padding
    {
        float up = 0;
        float down = 0;
        float left = 0;
        float right = 0;
    };

    struct Margin
    {
        float up = 0;
        float down = 0;
        float left = 0;
        float right = 0;
    };

    class UIElement
    {
      public:
        virtual ~UIElement() = default;
    };

    template <typename Child, typename Parent>
    struct TableElement : UIElement
    {
      protected:
        Padding padding;
        Margin margin;

      public:
        Parent* parent;
        Rectangle rec{};
        Child children;

        std::optional<Texture> tex{};
        std::optional<NPatchInfo> nPatchInfo{};

        virtual void UpdateChildren() = 0;
        virtual void DrawDebug2D() = 0;
        virtual void Draw2D()
        {
            if (tex.has_value())
            {
                if (nPatchInfo.has_value())
                {
                    DrawTextureNPatch(
                        tex.value(),
                        nPatchInfo.value(),
                        rec,
                        {0.0f, 0.0f},
                        0.0f,
                        WHITE); // Use {0.0f, 0.0f} for origin
                }
                else
                {
                    DrawTexture(tex.value(), rec.x, rec.y, WHITE);
                }
            }
        }

        void SetPaddingPixel(const Padding& _padding)
        {
            padding = _padding;
            UpdateChildren();
        }

        // Set padding in percent of parent
        void SetPaddingPercent(const Padding& _padding)
        {
            padding.up = rec.height * (_padding.up / 100);
            padding.down = rec.height * (_padding.down / 100);
            padding.left = rec.width * (_padding.left / 100);
            padding.right = rec.width * (_padding.right / 100);
            UpdateChildren();
        }

        // Returns pixel value of padding
        [[nodiscard]] const Padding& GetPadding() const
        {
            return padding;
        }

        [[nodiscard]] Window* GetWindow()
        {
            TableElement* current = this;
            while (current->parent != nullptr)
            {
                current = reinterpret_cast<TableElement*>(current->parent);
            }

            return reinterpret_cast<Window*>(current);
        }

        TableElement() = default;
        TableElement(const TableElement&) = default;
        TableElement(TableElement&&) noexcept = default;
        TableElement& operator=(const TableElement&) = default;
        TableElement& operator=(TableElement&&) noexcept = default;
        virtual ~TableElement() = default;
    };

    struct CellElement : UIElement
    {
        TableCell* parent{};
        Rectangle rec{};
        Texture tex{};
        VertAlignment vertAlignment = VertAlignment::TOP;
        HoriAlignment horiAlignment = HoriAlignment::LEFT;
        bool mouseHover = false;

        void SetVertAlignment(VertAlignment alignment);
        void SetHoriAlignment(HoriAlignment alignment);

        entt::sigh<void()> onStartHover;
        entt::sigh<void()> onEndHover;
        entt::sigh<void()> onMouseClicked;

        bool draggable = false;
        bool canReceiveDragDrops = false;
        bool beingDragged = false;
        entt::sigh<void()> onDragStart;
        entt::sigh<void()> onDragEnd;

        virtual void OnDragDropHere(UIElement* droppedElement)
        {
            if (!canReceiveDragDrops) return;

            std::cout << "Reached here \n";
        }

        virtual void OnMouseStartHover()
        {
            mouseHover = true;
            onStartHover.publish();
        };

        virtual void OnMouseContinueHover()
        {
        }

        virtual void OnMouseStopHover()
        {
            mouseHover = false;
            onEndHover.publish();
        };

        virtual void OnMouseClick()
        {
            onMouseClicked.publish();
        };

        virtual void UpdateDimensions() = 0;
        virtual void Draw2D() = 0;

        CellElement() = default;
        CellElement(const CellElement&) = default;
        CellElement(CellElement&&) noexcept = default;
        CellElement& operator=(const CellElement&) = default;
        CellElement& operator=(CellElement&&) noexcept = default;
        virtual ~CellElement() = default;
    };

    struct TextBox : public CellElement
    {
        enum class OverflowBehaviour
        {
            SHRINK_TO_FIT,
            WORD_WRAP
        };
        OverflowBehaviour overflowBehaviour = OverflowBehaviour::SHRINK_TO_FIT;
        float fontSize = 12;
        float fontSpacing = 2;
        Font font = GetFontDefault();
        // color?
        std::string content;

        void SetOverflowBehaviour(OverflowBehaviour _behaviour);
        void UpdateDimensions() override;
        void Draw2D() override;
        ~TextBox() override = default;
    };

    class ImageBox : public CellElement
    {
      public:
        enum class OverflowBehaviour
        {
            SHRINK_TO_FIT,
            SHRINK_ROW_TO_FIT
        };
        void OnMouseStartHover() override;
        void OnMouseStopHover() override;
        void OnMouseClick() override;
        void SetOverflowBehaviour(OverflowBehaviour _behaviour);
        void SetGrayscale();
        void RemoveShader();
        void UpdateDimensions() override;
        void Draw2D() override;
        ~ImageBox() override = default;

      protected:
        OverflowBehaviour overflowBehaviour = OverflowBehaviour::SHRINK_TO_FIT;
        std::optional<Shader> shader;

      private:
        [[nodiscard]] Dimensions calculateAvailableSpace() const;
        [[nodiscard]] float calculateAspectRatio() const;
        [[nodiscard]] Dimensions calculateInitialDimensions(const Dimensions& space) const;
        [[nodiscard]] Vector2 calculateAlignmentOffset(
            const Dimensions& dimensions, const Dimensions& space) const;
        void updateRectangle(const Dimensions& dimensions, const Vector2& offset, const Dimensions& space);
        void shrinkRowToFit() const;
        [[nodiscard]] Dimensions handleOverflow(const Dimensions& dimensions, const Dimensions& space) const;
    };

    struct CloseButton final : public ImageBox
    {
        ~CloseButton() override = default;
        void OnMouseClick() override;
    };

    struct TitleBar final : public TextBox
    {
        ~TitleBar() override = default;
    };

    struct AbilitySlot : public ImageBox
    {
        // TODO: tooltip timer does not reset on drag
        double hoverTimer = 0;
        float hoverTimerThreshold = 0.8;
        std::optional<Window*> tooltipWindow;
        PlayerAbilitySystem* playerAbilitySystem;
        int slotNumber;
        void SetAbilityInfo();
        void OnDragDropHere(UIElement* droppedElement) override;
        void OnMouseStartHover() override;
        void OnMouseContinueHover() override;
        void OnMouseStopHover() override;
    };

    struct InventorySlot : public ImageBox
    {
        entt::registry* registry;
        ControllableActorSystem* controllableActorSystem;
        double hoverTimer = 0;
        float hoverTimerThreshold = 0.8;
        std::optional<Window*> tooltipWindow;
        unsigned int row;
        unsigned int col;
        void SetItemInfo();
        void OnDragDropHere(UIElement* droppedElement) override;
        void OnMouseStartHover() override;
        void OnMouseContinueHover() override;
        void OnMouseStopHover() override;
    };

    struct TableCell final : public TableElement<std::unique_ptr<CellElement>, TableRow>
    {
        float requestedWidth{};
        bool autoSize = true;
        TextBox* CreateTextbox(
            const std::string& _content,
            float fontSize = 16,
            TextBox::OverflowBehaviour overflowBehaviour = TextBox::OverflowBehaviour::SHRINK_TO_FIT);
        TitleBar* CreateTitleBar(const std::string& _title, float fontSize);
        ImageBox* CreateImagebox(Texture _tex);
        CloseButton* CreateCloseButton(Texture _tex);
        AbilitySlot* CreateAbilitySlot(PlayerAbilitySystem* _playerAbilitySystem, int _slotNumber);
        InventorySlot* CreateInventorySlot(
            entt::registry* _registry,
            ControllableActorSystem* _controllableActorSystem,
            unsigned int row,
            unsigned int col);
        void UpdateChildren() override;
        void DrawDebug2D() override;
        void Draw2D() override;
        ~TableCell() override = default;
    };

    struct TableRow final : public TableElement<std::vector<std::unique_ptr<TableCell>>, Table>
    {
        float requestedHeight{};
        bool autoSize = true;
        TableCell* CreateTableCell();
        TableCell* CreateTableCell(float _requestedWidth);
        void UpdateChildren() override;
        void DrawDebug2D() override;
        void Draw2D() override;
        ~TableRow() override = default;
    };

    struct Table : public TableElement<std::vector<std::unique_ptr<TableRow>>, Window>
    {
        float requestedHeight{};
        float requestedWidth{};
        bool autoSize = true;
        TableRow* CreateTableRow();
        TableRow* CreateTableRow(float _requestedHeight);
        void UpdateChildren() override;
        void DrawDebug2D() override;
        void Draw2D() override;
        ~Table() override = default;
    };

    struct TableGrid final : public Table
    {
        float cellSpacing = 0;
        void UpdateChildren() override;
    };

    struct Window : TableElement<std::vector<std::unique_ptr<Table>>, void>
    {
        float widthPercent = 0;  // Width as percent of screen space
        float heightPercent = 0; // Height as percent of screen space

        bool hidden = false;
        bool markForRemoval = false;
        bool mouseHover = false;

        GameUIEngine* uiEngine;
        const Settings* settings; // for screen width/height
        entt::sigh<void()> onMouseStartHover;
        entt::sigh<void()> onMouseStopHover;
        WindowTableAlignment tableAlignment = WindowTableAlignment::STACK_HORIZONTAL;

        // Texture mainNPatchTexture; // npatch texture used by elements in window

        virtual void OnMouseStartHover();
        virtual void OnMouseStopHover();

        [[nodiscard]] Dimensions GetDimensions() const;
        void SetDimensionsPercent(float _widthPercent, float _heightPercent);
        virtual void SetPosition(float x, float y);
        [[nodiscard]] Vector2 GetPosition() const;

        TableGrid* CreateTableGrid(int rows, int cols, float cellSpacing = 0);
        Table* CreateTable();
        Table* CreateTable(float requestedWidthOrHeight);
        void Remove();
        virtual void OnScreenSizeChange();
        void DrawDebug2D() override;
        void Draw2D() override;
        void UpdateChildren() override;
    };

    struct WindowDocked final : public Window
    {
        float xOffsetPercent = 0;
        float yOffsetPercent = 0;
        VertAlignment vertAlignment = VertAlignment::TOP;
        HoriAlignment horiAlignment = HoriAlignment::LEFT;

        // void SetPosition(float x, float y) override = delete;
        [[nodiscard]] Vector2 GetOffset() const;
        void SetOffsetPercent(float _xOffsetPercent, float _yOffsetPercent);
        void SetAlignment(VertAlignment vert, HoriAlignment hori);
        void OnScreenSizeChange() override;
    };

    class DraggedObject
    {
      protected:
      public:
        Vector2 mouseOffset{};
        virtual UIElement* GetElement() = 0;
        virtual ~DraggedObject() = default;
        virtual void Update() = 0;
        virtual void Draw() = 0;
        virtual void OnDrop(){};
    };

    class DraggedWindow : public DraggedObject
    {
        Window* element{};

      public:
        UIElement* GetElement() override;
        void Update() override;
        void Draw() override;
        void OnDrop() override;
        explicit DraggedWindow(Window* _window);
    };

    class DraggedCellElement : public DraggedObject
    {
        CellElement* element{};

      public:
        UIElement* GetElement() override;
        void Update() override;
        void Draw() override;
        void OnDrop() override;
        explicit DraggedCellElement(CellElement* _element);
        ~DraggedCellElement() override;
    };

    class GameUIEngine
    {
        Timer dragTimer;
        static constexpr float draggedTimerThreshold = 0.25f;
        std::vector<std::unique_ptr<Window>> windows;
        std::vector<std::unique_ptr<Window>> delayedWindows;
        Cursor* cursor;
        UserInput* userInput;
        Settings* settings;

        std::optional<std::unique_ptr<DraggedObject>> draggedElement;
        std::optional<CellElement*> hoveredDraggableElement{};

        void clearAllHover(unsigned int start, unsigned int end);
        void pruneWindows();
        void processCell(CellElement* cell, Window* window, bool& elementFound, const Vector2& mousePos);
        void processWindows(const Vector2& mousePos);
        void cleanUpDragState();

      public:
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

        void DrawDebug2D() const;
        void Draw2D() const;
        void Update();

        GameUIEngine(Settings* _settings, UserInput* _userInput, Cursor* _cursor);
    };
} // namespace sage
