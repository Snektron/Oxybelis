#include "utility/Profiler.h"

using std::literals::chrono_literals::operator ""ms;
using std::chrono::high_resolution_clock;

Profiler::Profiler():
    statistics{0, 0ms, 0ms} {
}

void Profiler::start() {
    this->start_time = high_resolution_clock::now();
}

void Profiler::stop() {
    auto stop_time = high_resolution_clock::now();
    ++this->statistics.times_measured;
    this->statistics.total_time += std::chrono::duration_cast<std::chrono::milliseconds>(stop_time - this->start_time);
    this->statistics.average_time = this->statistics.total_time / this->statistics.times_measured;
}

std::ostream& operator<<(std::ostream& os, const Statistics& stats) {
    os << "Statistics:\n\tTimes measured: " << stats.times_measured
        << "\n\tTotal time: " << stats.total_time.count() << "ms\n\tAverage time: "
        << stats.average_time.count() << "ms\n";
    return os;
}
