#include "renderer.h"
#include <iostream>
#include <string>
#include <SDL2/SDL.h> 
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>    

Renderer::Renderer(const std::size_t screen_width,
                   const std::size_t screen_height,
                   const std::size_t grid_width, const std::size_t grid_height)
    : screen_width(screen_width),
      screen_height(screen_height),
      grid_width(grid_width),
      grid_height(grid_height) {
  
  // Initialize True Type Fonts
  if( TTF_Init() == -1 )
    std::cout<<"SDL_ttf could not initialize! SDL_ttf Error: "<<TTF_GetError()<<"\n";
  // gFont = TTF_OpenFont( "./lazy.ttf", 28 );
  gFont = TTF_OpenFont( "../data/Kingthings_Foundation.ttf", 28 );
  // TTF_SetFontStyle(gFont, 2);

  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL could not initialize.\n";
    std::cerr << "SDL_Error: " << SDL_GetError() << "\n";
  }

  // Create Window
  sdl_window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED, screen_width,
                                screen_height, SDL_WINDOW_SHOWN);

  if (nullptr == sdl_window) {
    std::cerr << "Window could not be created.\n";
    std::cerr << " SDL_Error: " << SDL_GetError() << "\n";
  }

  // Create renderer
  sdl_renderer = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_ACCELERATED);
  if (nullptr == sdl_renderer) {
    std::cerr << "Renderer could not be created.\n";
    std::cerr << "SDL_Error: " << SDL_GetError() << "\n";
  }
}

Renderer::~Renderer() {
  SDL_DestroyWindow(sdl_window);
  SDL_Quit();
}



void Renderer::Render(Snake const snake, SDL_Point const &food) {
  SDL_Rect block;
  block.w = screen_width / grid_width;
  block.h = screen_height / grid_height;

  // Clear screen
  SDL_SetRenderDrawColor(sdl_renderer, 0x1E, 0x1E, 0x1E, 0xFF);
  SDL_RenderClear(sdl_renderer);

  // Render food
  SDL_SetRenderDrawColor(sdl_renderer, 0xFF, 0xCC, 0x00, 0xFF);
  block.x = food.x * block.w;
  block.y = food.y * block.h;
  SDL_RenderFillRect(sdl_renderer, &block);

  // Render snake's body
  SDL_SetRenderDrawColor(sdl_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
  for (SDL_Point const &point : snake.body) {
    block.x = point.x * block.w;
    block.y = point.y * block.h;
    SDL_RenderFillRect(sdl_renderer, &block);
  }

  // Render snake's head
  block.x = static_cast<int>(snake.head_x) * block.w;
  block.y = static_cast<int>(snake.head_y) * block.h;
  if (snake.alive) {
    SDL_SetRenderDrawColor(sdl_renderer, 0x00, 0x7A, 0xCC, 0xFF);
  } else {
    SDL_SetRenderDrawColor(sdl_renderer, 0xFF, 0x00, 0x00, 0xFF);
  }
  SDL_RenderFillRect(sdl_renderer, &block);

  // Update Screen
  SDL_RenderPresent(sdl_renderer);
}

void Renderer::UpdateWindowTitle(int score, int fps) {
  std::string title{"Snake Score: " + std::to_string(score) + " FPS: " + std::to_string(fps)};
  SDL_SetWindowTitle(sdl_window, title.c_str());
}


// Render Menu
void Renderer::Render(Menu menu) {

  SDL_SetRenderDrawColor(sdl_renderer, 0x1E, 0x1E, 0x1E, 0xFF);
  SDL_RenderClear(sdl_renderer);

  if( gFont == NULL )
		std::cout<<"Failed to load lazy font! SDL_ttf Error: "<< TTF_GetError()<<"\n";
  SDL_Color textColor = 	{255,255,255};


  for(int i=0;i<menu.Size();i++) {
    if(menu.GetSelectedIndx()==i)
      textColor = {255,0,0};
    else
      textColor = 	{255,255,255};

    SDL_Surface* textSurface = TTF_RenderText_Solid( gFont, menu.GetItem(i).c_str(), textColor );
    if( textSurface == NULL )
    {
      printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
    }
    SDL_Texture* mTexture = SDL_CreateTextureFromSurface( sdl_renderer, textSurface );
    SDL_SetRenderDrawColor( sdl_renderer, 0xFF, 0xFF, 0xFF, 0xFF );

    //Render current frame
    SDL_Rect block;
    block.x = (screen_width- textSurface->w)/2;
    block.y = (screen_height- textSurface->h)/2 + i*textSurface->h;
    block.h = textSurface->h;
    block.w = textSurface->w;;
    
    //Render to screen
    SDL_RenderCopyEx( sdl_renderer, mTexture, NULL, &block, 0.0, NULL, SDL_FLIP_NONE ); 
  }

  //Update screen
  SDL_RenderPresent( sdl_renderer );

}