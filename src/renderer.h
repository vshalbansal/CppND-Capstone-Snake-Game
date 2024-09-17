#ifndef RENDERER_H
#define RENDERER_H

#include <vector>
// #include "SDL.h"
#include <SDL2/SDL.h> 
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>  
#include "snake.h"
#include "menu.h"



class Renderer {
 public:
  Renderer(const std::size_t screen_width, const std::size_t screen_height,
           const std::size_t grid_width, const std::size_t grid_height);
  ~Renderer();

  void Render(Snake const snake, SDL_Point const &food);
  void Render(Menu menu);
  void UpdateWindowTitle(int score, int fps);

 private:
 TTF_Font* gFont;
  SDL_Window *sdl_window;
  const std::size_t grid_width;
  SDL_Renderer *sdl_renderer;

  const std::size_t screen_width;
  const std::size_t screen_height;
  const std::size_t grid_height;
};

#endif