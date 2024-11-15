#ifndef MENU_H
#define MENU_H

#include<vector>
#include<string>
#include<iostream>
#include"scoreManager.h"

// class Game;
enum class MENU_ACTION {START, SELECT_SPEED, HIGH_SCORE, EXIT, ESCAPE, RESTART, RESUME, EASY, NORMAL, HARD, EXPERT};
class Menu {
    public:
    virtual ~Menu(){}
    void Select(int indx);
    int Selected();
    std::string Option(int indx);
    int Size();
    int font_size = 28;
    virtual MENU_ACTION Action() {return MENU_ACTION::ESCAPE;}

    protected:
    std::vector<std::string> options;
    int selected_option = 0;
    
};

class StartMenu : public Menu {
    public:
    StartMenu();
    MENU_ACTION Action() override;

};

class PauseMenu : public Menu {
    public:
    PauseMenu();
    MENU_ACTION Action() override;

};

class SpeedMenu : public Menu {
    public:
    SpeedMenu();
    MENU_ACTION Action() override;
};

class EndMenu : public Menu {
    public:
    EndMenu();
    MENU_ACTION Action() override;
};

class ScoreMenu: public Menu {
    public:
    ScoreMenu(std::vector<ScoreStat> top_score_list);
    MENU_ACTION Action() override;
};

#endif