#include "game.h"
#include <iostream>
#include<thread>


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

void Game::Run(std::size_t target_frame_duration, Controller &controller, Renderer &renderer) {
  std::cout<<"Game started\n";
  Uint32 title_timestamp = SDL_GetTicks();;
  Uint32 frame_start;
  Uint32 frame_end;
  Uint32 frame_duration;
  int current_frame_count = 0;
  
  while (state!=GAMESTATE::END) {
    frame_start = SDL_GetTicks();
    // Handle input
    controller.HandleInput(shared_from_this());
    // Update game
    Update();
    // Render game
    renderer.Render(shared_from_this());
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
      if(is_menu_option_selected) {
        MenuAction();
        is_menu_option_selected = false;
      }
      break;
    }
    case GAMESTATE::NAME_INPUT: {
      if(is_name_input_submitted) {
        menu_stack.push(std::make_shared<StartMenu>());
        state = GAMESTATE::ON_MENU;
      }
      break;
    }
    
    case GAMESTATE::PAUSED: {
      if(super_food)
        cv_food.notify_one(); 
      // -----super food timer pause
      if(!snake.can_die)
        cv_snake.notify_one();
      // -----super snake timer pause
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

  snake.Update();
  int new_x = static_cast<int>(snake.head_x);
  int new_y = static_cast<int>(snake.head_y);
  
  // check if there is food present at snake head location
  std::unique_lock<std::mutex> uLock_food(mtx_food);
  if(normal_food->IsOccupied(new_x,new_y)) {
    score+= normal_food->size;
    normal_food.reset();
    PlaceFood();
    uLock_food.unlock();
    snake.GrowBody();
    
  }
  else if(super_food && super_food->IsOccupied(new_x, new_y)) {    
    score+= super_food->size;
    if(super_food->type==FoodType::MAGIC) {
      snake.can_die = false;
      snake.color = {0,0,128,255};
      new_super_snake_timer = true;
      cv_snake.notify_one();
      std::thread super_snake_thread = std::thread(&Game::SuperSnakeTimer, this, 10);
      super_snake_thread.detach();
      // super_snake_timer.Reset(10);
    }
    super_food.reset();
    uLock_food.unlock();
    cv_food.notify_one();
    //-------super food timer end
    snake.GrowBody();

  }
  else 
    uLock_food.unlock();
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
            std::thread super_food_thread = std::thread(&Game::SuperFoodTimer, this, 10);
            super_food_thread.detach();
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
            std::thread super_food_thread = std::thread(&Game::SuperFoodTimer, this, 10);
            super_food_thread.detach();
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
      menu_stack.pop();
      if(!normal_food)
        PlaceFood();
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
      menu_stack.pop();
      state = GAMESTATE::END;
      break;
    }
    case MENU_ACTION::RESTART: {
      Reset();
      menu_stack.pop();
      menu_stack.push(std::make_shared<StartMenu>());
      break; 
    }
    case MENU_ACTION::RESUME: {
      menu_stack.pop();
      state = GAMESTATE::RUNNING;
      if(super_food) {
        std::thread super_food_thread = std::thread(&Game::SuperFoodTimer, this,super_food_remain_seconds);
        super_food_thread.detach();
      }
      if(!snake.can_die){
        std::unique_lock<std::mutex> uLock(mtx_snake);
        new_super_snake_timer = true;
        std::thread super_snake_thread = std::thread(&Game::SuperSnakeTimer, this, super_snake_remain_seconds);
        super_snake_thread.detach();
      }
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


void Game::SuperFoodTimer(int super_food_seconds) {
  std::chrono::time_point<std::chrono::system_clock> start;
  start = std::chrono::system_clock::now();
  std::unique_lock<std::mutex> uLock(mtx_food);
  super_food_remain_seconds = super_food_seconds;
  while(super_food && state == GAMESTATE::RUNNING) {
    uLock.unlock();
    int elapsed_seconds = (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start)).count();
    super_food_remain_seconds = super_food_seconds - elapsed_seconds;
    if(elapsed_seconds>=super_food_seconds) {
      super_food.reset();
      break;
    }
    uLock.lock();
    cv_food.wait_for(uLock,std::chrono::milliseconds(500), [this](){return state!=GAMESTATE::RUNNING || !GetSuperFood();});
  }
}
void Game::SuperSnakeTimer(int super_snake_seconds) {
  std::lock_guard<std::mutex> lck_guard(mtx_snake);
  super_snake_remain_seconds = super_snake_seconds;
  new_super_snake_timer = false;
  std::chrono::time_point<std::chrono::system_clock> start;
  start = std::chrono::system_clock::now();
  
  while(!snake.can_die && state == GAMESTATE::RUNNING && !new_super_snake_timer) {
    // uLock.unlock();
    int elapsed_seconds = (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start)).count();
    super_snake_remain_seconds = super_snake_seconds - elapsed_seconds;
    if(elapsed_seconds>=super_snake_seconds) {
      snake.can_die = true;
      snake.color =  {0xFF,0xFF,0xFF,0xFF};
      break;
    }
    // uLock.lock();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    // cv_food.wait_for(uLock,std::chrono::milliseconds(100), [this](){return  (new_super_snake_timer||state!= GAMESTATE::RUNNING ) ; });
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
    return super_food_remain_seconds;
  else
    return 0;

}

int Game::GetSuperSnakeRemainingTimer() {
  if(!snake.can_die)
    return super_snake_remain_seconds;
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