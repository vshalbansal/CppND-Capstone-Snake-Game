#include "controller.h"
#include <iostream>
#include "SDL.h"
#include "snake.h"
#include "menu.h"
#include "game.h"

void Controller::ChangeDirection(Snake &snake, Snake::Direction input, Snake::Direction opposite) const {
  // snake._mutex.lock();
  if (snake.direction != opposite || snake.size == 1) snake.direction = input;
  // snake._mutex.unlock();
  return;
}

void Controller::HandleMenuInput(bool &running, Menu &menu, Snake &snake, Game &game) const{
  // std::cout<<"inside handle menu input method\n";
  SDL_Event e;
  while(SDL_PollEvent(&e))
  {
    // std::cout<<"polling std_pollEvent\n";
    if (e.type ==SDL_QUIT) {
      // std::cout<<"e_type sdl quit\n";
      running = false;
    }
    else if(e.type == SDL_KEYDOWN) {
      // std::cout<<"handling input menu\n";
      switch (e.key.keysym.sym) {
        case SDLK_UP:
          menu.SelectItem((menu.GetSelectedIndx()-1) % menu.Size());
          break;
        case SDLK_DOWN:
          menu.SelectItem((menu.GetSelectedIndx()+1) % menu.Size());
          break;
        case SDLK_RETURN:
          std::cout<<"Enter pressed\n";
          menu.Action(snake, game);
          break;
        case SDLK_ESCAPE:
          game.stMenu.pop();
          if(game.stMenu.empty())
            game.onMenu = false;
          break;
      }
    }
    
  }

}
void Controller::HandleInput(bool &running, Snake &snake, Game &game) const {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT) {
      running = false;
    } else if (e.type == SDL_KEYDOWN) {
      switch (e.key.keysym.sym) {
        case SDLK_UP:
          ChangeDirection(snake, Snake::Direction::kUp,
                          Snake::Direction::kDown);
          break;

        case SDLK_DOWN:
          ChangeDirection(snake, Snake::Direction::kDown,
                          Snake::Direction::kUp);
          break;

        case SDLK_LEFT:
          ChangeDirection(snake, Snake::Direction::kLeft,
                          Snake::Direction::kRight);
          break;

        case SDLK_RIGHT:
          ChangeDirection(snake, Snake::Direction::kRight,
                          Snake::Direction::kLeft);
          break;
        case SDLK_ESCAPE:
          game.onMenu = true;
          game.stMenu.push(new PauseMenu());
          // open pause menu
          break;
      }
    }
  }
}