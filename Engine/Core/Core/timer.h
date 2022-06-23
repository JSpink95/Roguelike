#pragma once
#include "Core.h"
#include "Core/Object.h"

#include <chrono>
#include <functional>

struct Timer
{
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = Clock::time_point;
    using Duration = std::chrono::duration<double>;

    Timer(bool start = false)
    {
        if (start)
        {
            Mark();
        }
    }

    double Mark()
    {
        double elapsed = ElapsedTime();

        startTime = Clock::now();

        return elapsed;
    }

    double ElapsedTime() const
    {
        return std::chrono::duration_cast<Duration>(Clock::now() - startTime).count();
    }

    TimePoint startTime;
};

struct EventTimer: public Object
{
    BIND_OBJECT(EventTimer);

    using OnTimerElapsed = std::function<bool()>;

    static inline Ref Create(bool start, float delayInSeconds, OnTimerElapsed onElapsed)
    {
        return std::make_shared<EventTimer>(start, delayInSeconds, onElapsed);
    }

    EventTimer(bool start, float timeDelay, OnTimerElapsed onElapsed);

    inline bool IsFinished() const { return onTimerElapsed == nullptr; }
    inline bool HasElapsed() const { return (float)timer.ElapsedTime() > length; }
    inline double ElapsedTime() const { return timer.ElapsedTime(); }

    void StartTimer();

    void Update(float deltaTime);
    void OnTimerFinish();

    Timer timer;
    float length = 0.0f;
    OnTimerElapsed onTimerElapsed = nullptr;
};