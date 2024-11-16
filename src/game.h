#ifndef GAME_H
#define GAME_H

#include "SDL.h"
#include "snake.h"
#include "menu.h"
#include "food.h"
#include"scoreManager.h"
#include"controller.h"
#include "renderer.h"
#include <random>
#include <stack>
#include<memory>
#include<condition_variable>
#include<mutex>
#include<chrono>




enum class GAMESTATE  {ON_MENU, NAME_INPUT, RUNNING, PAUSED, END};
// enum class MENU_ACTION {START, SELECT_SPEED, HIGH_SCORE, EXIT, ESCAPE, RESTART, RESUME, EASY, NORMAL, HARD, EXPERT};
class Game : public std::enable_shared_from_this<Game>{
 public:
  Game(std::size_t grid_width, std::size_t grid_height, std::string score_file);
  void Run(std::size_t target_frame_duration, Controller &controller, Renderer &renderer);
  void SetPlayerName(std::string name);
  std::string GetPlayerName() const {return player_name;}
  int GetScore() const;
  int GetSize() const;
  Menu* GetCurrentMenu() ;
  Food* GetNormalFood() const;
  Food* GetSuperFood() const;
  int GetSuperFoodRemainingTimer();
  int GetSuperSnakeRemainingTimer();
  int GetTopScore() ;
  int GetFPS() const;
  void SuperFoodTimer(int super_food_seconds);
  void SuperSnakeTimer(int super_snake_seconds);
  


  GAMESTATE state;
  MENU_ACTION menu_action;
  bool is_menu_option_selected = false;
  bool is_name_input_submitted = false;
  Snake snake;

  std::mutex mtx_food;
  std::mutex mtx_snake;
  

 private:
  
  std::string player_name = "";
  int frame_count = 0;
  std::stack<std::shared_ptr<Menu>> menu_stack;
  std::shared_ptr<Food> normal_food;
  std::shared_ptr<Food> super_food;

  bool new_super_snake_timer = false;

  int score{0};
  int super_food_remain_seconds;
  int super_snake_remain_seconds;
  ScoreManager score_manager;

  std::random_device dev;
  std::mt19937 engine;
  std::uniform_int_distribution<int> random_w;
  std::uniform_int_distribution<int> random_h;
  std::uniform_int_distribution<int> random_type;
  
  std::condition_variable cv_food;
  std::condition_variable cv_snake;

  
  std::size_t grid_w;
  std::size_t grid_h;
  
  void PlaceFood();
  void Action();
  void Update();
  void MenuAction();
  void Reset();
};

#endif