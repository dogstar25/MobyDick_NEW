#include "Timer.h"
#include <algorithm> 

Timer::Timer(float targetDuration, bool autoReset) : m_targetDuration(targetDuration), m_autoReset(autoReset)
{

    if (targetDuration == 0.) {
        m_infiniteLifetime = true;
    }

}

std::chrono::duration<float> Timer::timeElapsed() 
{ 
    std::chrono::duration<float> timeDiff = std::chrono::steady_clock::now() - m_timeSnapshot;
    return timeDiff;
}

float Timer::percentTargetMet()
{
    float percentTargetMet{};
    if (m_infiniteLifetime == false) {
        percentTargetMet = (m_targetDuration - timeRemaining()) / m_targetDuration;
    }
    return percentTargetMet;
}

std::chrono::duration<float> Timer::timeRemaining()
{
    auto timeRemaining =  std::max((m_targetDuration - timeElapsed()).count(), (float)0);

    return std::chrono::duration<float>(timeRemaining);
}

bool Timer::hasMetTargetDuration()
{
    if (m_infiniteLifetime == false) {

        std::chrono::duration<float> timeDiff = timeElapsed();

        if (timeDiff >= m_targetDuration) {

            //reset the timer
            if (m_autoReset) {
                reset();
            }

            return true;
        }
    }

    return false;
}

void Timer::reset()
{
    m_timeSnapshot = std::chrono::steady_clock::now();

}

bool Timer::isFirstTime()
{
    if (m_firstTime) {
        m_firstTime = false;
        return true;
    }

    return false;
    

}
