#include<iostream>
#include "menu.h"
#include "game.h"
// #include<thread>

int Menu::GetSelectedIndx() {
    return selected_item_indx;
}
void Menu::SelectItem(int indx) {
    selected_item_indx = indx;
}

int Menu::Size() {return items.size();}


std::string Menu::GetItem(int indx) {
    return items[indx];
}

StartMenu::StartMenu() {
    items.push_back("Start");
    items.push_back("Select Difficulty");
    items.push_back("Exit");
}
void StartMenu::Action (Snake &snake, Game &game) {
    std::cout<<"inside startmenu action\n";
    switch (selected_item_indx) {
        // start game
        case 0:
            game.Reset();
            // create a new thread for update
            game.onMenu = false;
            break;
        // open speed selection menu
        case 1:
            game.stMenu.push(new SpeedMenu());
            break;
        // exit game
        case 2:
            game.Quit();
            break;
    }
}

PauseMenu::PauseMenu() {
    items.push_back("Restart");
    items.push_back("Resume");
    items.push_back("Exit");
}

void PauseMenu::Action(Snake &snake, Game &game) {
    switch (selected_item_indx) {
        //restart game
        case 0:
            game.Reset();
            game.stMenu.pop();
            game.stMenu.push(new StartMenu());
            break;
        // resume game
        case 1:
            game.stMenu.pop();
            game.onMenu = false;
            break;
        //quit game
        case 2:
            game.Quit();
            break;

    }
}

SpeedMenu::SpeedMenu() {
    items.push_back("Easy");
    items.push_back("Normal");
    items.push_back("Hard");
    items.push_back("Expert");
}
void SpeedMenu::Action(Snake &snake, Game &game) {
    game.Reset();
    switch (selected_item_indx) {
        case 0:
            snake.speed = 0.05f;
            break;
        case 1:
            snake.speed = 0.1f;
            break;
        case 2:
            snake.speed = 0.15f;
            break;
        case 3:
            snake.speed = 0.2f;
            break;
    }
    game.stMenu.pop();
    game.onMenu = false;
    // std::cout<<game.onMenu<<"\n";
}


EndMenu::EndMenu() {
    items.push_back("Restart");
    items.push_back("Exit");
}
void EndMenu::Action(Snake &snake, Game &game) {
    // game.Reset();
    switch (selected_item_indx) {
        // restart game
        case 0:
            game.stMenu.pop();
            game.stMenu.push(new StartMenu());
            break;
        // quit game
        case 1:
            game.Quit();
    }
}