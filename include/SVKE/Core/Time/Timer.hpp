#pragma once

#include <chrono>

namespace vk
{
class Timer
{
  public:
    Timer();

    void restart();

    [[nodiscard]]
    const float getElapsedTimeAsSeconds();

  private:
    std::chrono::time_point<std::chrono::steady_clock> startTPoint;
};
} // namespace vk
