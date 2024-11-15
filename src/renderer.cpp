#include "renderer.h"
#include <iostream>
#include<thread>
#include<chrono>


Renderer::Renderer(const std::size_t screen_width,
                   const std::size_t screen_height,
                   const std::size_t grid_width, const std::size_t grid_height) 
    : screen_width(screen_width),
      screen_height(screen_height),
      grid_width(grid_width),
      grid_height(grid_height){

        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
          std::cerr << "SDL could not initialize.\n";
          std::cerr << "SDL_Error: " << SDL_GetError() << "\n";
        }
        else
          std::cout<<"SDL init success.\n";
              
        // Initialize True Type Fonts
        if( TTF_Init() == -1 )
          std::cout<<"SDL_ttf could not initialize! SDL_ttf Error: "<<TTF_GetError()<<"\n";
        else
          std::cout<<"TTF init success.\n";
        
        // Load Fonts for Menu Options
        menu_fonts = TTF_OpenFont( "../data/luximb.ttf", 28 );
        if( menu_fonts == NULL ) {
          std::cout<<"Failed to load menu font! SDL_ttf Error: "<< TTF_GetError()<<"\n";
          return;
        }

        // Load Fonts for Score and Timers
        state_fonts = TTF_OpenFont("../data/OpenSans-VariableFont_wdth,wght.ttf", 16);
        if(state_fonts == NULL) {
          std::cout<<"Failed to load game stat font! SDL_ttf Error: "<< TTF_GetError()<<"\n";
          return;
        }

        // Create Window
        sdl_window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED,
                                      SDL_WINDOWPOS_CENTERED, screen_width,
                                      screen_height, SDL_WINDOW_SHOWN);

        if (nullptr == sdl_window) {
          std::cerr << "Window could not be created.\n";
          std::cerr << " SDL_Error: " << SDL_GetError() << "\n";
        }
        else
          std::cout<<"SDL window creation success.\n";

        // Create renderer
        sdl_renderer = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_ACCELERATED);
        if (nullptr == sdl_renderer) {
          std::cerr << "Renderer could not be created.\n";
          std::cerr << "SDL_Error: " << SDL_GetError() << "\n";
        }
}

