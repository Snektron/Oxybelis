#ifndef _OXYBELIS_UTILITY_TIMER_H
#define _OXYBELIS_UTILITY_TIMER_H

#include <chrono>
#include <iostream>
#include <string>

class Timer {
    std::string text;
    std::chrono::time_point<std::chrono::high_resolution_clock> start;

public:
    Timer(const std::string& text):
        text(text), start(std::chrono::high_resolution_clock::now()) {
    }

    ~Timer() {
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> dur = end - this->start;

        std::cout << this->text << dur.count() << "s" << std::endl;
    }
};

#endif
