#include "menu.h"
#include<sstream>
#include<iostream>

int Menu::Selected() {
    return selected_option;
}
void Menu::Select(int indx) {
    selected_option = indx;
}

int Menu::Size() {return options.size();}


std::string Menu::Option(int indx) {
    return options[indx];
}

StartMenu::StartMenu() {
    options.push_back("Start");
    options.push_back("Select Difficulty");
    options.push_back("High Score");
    options.push_back("Exit");
}
MENU_ACTION StartMenu::Action ( ) {
    switch (selected_option) {
        // start game
        case 0:
            return MENU_ACTION::START;
        // open speed selection menu
        case 1:
            return MENU_ACTION::SELECT_SPEED;
        // show top 5 game scores
        case 2:
            return MENU_ACTION::HIGH_SCORE;
        // exit game
        case 3:
            return MENU_ACTION::EXIT;
    }
    return MENU_ACTION::ESCAPE;
}

PauseMenu::PauseMenu() {
    options.push_back("Restart");
    options.push_back("Resume");
    options.push_back("High Score");
    options.push_back("Exit");
}

MENU_ACTION PauseMenu::Action( ) {
    switch (selected_option) {
        //restart game
        case 0:
            return MENU_ACTION::RESTART;
        // resume game
        case 1: 
            return MENU_ACTION::RESUME;
        // show game top scores
        case 2:
            return MENU_ACTION::HIGH_SCORE;
        //quit game
        case 3: 
            return MENU_ACTION::EXIT;

    }
    return MENU_ACTION::ESCAPE;
}

SpeedMenu::SpeedMenu() {
    options.push_back("Easy");
    options.push_back("Normal");
    options.push_back("Hard");
    options.push_back("Expert");
}
MENU_ACTION SpeedMenu::Action( ) {
    switch (selected_option) {
        case 0:
            return MENU_ACTION::EASY;
        case 1:
            return MENU_ACTION::NORMAL;
        case 2:
            return MENU_ACTION::HARD;
        case 3:
            return MENU_ACTION::EXPERT;
    }
    return MENU_ACTION::ESCAPE;
}


EndMenu::EndMenu( ) {
    options.push_back("Restart");
    options.push_back("High Score");
    options.push_back("Exit");
}
MENU_ACTION EndMenu::Action( ) {
    switch (selected_option) {
        // restart game
        case 0: 
            return MENU_ACTION::RESTART;
        // show game top scores
        case 1:
            return MENU_ACTION::HIGH_SCORE;
        // quit game
        case 2:
            return MENU_ACTION::EXIT;
    }
    return MENU_ACTION::ESCAPE;
}

ScoreMenu::ScoreMenu(std::vector<ScoreStat> top_score_list) {
    font_size = 20;
    if(top_score_list.empty())
        options.push_back("Be The First Top Scorer");
    else {
        
        for(auto a:top_score_list) {
            std::stringstream score_str;
            score_str.width(22);
            score_str<<std::left<<a.name;
            score_str.width(5);
            score_str<<std::left<<std::to_string(a.score);
            score_str.width(16);
            score_str<<std::left<<a.timestamp;
            options.push_back(score_str.str());
        }
    }
}
MENU_ACTION ScoreMenu::Action( ) {
    return MENU_ACTION::ESCAPE;

}
