#include "SVKE/Time/Clock.hpp"

vk::Clock::Clock()
{
    restart();
}

const float vk::Clock::restart()
{
    const float elapsed = getElapsedTimeAsSeconds();
    startTPoint = std::chrono::steady_clock::now();

    return elapsed;
}

const float vk::Clock::getElapsedTimeAsSeconds()
{
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration<float, std::chrono::seconds::period>(now - startTPoint).count();
}
