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
#include<mutex>
#include"food.h"
#include<memory>
#include<string>
// class Food;
class Game;


class Renderer {
 public:
  Renderer(const std::size_t screen_width, const std::size_t screen_height,
           const std::size_t grid_width, const std::size_t grid_height);
  ~Renderer();

  void Start(std::shared_ptr<Game> game);
  

 private:
  
  SDL_Window *sdl_window;
  SDL_Renderer *sdl_renderer;
  SDL_Texture* texture;
  SDL_Color textColor;
  TTF_Font* menu_fonts;
  TTF_Font* state_fonts;
  int textSurfaceWidth;
  int textSurfaceHeight;

  const std::size_t screen_width;
  const std::size_t screen_height;
  const std::size_t grid_height;
  const std::size_t grid_width;


  void Render(Snake const &snake, Food* normal_food, Food* super_food, /*std::vector<std::shared_ptr<Food>*> const &food*/ std::shared_ptr<Game> game);
  void Render(Menu *menu,std::shared_ptr<Game>  game);
  void Render(std::string player_name);
  

  // void UpdateWindowTitle(int score, int fps);
  void RenderScore(std::shared_ptr<Game> game);
  void RenderTimers(std::shared_ptr<Game> game);
  void RenderFPS(std::shared_ptr<Game> game);
  

  void SetBlock(int x, int y, int h, int w) ;
  void LoadText(std::string textureText, TTF_Font *fonts, SDL_Color textColor);
  void RenderText( int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE );
};

#endif