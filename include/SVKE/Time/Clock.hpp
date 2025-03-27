#pragma once

#include <chrono>

namespace vk
{
class Clock
{
  public:
    Clock();

    void restart();

    [[nodiscard]]
    const float getElapsedTimeAsSeconds();

  private:
    std::chrono::time_point<std::chrono::steady_clock> startTPoint;
};
} // namespace vk
