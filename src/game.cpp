#include "game.h"
#include <iostream>
#include "SDL.h"
#include "menu.h"
// #include<thread>
// #include<chrono>
Game::Game(std::size_t grid_width, std::size_t grid_height)
    : snake(grid_width, grid_height),
      grid_h(grid_height),
      grid_w(grid_width),
      engine(dev()),
      random_w(0, static_cast<int>(grid_width - 1)),
      random_h(0, static_cast<int>(grid_height - 1)),
      running(false),
      onMenu(true) {
        std::cout<<"random_w: "<<random_w<<" random_h: "<<random_h<<"\n";
        // start with main menu
        stMenu.push(new StartMenu());
}

void Game::Run(Controller  const &controller, Renderer &renderer,
               std::size_t target_frame_duration) {
  Uint32 title_timestamp = SDL_GetTicks();
  Uint32 frame_start;
  Uint32 frame_end;
  Uint32 frame_duration;
  int frame_count = 0;
  running = true;



  while (running) {
    frame_start = SDL_GetTicks();

    // Input, Update, Render - the main game loop.
    if(onMenu) {
      controller.HandleMenuInput(running, *(stMenu.top()), snake, *this);
      renderer.Render(*(stMenu.top()));
      
    }
      
    else {
      controller.HandleInput(running, snake, *this);
      Update();
      renderer.Render(snake, food);
    }

    

    frame_end = SDL_GetTicks();

    // Keep track of how long each loop through the input/update/render cycle
    // takes.
    frame_count++;
    frame_duration = frame_end - frame_start;

    // After every second, update the window title.
    if (frame_end - title_timestamp >= 1000) {
      renderer.UpdateWindowTitle(score, frame_count);
      frame_count = 0;
      title_timestamp = frame_end;
    }

    // If the time for this frame is too small (i.e. frame_duration is
    // smaller than the target ms_per_frame), delay the loop to
    // achieve the correct frame rate.
    if (frame_duration < target_frame_duration) {
      SDL_Delay(target_frame_duration - frame_duration);
    }
  }
}

void Game::PlaceFood() {
  int x, y;
  while (true) {
    x = random_w(engine);
    y = random_h(engine);
    // Check that the location is not occupied by a snake item before placing
    // food.
    if (!snake.SnakeCell(x, y)) {
      food.x = x;
      food.y = y;
      return;
    }
  }
}

void Game::Update() {
  if (!snake.alive) {
    stMenu.push(new EndMenu());
    onMenu = true;
  }

  snake.Update();

  int new_x = static_cast<int>(snake.head_x);
  int new_y = static_cast<int>(snake.head_y);

  // Check if there's food over here
  if (food.x == new_x && food.y == new_y) {
    score++;
    PlaceFood();
    // Grow snake and increase speed.
    snake.GrowBody();
    snake.speed += 0.02;
  }
}

void Game::Quit() {
  running = false;
}
int Game::GetScore() const { return score; }
int Game::GetSize() const { return snake.size; }

void Game::Reset() {
  running = true;
  snake.alive = true;
  snake.body.clear();
  score = 0;
  snake.size = 1;
  snake.head_x = grid_w/2;
  snake.head_y = grid_h/2;
  snake.speed = 0.1f;
}