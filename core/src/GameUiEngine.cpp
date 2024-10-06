//
// Created by steve on 02/10/2024.
//

#include "GameUiEngine.hpp"

namespace sage
{

    [[nodiscard]] Window* GameUIEngine::CreateWindow(Image _nPatchTexture, Vector2 pos, float w, float h)
    {
        windows.push_back(std::make_unique<Window>());
        auto& window = windows.back();
        window->nPatchTexture = LoadTextureFromImage(_nPatchTexture);
        window->rec = {pos.x, pos.y, w, h};
        return window.get();
    }

    [[nodiscard]] Table* Window::CreateTable()
    {
        children.push_back(std::make_unique<Table>());
        const auto& table = children.back();
        table->parent = this;
        // Table inherits window's dimensions and position
        table->rec = rec;
        // TODO: Add padding here
        // table.tex = LoadTexture("resources/textures/panel_background.png");

        return table.get();
    }

    [[nodiscard]] TableRow* Table::CreateTableRow()
    {
        children.push_back(std::make_unique<TableRow>());
        const auto& row = children.back();
        UpdateChildren();
        return row.get();
    }

    [[nodiscard]] TableCell* TableRow::CreateTableCell()
    {
        children.push_back(std::make_unique<TableCell>());
        const auto& cell = children.back();
        cell->parent = this;
        UpdateChildren();
        return cell.get();
    }

    TextBox* TableCell::CreateTextbox(const std::string& _content)
    {
        children = std::make_unique<TextBox>();
        auto* textbox = dynamic_cast<TextBox*>(children.get());
        textbox->fontSize = 42;
        textbox->content = _content;
        UpdateChildren();
        return textbox;
    }

    ImageBox* TableCell::CreateImagebox(Image _tex)
    {
        children = std::make_unique<ImageBox>();
        auto* image = dynamic_cast<ImageBox*>(children.get());
        image->tex = LoadTextureFromImage(_tex);
        UpdateChildren();
        return image;
    }

    void Table::UpdateChildren()
    {
        const float rowHeight = std::ceil(rec.height / children.size());
        for (int i = 0; i < children.size(); ++i)
        {
            const auto& row = children.at(i);
            row->parent = this;
            row->rec = rec;
            row->rec.height = rowHeight;
            row->rec.y = rec.y + (rowHeight * i);
            if (!row->children.empty()) row->UpdateChildren();
            // TODO: Add margin here
            // TODO: Add padding here
        }
    }

    void TableRow::UpdateChildren()
    {
        const float cellWidth = std::ceil(rec.width / children.size());
        for (int i = 0; i < children.size(); ++i)
        {
            const auto& cell = children.at(i);
            cell->parent = this;
            cell->rec = rec;
            cell->rec.width = cellWidth;
            cell->rec.x = rec.x + (cellWidth * i);
            cell->UpdateChildren();
            // TODO: Add margin here
            // TODO: Add padding here
        }
    }

    void TableCell::UpdateChildren()
    {
        if (children)
        {
            children->parent = this;
            children->rec = rec;
            children->UpdateDimensions();
        }
    }

    void TextBox::UpdateDimensions()
    {
        constexpr int MIN_FONT_SIZE = 4;

        float availableWidth = parent->rec.width - (parent->GetPadding().left + parent->GetPadding().right);
        Vector2 textSize = MeasureTextEx(GetFontDefault(), content.c_str(), fontSize, fontSpacing);
        while (textSize.x > availableWidth && fontSize > MIN_FONT_SIZE)
        {
            fontSize -= 1;
            textSize = MeasureTextEx(GetFontDefault(), content.c_str(), fontSize, fontSpacing);
        }

        float horiOffset = 0; // Left
        float vertOffset = 0; // Top
        float availableHeight = parent->rec.height - (parent->GetPadding().up + parent->GetPadding().down);

        if (vertAlignment == VertAlignment::MIDDLE)
        {
            vertOffset = (availableHeight - textSize.y) / 2;
        }
        else if (vertAlignment == VertAlignment::BOTTOM)
        {
            vertOffset = availableHeight - textSize.y;
        }

        if (horiAlignment == HoriAlignment::RIGHT)
        {
            horiOffset = availableWidth - textSize.x;
        }
        else if (horiAlignment == HoriAlignment::CENTER)
        {
            horiOffset = (availableWidth - textSize.x) / 2;
        }

        rec = {
            parent->rec.x + parent->GetPadding().left + horiOffset,
            parent->rec.y + parent->GetPadding().up + vertOffset,
            textSize.x,
            textSize.y};
    }

