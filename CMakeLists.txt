cmake_minimum_required(VERSION 3.7)

project(SnakeGame)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-std=c++17 )

set(THREADS_PREFER_PTHREAD_FLAG ON)
set(CXX_FLAGS "-Wall -lstdc++fs -pthread")
set(CMAKE_CXX_FLAGS, "${CXX_FLAGS}")



#project(SnakeGame)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

find_package(Threads REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)


include_directories(${SDL2_INCLUDE_DIRS} src)
include_directories(${SDL_TTF_INCLUDE_DIRS} src)


add_executable(SnakeGame src/scoreManager.cpp src/food.cpp src/main.cpp src/menu.cpp src/game.cpp src/controller.cpp src/renderer.cpp src/snake.cpp )
string(STRIP ${SDL2_LIBRARIES} SDL2_LIBRARIES)
target_link_libraries(SnakeGame SDL2 SDL2_ttf)

target_link_libraries(SnakeGame ${SDL2_LIBRARIES} ${SDL2_ttf})
target_link_libraries(SnakeGame ${SDL2_ttf_LIBRARIES})
target_link_libraries(SnakeGame Threads::Threads)

