#ifndef MENU_H
#define MENU_H

#include<vector>
#include<string>
#include<iostream>

class Game;
class Snake;
class Menu {
    public:
    virtual ~Menu(){}
    void SelectItem(int indx);
    // void AddItem(std::string item);
    int GetSelectedIndx();
    // std::string GetSelectedItem();
    std::string GetItem(int indx);
    int Size();
    virtual void Action(Snake &snake, Game &game) {}

    protected:
    std::vector<std::string> items;
    int selected_item_indx = 0;
    
};

class StartMenu : public Menu {
    public:
    StartMenu();
    void Action(Snake &snake, Game &game) override;

};

class PauseMenu : public Menu {
    public:
    PauseMenu();
    void Action(Snake &snake, Game &game) override;

};

class SpeedMenu : public Menu {
    public:
    SpeedMenu();
    void Action(Snake &snake, Game &game) override;

};

class EndMenu : public Menu {
    public:
    EndMenu();
    void Action(Snake &snake, Game &game) override;

};

#endif