    void ImageBox::UpdateDimensions()
    {
        float availableWidth = parent->rec.width - (parent->GetPadding().left + parent->GetPadding().right);
        float availableHeight = parent->rec.height - (parent->GetPadding().up + parent->GetPadding().down);

        float originalRatio = static_cast<float>(tex.width) / tex.height;
        float finalWidth, finalHeight;

        if (originalRatio > 1.0f) // Wider than tall
        {
            finalWidth = availableWidth;
            finalHeight = availableWidth / originalRatio;
        }
        else // Taller than wide
        {
            finalHeight = availableHeight;
            finalWidth = availableHeight * originalRatio;
        }

        float horiOffset = 0; // Left
        float vertOffset = 0; // Top

        if (vertAlignment == VertAlignment::MIDDLE)
        {
            vertOffset = (availableHeight - finalHeight) / 2;
        }
        else if (vertAlignment == VertAlignment::BOTTOM)
        {
            vertOffset = availableHeight - finalHeight;
        }

        if (horiAlignment == HoriAlignment::RIGHT)
        {
            horiOffset = availableWidth - finalWidth;
        }
        else if (horiAlignment == HoriAlignment::CENTER)
        {
            horiOffset = (availableWidth - finalWidth) / 2;
        }

        rec = Rectangle{
            parent->rec.x + parent->GetPadding().left + horiOffset,
            parent->rec.y + parent->GetPadding().up + vertOffset,
            finalWidth,
            finalHeight};

        tex.width = finalWidth;
        tex.height = finalHeight;
    }

    void Window::Draw2D() const
    {
        if (tex.id > 0)
        {
            DrawTextureNPatch(tex, nPatchInfo, rec, {0.0f, 0.0f}, 0.0f,
                              WHITE); // Use {0.0f, 0.0f} for origin
        }

        for (auto& child : children)
        {
            child->Draw2D();
        }
    }

    void Table::Draw2D()
    {
        if (tex.id > 0)
        {
            DrawTextureNPatch(tex, nPatchInfo, rec, {0.0f, 0.0f}, 0.0f,
                              WHITE); // Use {0.0f, 0.0f} for origin
        }
        std::vector colors = {PINK, RED, BLUE, YELLOW, WHITE};
        for (int i = 0; i < children.size(); ++i)
        {
            const auto& row = children[i];
            // DrawRectangle(row->rec.x, row->rec.y, row->rec.width, row->rec.height, colors[i]);
            row->Draw2D();
        }
    }

    void TableRow::Draw2D()
    {
        if (tex.id > 0)
        {
            DrawTextureNPatch(tex, nPatchInfo, rec, {0.0f, 0.0f}, 0.0f,
                              WHITE); // Use {0.0f, 0.0f} for origin
        }
        std::vector colors = {RED, BLUE, YELLOW, WHITE, PINK};
        for (int i = 0; i < children.size(); ++i)
        {
            const auto& cell = children[i];
            Color col = colors[i];
            col.a = 150;
            // DrawRectangle(cell->rec.x, cell->rec.y, cell->rec.width, cell->rec.height, col);
            cell->Draw2D();
        }
    }

    void TableCell::Draw2D()
    {
        if (tex.id > 0)
        {
            DrawTextureNPatch(tex, nPatchInfo, rec, {0.0f, 0.0f}, 0.0f,
                              WHITE); // Use {0.0f, 0.0f} for origin
        }
        if (children) // single element
        {
            children->Draw2D();
        }
    }

    void ImageBox::Draw2D()
    {
        DrawTexture(tex, rec.x, rec.y, WHITE);
    }

    void TextBox::Draw2D()
    {
        DrawTextEx(GetFontDefault(), content.c_str(), Vector2{rec.x, rec.y}, fontSize, fontSpacing, BLACK);
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
    {
        //        nPatchTexture = LoadTexture("resources/textures/ninepatch_button.png");
        //        info1 = {Rectangle{0.0f, 0.0f, 64.0f, 64.0f}, 12, 40, 12, 12, NPATCH_NINE_PATCH};
        //        info2 = {Rectangle{0.0f, 128.0f, 64.0f, 64.0f}, 16, 16, 16, 16, NPATCH_NINE_PATCH};
        //        info3 = {Rectangle{0.0f, 64.0f, 64.0f, 64.0f}, 8, 8, 8, 8, NPATCH_NINE_PATCH};
    }
} // namespace sage