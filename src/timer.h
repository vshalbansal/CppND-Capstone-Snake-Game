#ifndef TIMER_H
#define TIMER_H

#include<chrono>

class Timer {
    public:
    Timer();
    int Remaining();
    void Pause();
    void Start();
    void Reset(int ticks);
    private:
    std::chrono::time_point<std::chrono::system_clock> start;
    int remaining_ticks;
};

#endif