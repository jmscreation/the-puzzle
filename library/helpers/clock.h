#ifndef __CLOCK_H__
#define __CLOCK_H__

#include <chrono>

typedef std::chrono::time_point<std::chrono::high_resolution_clock> timepoint;

class Clock {
    timepoint start;
public:
    Clock();
    virtual ~Clock()=default;
    double getSeconds() const;
    double getMilliseconds() const;
    inline void restart() { start = std::chrono::high_resolution_clock::now(); }
};

#endif // __CLOCK_H__