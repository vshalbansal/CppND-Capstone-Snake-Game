#include "timer.h"
#include<iostream>

Timer::Timer():
    start(std::chrono::system_clock::now()),
    remaining_ticks(0) {
    
}
void Timer::Reset(int ticks){
    start = std::chrono::system_clock::now();
    remaining_ticks = ticks;
}
void Timer::Pause() {
    remaining_ticks = remaining_ticks - (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start)).count();
}
void Timer::Start() {
    start = std::chrono::system_clock::now();
}
int Timer::Remaining() {
    return remaining_ticks - (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start)).count();
}