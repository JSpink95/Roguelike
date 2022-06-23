#include "Core\timer.h"

EventTimer::EventTimer(bool start, float timeDelay, OnTimerElapsed onElapsed)
    : timer(start)
    , length(timeDelay)
    , onTimerElapsed(onElapsed)
{

}

void EventTimer::StartTimer()
{
    timer.Mark();
}

void EventTimer::Update(float deltaTime)
{
    if (IsFinished())
        return;

    if (HasElapsed())
        OnTimerFinish();
}

void EventTimer::OnTimerFinish()
{
    bool hasFinished = true;
    if (onTimerElapsed)
        hasFinished = onTimerElapsed();

    if (hasFinished)
        onTimerElapsed = nullptr;
}