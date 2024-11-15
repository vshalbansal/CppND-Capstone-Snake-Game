
#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "snake.h"
#include "menu.h"
#include<memory>

class Game;
class Controller {
 public:
    void Start(std::shared_ptr<Game> game);
  void HandleGameInput(Snake &snake, std::shared_ptr<Game> game, SDL_Event &e) const;

  void HandleMenuInput(Menu *menu, std::shared_ptr<Game> game, SDL_Event &e) const;

  void HandleTextInput(std::shared_ptr<Game> game, SDL_Event &e, std::string &player_name);

 private:
  void ChangeDirection(Snake &snake, Snake::Direction input,
                       Snake::Direction opposite) const;
};

#endif