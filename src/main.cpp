#include <iostream>
#include "controller.h"
#include "game.h"
#include "renderer.h"
#include <thread>
#include "SDL.h"
#include<memory>

int main() {
  constexpr std::size_t kFramesPerSecond{60};
  constexpr std::size_t kMsPerFrame{1000 / kFramesPerSecond};
  constexpr std::size_t kScreenWidth{640};
  constexpr std::size_t kScreenHeight{640};
  constexpr std::size_t kGridWidth{32};
  constexpr std::size_t kGridHeight{32};
  
  std::string score_file = "../data/high_scores.txt";
  Renderer renderer(kScreenWidth, kScreenHeight, kGridWidth, kGridHeight);
  Controller controller ;
  
  std::shared_ptr<Game> game = std::make_shared<Game>(kGridWidth, kGridHeight, score_file);
  game->Run(kMsPerFrame,controller,renderer);


  std::cout << "Game has terminated successfully!\n";
  std::cout << "Score: " << game->GetScore() << "\n";
  std::cout << "Size: " << game->GetSize() << "\n";
  return 0;
}