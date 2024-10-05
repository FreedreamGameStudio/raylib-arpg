//
// Created by steve on 02/10/2024.
//

#include "GameUiEngine.hpp"

namespace sage
{

    [[nodiscard]] Window* GameUIEngine::CreateWindow(Vector2 pos, float w, float h)
    {
        auto window = std::make_unique<Window>();
        window->nPatchInfo = {Rectangle{0.0f, 0.0f, 64.0f, 64.0f}, 12, 40, 12, 12, NPATCH_NINE_PATCH};
        window->tex = nPatchTexture;
        window->rec = {pos.x, pos.y, w, h};
        auto ptr = window.get();
        windows.push_back(std::move(window));
        return ptr;
    }

    [[nodiscard]] Table* Window::CreateTable()
    {
        auto table = std::make_unique<Table>();
        table->parent = this;
        // Table inherits window's dimensions and position
        table->rec = rec;
        // TODO: Add padding here
        // table.tex = LoadTexture("resources/textures/panel_background.png");
        auto ptr = table.get();
        children.push_back(std::move(table));
        return ptr;
    }

    [[nodiscard]] TableRow* Table::CreateTableRow()
    {
        auto row = std::make_unique<TableRow>();
        auto ptr = row.get();
        children.push_back(std::move(row));
        UpdateChildren();
        // TODO: Add padding here
        return ptr;
    }

    [[nodiscard]] TableCell* TableRow::CreateTableCell(Padding _padding, Margin _margin)
    {
        auto cell = std::make_unique<TableCell>();
        cell->padding = _padding;
        cell->margin = _margin;
        cell->parent = this;
        auto ptr = cell.get();
        children.push_back(std::move(cell));
        UpdateChildren();
        return ptr;
    }

    [[nodiscard]] TableCell* TableRow::CreateTableCell()
    {
        return CreateTableCell({}, {});
    }

    TextBox* TableCell::CreateTextbox(const std::string& _content)
    {
        child = std::make_unique<TextBox>();
        auto* textbox = dynamic_cast<TextBox*>(child.get());
        textbox->fontSize = 10;
        textbox->content = _content;
        UpdateChild();
        return textbox;
    }

    Button* TableCell::CreateButton(Texture _tex)
    {
        auto button = std::make_unique<Button>();
        button->tex = _tex;

        // Position the button inside the cell, accounting for padding
        button->rec = {
            rec.x + padding.left,
            rec.y + padding.up,
            rec.width - (+padding.right),
            rec.height - (padding.up + padding.down)};
        child = std::move(button);
        return dynamic_cast<Button*>(child.get());
    }

    void Table::UpdateChildren()
    {
        float rowHeight = rec.height / children.size();
        for (int i = 0; i < children.size(); ++i)
        {
            auto& row = children.at(i);
            row->parent = this;
            row->rec = rec;
            row->rec.height = rowHeight;
            row->rec.y = rec.y + (rowHeight * i);
            if (row->children.size() > 0) row->UpdateChildren();
            // TODO: Add margin here
        }
    }

    void TableRow::UpdateChildren()
    {
        float cellWidth = rec.width / children.size();
        for (int i = 0; i < children.size(); ++i)
        {
            auto& cell = children.at(i);
            cell->parent = this;
            cell->rec = rec;
            cell->rec.width = cellWidth;
            cell->rec.x = rec.x + (cellWidth * i);
            cell->UpdateChild();
        }
    }

    void TableCell::UpdateChild()
    {
        if (child)
        {
            child->parent = this;
            child->UpdateRec();
        }
    }

    void TextBox::UpdateRec()
    {
        // Calculate text dimensions
        Vector2 textSize = MeasureTextEx(GetFontDefault(), content.c_str(), fontSize, 1);
        rec = {parent->rec.x + parent->padding.left, parent->rec.y + parent->padding.up, textSize.x, textSize.y};
    }

    void Button::UpdateRec()
    {
    }

    void Window::Draw2D() const
    {
        //        DrawTextureNPatch(tex, nPatchInfo, rec, {0.0f, 0.0f}, 0.0f,
        //                          WHITE); // Use {0.0f, 0.0f} for origin

        for (auto& child : children)
        {
            child->Draw2D();
        }
    }

    void Table::Draw2D()
    {
        std::vector colors = {PINK, RED, BLUE, YELLOW, WHITE};
        for (int i = 0; i < children.size(); ++i)
        {
            auto& row = children[i];
            DrawRectangle(row->rec.x, row->rec.y, row->rec.width, row->rec.height, colors[i]);
            row->Draw2D();
        }
    }

    void TableRow::Draw2D()
    {
        std::vector colors = {RED, BLUE, YELLOW, WHITE, PINK};
        for (int i = 0; i < children.size(); ++i)
        {
            auto& cell = children[i];
            Color col = colors[i];
            col.a = 150;
            DrawRectangle(cell->rec.x, cell->rec.y, cell->rec.width, cell->rec.height, col);
            cell->Draw2D();
        }
    }

    void TableCell::Draw2D()
    {
        if (child)
        {
            child->Draw2D();
        }
    }

    void Button::Draw2D()
    {
    }

    void TextBox::Draw2D()
    {
        DrawText(content.c_str(), rec.x, rec.y, fontSize, BLACK);
    }

    void GameUIEngine::Draw2D()
    {
        for (auto& window : windows)
        {
            window->Draw2D();
        }
    }

    void GameUIEngine::Update()
    {
        // Handle input and update UI state here (e.g., button clicks, hover effects)
    }

    GameUIEngine::GameUIEngine(Settings* _settings, UserInput* _userInput, Cursor* _cursor)
        : nextId(1) // Initialize nextId to 1
    {
        nPatchTexture = LoadTexture("resources/textures/ninepatch_button.png");
    }
} // namespace sage