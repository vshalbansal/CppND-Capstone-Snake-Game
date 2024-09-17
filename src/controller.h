#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "snake.h"
#include "menu.h"

class Game;
class Controller {
 public:
  void HandleInput(bool &running, Snake &snake, Game &game) const;

  void HandleMenuInput(bool &running, Menu &menu, Snake &snake, Game &game) const;

 private:
  void ChangeDirection(Snake &snake, Snake::Direction input,
                       Snake::Direction opposite) const;
};

#endif