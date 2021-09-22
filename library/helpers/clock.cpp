#include "clock.h"

Clock::Clock() {
    restart();
}

double Clock::getSeconds() const {
    return getMilliseconds() / 1000.0;
}

double Clock::getMilliseconds() const {
    return double(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count()) / 1000.0;
}