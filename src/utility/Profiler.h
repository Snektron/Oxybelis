#ifndef _OXYBELIS_UTILITY_PROFILER_H
#define _OXYBELIS_UTILITY_PROFILER_H

#include <chrono>
#include <iostream>
#include <cstddef>

struct Statistics {
    size_t times_measured;
    std::chrono::duration<double, std::milli> total_time;
    std::chrono::duration<double, std::milli> average_time;
};

class Profiler {
    Statistics statistics;

    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;

public:
    Profiler();
    void start();
    void stop();

    const Statistics& stats() const {
        return this->statistics;
    }
};

std::ostream& operator<<(std::ostream& os, const Statistics& stats);

#endif
