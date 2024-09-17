cmake_minimum_required(VERSION 3.7)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-std=c++17)

set(CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS, "${CXX_FLAGS}")

project(SDL2Test)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS} src)
include_directories(${SDL_TTF_INCLUDE_DIRS} src)

add_executable(SnakeGame src/main.cpp src/menu.cpp src/game.cpp src/controller.cpp src/renderer.cpp src/snake.cpp src/menu.cpp)
string(STRIP ${SDL2_LIBRARIES} SDL2_LIBRARIES)
target_link_libraries(SnakeGame SDL2 SDL2_ttf)
# target_link_libraries(SnakeGame ${SDL2_LIBRARIES} ${SDL2_ttf})
# target_link_libraries(SnakeGame ${SDL2_ttf_LIBRARIES})
