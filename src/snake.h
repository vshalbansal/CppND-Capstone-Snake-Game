#ifndef SNAKE_H
#define SNAKE_H

#include <vector>
// #include <functional>
#include<unordered_set>
// #include<utility>
#include <mutex>
#include<condition_variable>
#include "SDL.h"
#include<string>


// struct PairHash {
//     template <class T1, class T2>
//     std::size_t operator()(const std::pair<T1, T2>& p) const {
//         auto hash1 = std::hash<T1>{}(p.first);
//         auto hash2 = std::hash<T2>{}(p.second);
//         return hash1 ^ (hash2 << 1); // or use another method to combine hashes
//     }
// };

// struct PairEqual {
//     template <class T1, class T2>
//     bool operator()(const std::pair<T1, T2>& p1, const std::pair<T1, T2>& p2) const {
//         return p1.first == p2.first && p1.second == p2.second;
//     }
// };

class Snake {
 public:
  enum class Direction { kUp, kDown, kLeft, kRight };

  Snake(int grid_width, int grid_height)
      : grid_width(grid_width),
        grid_height(grid_height),
        head_x(grid_width / 2),
        head_y(grid_height / 2) {}
  // void Handle(Game &game);
  void Update();
  void GrowBody();
  bool SnakeCell(int x, int y);
  void Reset();
  

  Direction direction = Direction::kUp;

  float speed{0.1f};
  int size{1};
  bool alive{true};
  float head_x;
  float head_y;
  std::vector<SDL_Point> body;
//   std::unordered_set<std::pair<int, int>, PairHash, PairEqual> umset_body;
  std::unordered_multiset<std::string> umset_body;
  bool can_die{true};
  SDL_Color color {0xFF, 0xFF, 0xFF, 0xFF};
  std::condition_variable cv_update;
  bool changed_dir = false;
  std::mutex _mutex_dir;

 private:
  void UpdateHead();
  void UpdateBody(SDL_Point &current_cell, SDL_Point &prev_cell);
  
  bool growing{false};
  int grid_width;
  int grid_height;
  
};

#endif