//Start Rendering
void Renderer::Start(std::shared_ptr<Game> game) {

  
  std::cout<<"Rendering started\n";
  while(game->state!=GAMESTATE::END) {
    switch(game->state) {
      case GAMESTATE::NAME_INPUT:
        Render(game->GetPlayerName());
        break;
      case GAMESTATE::ON_MENU: {
        std::lock_guard<std::mutex> lckGrd(game->mtx_menu);
        Menu* menu = game->GetCurrentMenu();
        if(menu)
          Render(menu,game); 
        break;
      }
      case GAMESTATE::RUNNING:
        Render(game->snake, game->GetNormalFood(), game->GetSuperFood(), game ); 
        break;
    }
    SDL_RenderPresent(sdl_renderer);
    
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  
  std::cout<<"Rendering stopped\n";
}
// Render Menu
void Renderer::Render(Menu *menu,std::shared_ptr<Game> game) {
  SDL_SetRenderDrawColor(sdl_renderer, 0x1E, 0x1E, 0x1E, 0xFF);
  SDL_RenderClear(sdl_renderer);
  textColor = 	{255,255,255};
  TTF_SetFontSize(menu_fonts, menu->font_size);

  if(!game->snake.alive) {
    LoadText("Game Over", menu_fonts, textColor);
    RenderText((screen_width-textSurfaceWidth)/2, 0);
    // Final Score
    std::string final_score_str = "Your Score : "+std::to_string(game->GetScore());
    LoadText(final_score_str, menu_fonts, textColor);
    RenderText((screen_width-textSurfaceWidth)/2, textSurfaceHeight);
  }
  for(int i=0;i<menu->Size();i++) {
    if(menu->Selected()==i)
      textColor = {255,0,0};
    else
      textColor = 	{255,255,255};
    LoadText(menu->Option(i), menu_fonts, textColor);
    SDL_SetRenderDrawColor( sdl_renderer, 0xFF, 0xFF, 0xFF, 0xFF );
    RenderText((screen_width-textSurfaceWidth )/2, (screen_height-textSurfaceHeight*menu->Size())/2 + i*textSurfaceHeight);
  }
}


// Render Player Name Input Screen
void Renderer::Render(std::string player_name) {
  SDL_SetRenderDrawColor(sdl_renderer, 0x1E, 0x1E, 0x1E, 0xFF);
  SDL_RenderClear(sdl_renderer);
  textColor = 	{255,255,255};
  std::string namePrompt = "Enter Your Name";
  LoadText(namePrompt, menu_fonts, textColor);
  RenderText((screen_width-textSurfaceWidth)/2, 0);

  textColor = 	{255,0,0};
  if(player_name!="")
    LoadText(player_name, menu_fonts, textColor);
  else
    LoadText("(Type Your Name Here)", menu_fonts, textColor);
  RenderText((screen_width-textSurfaceWidth)/2, (screen_height-textSurfaceHeight)/2);

  std::string nameInstruction = "Press Esc to Continue as Anonymous";
  textColor = 	{255,255,255};
  LoadText(nameInstruction, menu_fonts, textColor);
  RenderText((screen_width-textSurfaceWidth)/2, (screen_height-textSurfaceHeight));

}

// Render Game Play
void Renderer::Render(Snake const &snake, Food* normal_food, Food* super_food, /*std::vector<std::shared_ptr<Food>*> const &food_list,*/ std::shared_ptr<Game> game) {

  // Clear screen
  SDL_SetRenderDrawColor(sdl_renderer, 0x1E, 0x1E, 0x1E, 0xFF);
  SDL_RenderClear(sdl_renderer);
  
  
  // Render food
  std::lock_guard<std::mutex> lck_grd_food(game->mtx_food);
  if(normal_food) {
    SDL_SetRenderDrawColor(sdl_renderer, normal_food->color.r, normal_food->color.g, normal_food->color.b, normal_food->color.a);
    SetBlock(normal_food->point.x, normal_food->point.y,normal_food->size,normal_food->size);
  }
  if(super_food) {
    SDL_SetRenderDrawColor(sdl_renderer, super_food->color.r, super_food->color.g, super_food->color.b, super_food->color.a);
    SetBlock(super_food->point.x, super_food->point.y,super_food->size,super_food->size);

  }

  // Render snake's body  
  SDL_SetRenderDrawColor(sdl_renderer, snake.color.r, snake.color.g,snake.color.b,snake.color.a);

  std::unique_lock<std::mutex> uLockSnake(game->mtx_snake);
  for(auto point = snake.body.begin();point!=snake.body.end();point++) {
    // SetBlock(block, point->x * block.w, point->y * block.h);
    SetBlock( point->x , point->y,1,1);
  }

  // Render snake's head
  if (snake.alive) {
    SDL_SetRenderDrawColor(sdl_renderer, 0x00, 0x7A, 0xCC, 0xFF);
  } 
  else {
    SDL_SetRenderDrawColor(sdl_renderer, 0xFF, 0x00, 0x00, 0xFF);
  }
  SetBlock(static_cast<int>(snake.head_x), static_cast<int>(snake.head_y), 1,1);
  uLockSnake.unlock();
  
  //Add Score to the Game 
  RenderScore(game);
  // Add Timers to the Game
  RenderTimers(game);

  //render current frames per second
  RenderFPS(game);
  
  
}

void Renderer::RenderFPS(std::shared_ptr<Game> game) {
  std::string fps = "FPS : "+std::to_string(game->GetFPS());
  LoadText(fps, state_fonts, textColor);
  RenderText(screen_width-textSurfaceWidth, screen_height-textSurfaceHeight);
}


void Renderer::SetBlock(int x, int y, int h, int w) {
  SDL_Rect block;
  block.w = (screen_width / grid_width);
  block.h = (screen_height / grid_height);
  block.x = x*block.w;
  block.y = y*block.h;
  block.w*=w;
  block.h*=h;
  SDL_RenderFillRect(sdl_renderer, &block);
}



// Render Score 
void Renderer::RenderScore(std::shared_ptr<Game> game) {
  // Render Current Score
  std::string score_str = "Score : "+std::to_string(game->GetScore());
  textColor = 	{255,0,0};
  LoadText(score_str, state_fonts, textColor);
  RenderText(0,0);

  // Render High Score
  std::string high_score = "High Score : "+std::to_string(game->GetTopScore());
  LoadText(high_score,state_fonts, textColor);
  RenderText(screen_width-textSurfaceWidth, 0);
}

// Render Timers
void Renderer::RenderTimers(std::shared_ptr<Game> game){
  // Add super food timer to renderer
  if(game->GetSuperFoodRemainingTimer()>0) {
    std::string super_food_timer_str = "Super Food : " + std::to_string(game->GetSuperFoodRemainingTimer());
    LoadText(super_food_timer_str, state_fonts, textColor);
    RenderText(0, textSurfaceHeight);
  }

  // Add super snake timer to renderer
  if(game->GetSuperSnakeRemainingTimer()>0) {
    std::string supersnake_timer_str = "Super Snake : " + std::to_string(game->GetSuperSnakeRemainingTimer());
    LoadText(supersnake_timer_str, state_fonts, textColor);
    RenderText(0,2*textSurfaceHeight);
  }
}


// Create text surface from text
void Renderer::LoadText(std::string textureText, TTF_Font *fonts, SDL_Color textColor) {
  SDL_Surface* textSurface = TTF_RenderText_Solid( fonts, textureText.c_str(), textColor);
  if( textSurface != NULL ) {
		//Create texture from surface
    texture = SDL_CreateTextureFromSurface( sdl_renderer, textSurface );
		if( texture == NULL ) {
			printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
		}
		else {
			textSurfaceWidth = textSurface->w;
			textSurfaceHeight = textSurface->h;
		}
		//Get rid of old surface
		SDL_FreeSurface( textSurface );
	}
	else {
		printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
	}
}


// add text surface to renderer
void Renderer::RenderText(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip) {
  SDL_Rect block = {x,y,textSurfaceWidth,textSurfaceHeight};

  if( clip != NULL ) {
		block.w = clip->w;
		block.h = clip->h;
	}
	//Render to screen
	SDL_RenderCopyEx( sdl_renderer, texture, clip, &block, angle, center, flip );
}


Renderer::~Renderer() {
  SDL_DestroyWindow(sdl_window);
  SDL_Quit();
}