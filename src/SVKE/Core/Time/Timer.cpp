#include "SVKE/Core/Time/Timer.hpp"

vk::Timer::Timer()
{
    restart();
}

void vk::Timer::restart()
{
    startTPoint = std::chrono::steady_clock::now();
}

const float vk::Timer::getElapsedTimeAsSeconds()
{
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration<float, std::chrono::seconds::period>(now - startTPoint).count();
}
