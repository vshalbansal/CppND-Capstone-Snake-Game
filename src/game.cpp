#include "game.h"
#include <iostream>


Game::Game(std::size_t grid_width, std::size_t grid_height, std::string score_file)
    :snake(grid_width, grid_height),
      grid_h(grid_height),
      grid_w(grid_width),
      engine(dev()),
      random_w(0, static_cast<int>(grid_width - 1)),
      random_h(0, static_cast<int>(grid_height - 1)),
      random_type(1,100),
      score_manager(score_file),
      state(GAMESTATE::NAME_INPUT)
      {  
}

void Game::Run(std::size_t target_frame_duration) {
  std::cout<<"Game started\n";
  Uint32 title_timestamp = SDL_GetTicks();;
  Uint32 frame_start;
  Uint32 frame_end;
  Uint32 frame_duration;
  int current_frame_count = 0;
  
  while (state!=GAMESTATE::END) {
    frame_start = SDL_GetTicks();
    Update();
    frame_end = SDL_GetTicks();
    // Keep track of how long each loop through the update cycle takes.
    current_frame_count++;
    frame_duration = frame_end - frame_start;
    

    // After every second, update the frame counter.
    if (frame_end - title_timestamp >= 1000) {
      frame_count = current_frame_count;
      current_frame_count = 0;
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



void Game::Update() {
  switch(state) {
    case GAMESTATE::ON_MENU: {
      // wait for controller to signal menu action
      std::unique_lock<std::mutex> uLock(mtx_game);
      cv_game.wait(uLock, [this] {return do_menu_action || state == GAMESTATE::END;});
      MenuAction();
      do_menu_action = false;
      break;
    }
    case GAMESTATE::NAME_INPUT: {
      // wait for controller to signal name input confirmation
      std::unique_lock<std::mutex> uLock(mtx_game);
      cv_game.wait(uLock, [this] {return state == GAMESTATE::BEGIN || state == GAMESTATE::END;});
      break;
    }
    // when game starts first time after name input
    case GAMESTATE::BEGIN: {
      menu_stack.push(std::make_shared<StartMenu>());
      state = GAMESTATE::ON_MENU;
      break;
    }
    case GAMESTATE::PAUSED: {
      if(super_food) 
        super_food_timer.Pause();
      if(!snake.can_die)
        super_snake_timer.Pause();
      menu_stack.push(std::make_shared<PauseMenu>());
      state = GAMESTATE::ON_MENU;
      break;
    }
    
    case GAMESTATE::RUNNING:
      Action();
      break;
  }
}


void Game::Action() {
  if (!snake.alive) {
    score_manager.PutScore(score, player_name);
    menu_stack.push(std::make_shared<EndMenu>());
    state = GAMESTATE::ON_MENU;
    return;
  }

  std::unique_lock<std::mutex> uLockSnake(mtx_snake);
  snake.Update();
  uLockSnake.unlock();
  int new_x = static_cast<int>(snake.head_x);
  int new_y = static_cast<int>(snake.head_y);
  
  // check if super food has timed out
  if(super_food_timer.Remaining()<=0) {
    super_food.reset();
  }

  // check if super snake has timed out
  if(super_snake_timer.Remaining()<=0) {
    snake.can_die = true;
    snake.color =  {0xFF,0xFF,0xFF,0xFF};
  }

  // check if there is food present at snake head location
  if(normal_food->IsOccupied(new_x,new_y)) {
    score+= normal_food->size;
    std::lock_guard<std::mutex> lckGrd(mtx_food);
    normal_food.reset();
    PlaceFood();
    snake.GrowBody();
  }
  else if(super_food && super_food->IsOccupied(new_x, new_y)) {    
    score+= super_food->size;
    if(super_food->type==FoodType::MAGIC) {
      snake.can_die = false;
      snake.color = {0,0,128,255};
      super_snake_timer.Reset(10);
    }
    super_food.reset();
    snake.GrowBody();

  }
  // snake.speed += 0.02;
}


void Game::PlaceFood() {
  int x, y, type;
  // keep creating food until normal food variable is set
  while (true) {
    x = random_w(engine);
    y = random_h(engine);
    type = random_type(engine);
    std::shared_ptr<Food> new_food = std::make_shared<Food>(x,y,type);
    // if new food coordinates are not occupied by snake 
    if(!snake.SnakeCell(x,y)) {
      switch (new_food->type) {
        // normal food only need to ensure no super food already exists at x,y
        case FoodType::NORMAL: {
          if(!super_food || !super_food->IsOccupied(x,y) ) {
            normal_food.swap(new_food);
            return;
          }
          break;
        }
        // magic food need to ensure no super food already exists and no normal food exists at x,y
        case FoodType::MAGIC: {
          if(!super_food && ( !normal_food || !normal_food->IsOccupied(x,y) ) ) {
            super_food.swap(new_food);
            super_food_timer.Reset(10);
          }
          break;
        }
        // Big food needs to ensure no super food, no snake cell at four possible points of big food, and no normal food at x,y
        // also x or y should not be the last point on the grid in any direction
        case FoodType::BIG: {
          if(!super_food && 
            (!snake.SnakeCell(x+1,y) && !snake.SnakeCell(x,y+1) && !snake.SnakeCell(x+1,y+1)) &&  ( !normal_food || !normal_food->IsOccupied(x,y) ) &&
            (x!=grid_w-1 && y!=grid_h-1)) {
            super_food.swap(new_food);
            super_food_timer.Reset(10);
          }
          break;
        }
          
      }
    }
  }
}


void Game::MenuAction() {
  switch(menu_action) {
    case MENU_ACTION::ESCAPE: {
      // do not remove the last menu on the stack
      // as long as game state is on_menu
      if(menu_stack.size()>1)
        menu_stack.pop();
      break;
    }
    case MENU_ACTION::START: {
      std::lock_guard<std::mutex> lckGrd(mtx_menu);
      menu_stack.pop();
      if(!normal_food){
        std::lock_guard<std::mutex> lckGrd(mtx_food);
        PlaceFood();
      }
      state = GAMESTATE::RUNNING;
      break;
    }
    case MENU_ACTION::SELECT_SPEED: {
      menu_stack.push(std::make_shared<SpeedMenu>());
      break;

    }
    case MENU_ACTION::HIGH_SCORE: {
      menu_stack.push(std::make_shared<ScoreMenu>(score_manager.ListTopScores()));
      break;
    }
    case MENU_ACTION::EXIT: {
      std::lock_guard<std::mutex> lckGrd(mtx_menu);
      menu_stack.pop();
      state = GAMESTATE::END;
      break;
    }
    case MENU_ACTION::RESTART: {
      Reset();
      std::lock_guard<std::mutex> lckGrd(mtx_menu);
      menu_stack.pop();
      menu_stack.push(std::make_shared<StartMenu>());
      break; 
    }
    case MENU_ACTION::RESUME: {
      std::lock_guard<std::mutex> lckGrd(mtx_menu);
      menu_stack.pop();
      if(super_food)
        super_food_timer.Start();
      if(!snake.can_die)
        super_snake_timer.Start();
      state = GAMESTATE::RUNNING;
      break;
    }
    case MENU_ACTION::EASY: {
      snake.speed = 0.05f;
      menu_stack.pop();
      break;
    }
    case MENU_ACTION::NORMAL: {
      snake.speed = 0.1f;
      menu_stack.pop();
      break;
    }
    case MENU_ACTION::HARD: {
      snake.speed = 0.15f;
      menu_stack.pop();
      break;
    }
    case MENU_ACTION::EXPERT: {
      snake.speed = 0.20f;
      menu_stack.pop();
      break;
    }
  }
}

void Game::SetPlayerName(std::string name) {
  // if(name=="")
  //   player_name = "Anonymous";
  if(name.size()<=20)
    player_name = name;
  else
    player_name = name.substr(0,20);
}

// get current game score
int Game::GetScore() const { return score; }

// get current snake size
int Game::GetSize() const { return snake.size; }

// get current active menu
Menu* Game::GetCurrentMenu() { 
  if(!menu_stack.empty()) 
    return menu_stack.top().get();
  else
    return nullptr;
}


Food* Game::GetNormalFood() const {
  if(normal_food)
    return normal_food.get();
  else
    return nullptr;
}


Food* Game::GetSuperFood() const {
  if(super_food)
    return super_food.get();
  else
    return nullptr;
}

int Game::GetSuperFoodRemainingTimer()  {
  if(super_food)
    return super_food_timer.Remaining();
  else
    return 0;

}

int Game::GetSuperSnakeRemainingTimer() {
  if(!snake.can_die)
    return super_snake_timer.Remaining();
  else
    return 0;
}

int Game::GetTopScore() {
  return score_manager.GetTopScore();
}

int Game::GetFPS() const {
  return frame_count;
}

// reset game for restarting
void Game::Reset() {
  score = 0;
  snake.Reset();
  super_food.reset();
  normal_food.reset();
}