//
// Created by Steve Wheeler on 03/10/2024.
//

#include "GameUiFactory.hpp"
#include "GameUiEngine.hpp"

namespace sage
{

    void GameUiFactory::CreateExampleWindow(GameUIEngine* engine, Vector2 pos)
    {

        // TODO: Horizontal and vertical alignment settings would be very nice
        // TODO: Have a "CreateTitleBar" with a textbox and a image of a cross for closing
        // When the title bar is clicked, you can have it get the parent's window pos and update it based on mouse
        // location (and call update children)
        // TODO: Be able to specify a cell's width
        // TODO: Specify padding to percent, add a function to convert it to pixels

        auto window = engine->CreateWindow(pos, 500, 200);
        auto table = window->CreateTable();

        auto row = table->CreateTableRow();
        // auto row0 = table->CreateTableRow();
        auto cell = row->CreateTableCell();
        cell->CreateTextbox("Number 11111111111!");
        auto cell2 = row->CreateTableCell();
        auto cell3 = row->CreateTableCell();
        cell2->CreateImagebox(LoadImage("resources/icon.png"));
        cell3->CreateImagebox(LoadImage("resources/icon.png"));

        // auto cell0 = row->CreateTableCell();

        // cell->padding.left = 10;
        // cell2->CreateTextbox("Number 2!");

        // cell3->CreateTextbox("Number 3!");
        cell3->SetPaddingPercent({0, 0, 20, 0});

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