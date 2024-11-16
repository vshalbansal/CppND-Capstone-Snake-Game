
#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "snake.h"
#include "menu.h"
#include<memory>

class Game;
class Controller {
 public:
  void HandleInput(std::shared_ptr<Game> game);
  

 private:
  void HandleGameInput(Snake &snake, std::shared_ptr<Game> game, SDL_Event &e) const;
  void HandleMenuInput(Menu *menu, std::shared_ptr<Game> game, SDL_Event &e) const;
  void HandleNameInput(std::shared_ptr<Game> game, SDL_Event &e, std::string &player_name);
  void ChangeDirection(Snake &snake, Snake::Direction input,
                       Snake::Direction opposite) const;
};

#endif