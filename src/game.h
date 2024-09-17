#ifndef GAME_H
#define GAME_H

#include <random>
#include <stack>
#include "SDL.h"
#include "controller.h"
#include "renderer.h"
#include "snake.h"
#include "menu.h"

class Game {
 public:
  Game(std::size_t grid_width, std::size_t grid_height);
  void Run(Controller const &controller, Renderer &renderer,
           std::size_t target_frame_duration);
  int GetScore() const;
  int GetSize() const;
  void Quit();
  bool onMenu;
  std::stack<Menu*> stMenu;
  bool running;
  void Reset();
  // bool OnStartMenu() {return on_start_menu;}
  // bool IsPaused() {return is_paused;}
  // bool IsQuit() {return is_quit;}
  

 private:
  Snake snake;
  SDL_Point food;
  

  std::random_device dev;
  std::mt19937 engine;
  std::uniform_int_distribution<int> random_w;
  std::uniform_int_distribution<int> random_h;

  int score{0};
  std::size_t grid_w;
  std::size_t grid_h;
  // bool onMenu;
  
  void PlaceFood();
  void Update();
};

#endif