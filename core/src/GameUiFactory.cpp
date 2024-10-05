//
// Created by Steve Wheeler on 03/10/2024.
//

#include "GameUiFactory.hpp"
#include "GameUiEngine.hpp"

namespace sage
{

    void GameUiFactory::CreateLootWindow(GameUIEngine* engine, Vector2 pos)
    {
        auto window = engine->CreateWindow(pos, 200, 200);
        auto table = window->CreateTable();

        auto row = table->CreateTableRow();
        auto row0 = table->CreateTableRow();
        auto cell = row->CreateTableCell();
        auto cell2 = row->CreateTableCell();
        auto cell3 = row->CreateTableCell();
        // auto cell0 = row->CreateTableCell();
        cell->CreateTextbox("Number 1!");
        cell->padding.left = 10;
        cell2->CreateTextbox("Number 2!");
        cell3->CreateTextbox("Number 3!");

        auto row2 = table->CreateTableRow();
        auto cell4 = row2->CreateTableCell();
        auto cell5 = row2->CreateTableCell();
        cell4->CreateTextbox("Number 4!");
        cell5->CreateTextbox("Number 5!");
    }

    void GameUiFactory::CreateInventoryWindow(GameUIEngine* engine, Vector2 pos, float w, float h)
    {
        engine->CreateWindow(pos, w, h);
    }
} // namespace sage