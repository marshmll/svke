#include "SVKE/Time/Clock.hpp"

vk::Clock::Clock()
{
    restart();
}

void vk::Clock::restart()
{
    startTPoint = std::chrono::steady_clock::now();
}

const float vk::Clock::getElapsedTimeAsSeconds()
{
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration<float, std::chrono::seconds::period>(now - startTPoint).count();
}
