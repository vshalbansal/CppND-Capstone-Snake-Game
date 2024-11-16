#include "controller.h"
#include <iostream>
#include "SDL.h"
#include "snake.h"
#include "menu.h"
#include "game.h"
#include<mutex>
#include<memory>
#include<thread>

void Controller::ChangeDirection(Snake &snake, Snake::Direction input, Snake::Direction opposite) const {
  
  if (snake.alive && (snake.direction != opposite || snake.size == 1) && snake.changed_dir) {
    snake.direction = input;
    snake.changed_dir = false;
  }
  return;
}


void Controller::HandleInput(std::shared_ptr<Game> game) {
  SDL_Event e;
  SDL_PollEvent(&e);
  if (e.type == SDL_QUIT) {
    game->state = GAMESTATE::END;
    return;
  }
  else {
    switch(game->state) {
      case GAMESTATE::NAME_INPUT: {
        SDL_StartTextInput();
        std::string player_name = game->GetPlayerName();
        HandleNameInput(game,e,player_name); 
        break;
      }
      case GAMESTATE::ON_MENU:
        HandleMenuInput(game->GetCurrentMenu(), game, e);
        break;
      case GAMESTATE::RUNNING:
        HandleGameInput( game->snake, game, e);
        break;
    }
  }
}


void Controller::HandleMenuInput(Menu *menu, std::shared_ptr<Game> game, SDL_Event &e) const {
  
  if(e.type == SDL_KEYDOWN) {
    switch (e.key.keysym.sym) {
      case SDLK_UP:
        menu->Select((((menu->Selected()-1) % menu->Size()) + menu->Size()) % menu->Size());
        break;
      case SDLK_DOWN:
        menu->Select( (menu->Selected()+1) % menu->Size() );
        break;
      case SDLK_RETURN:
        game->menu_action = menu->Action();
        game->is_menu_option_selected = true;
        // game->cv_game.notify_one();
        break;
      case SDLK_ESCAPE:
        game->menu_action = MENU_ACTION::ESCAPE;
        game->is_menu_option_selected = true;
        break;
    }      
  }

}



void Controller::HandleGameInput(Snake &snake, std::shared_ptr<Game> game, SDL_Event &e)  const {
  // SDL_Event e;
  
  if (e.type == SDL_KEYDOWN) {
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
        game->state = GAMESTATE::PAUSED;      
        break;
    }
  }
}

void Controller::HandleNameInput(std::shared_ptr<Game> game, SDL_Event &e, std::string &player_name) {

  if(e.type == SDL_TEXTINPUT) {
    if( !( SDL_GetModState() & KMOD_CTRL && ( std::toupper(e.text.text[ 0 ]) == 'C' || std::toupper(e.text.text[ 0 ]) == 'V' ) ) ) {
      //Append character, maximum allowed size for player name is 20
      if(player_name.size()<=20) {
        player_name += std::string(e.text.text);
        game->SetPlayerName(std::move(player_name));
      }
    }
  }
  else if(e.type == SDL_KEYDOWN) {
    if(e.key.keysym.sym == SDLK_BACKSPACE && player_name.length()>0) {
      player_name.pop_back();
      game->SetPlayerName(std::move(player_name));
    }
    else if(e.key.keysym.sym == SDLK_ESCAPE ) {
      game->SetPlayerName("Anonymous");
      game->is_name_input_submitted = true;
      SDL_StopTextInput();
    }
    else if(e.key.keysym.sym == SDLK_RETURN) {
      if(player_name=="")
        game->SetPlayerName("Anonymous");
      else
        game->SetPlayerName(std::move(player_name));
      game->is_name_input_submitted = true;
      SDL_StopTextInput();
    }
  }  
}
