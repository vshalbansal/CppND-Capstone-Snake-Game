#ifndef GAME_H
#define GAME_H

#include "SDL.h"
#include "snake.h"
#include "menu.h"
#include "food.h"
#include"timer.h"
#include"scoreManager.h"
#include <random>
#include <stack>
#include<memory>
#include<condition_variable>
#include<mutex>
#include<chrono>




enum class GAMESTATE  {ON_MENU, NAME_INPUT,BEGIN, RUNNING, PAUSED, END};
// enum class MENU_ACTION {START, SELECT_SPEED, HIGH_SCORE, EXIT, ESCAPE, RESTART, RESUME, EASY, NORMAL, HARD, EXPERT};
class Game : public std::enable_shared_from_this<Game>{
 public:
  Game(std::size_t grid_width, std::size_t grid_height, std::string score_file);
  void Run(std::size_t target_frame_duration);
  void SetPlayerName(std::string name);
  std::string GetPlayerName() const {return player_name;}
  int GetScore() const;
  int GetSize() const;
  Menu* GetCurrentMenu() ;
  Food* GetNormalFood() const;
  Food* GetSuperFood() const;
  int GetSuperFoodRemainingTimer() ;
  int GetSuperSnakeRemainingTimer();
  int GetTopScore() ;
  int GetFPS() const;
  


  GAMESTATE state;
  MENU_ACTION menu_action;
  bool do_menu_action = false;

  std::condition_variable cv_game;
  std::mutex mtx_menu;
  std::mutex mtx_food;
  std::mutex mtx_snake;
  std::mutex mtx_game;

  Snake snake;
  

 private:
  
  std::string player_name = "";
  int frame_count = 0;
  std::stack<std::shared_ptr<Menu>> menu_stack;
  std::shared_ptr<Food> normal_food;
  std::shared_ptr<Food> super_food;
  int score{0};
  Timer super_food_timer;
  Timer super_snake_timer;
  ScoreManager score_manager;

  std::random_device dev;
  std::mt19937 engine;
  std::uniform_int_distribution<int> random_w;
  std::uniform_int_distribution<int> random_h;
  std::uniform_int_distribution<int> random_type;
  
  std::size_t grid_w;
  std::size_t grid_h;
  
  void PlaceFood();
  void Action();
  void Update();
  void MenuAction();
  void Reset();
};

#endif