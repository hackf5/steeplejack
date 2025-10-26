#pragma once

#include <chrono>
#include <cmath>

namespace steeplejack
{
template <uint16_t Window> class RollingAverage
{
  private:
    uint16_t m_index = 0;
    uint16_t m_count = 0;
    uint32_t m_previous_inputs[Window];
    uint64_t m_sum = 0;

  public:
    uint32_t operator()(uint32_t input, double factor = 1.0)
    {
        m_sum -= m_previous_inputs[m_index];
        m_sum += input;
        m_previous_inputs[m_index] = input;
        if (++m_index == Window)
            m_index = 0;

        return static_cast<uint32_t>(std::lround((factor * m_sum) / (m_count < Window ? ++m_count : Window)));
    }
};

template <uint16_t Window = 10, uint32_t IntervalMS = 100> class Framerate
{
  private:
    RollingAverage<Window> m_rolling_average;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_previous_time;
    uint32_t m_previous_frame_count = 0;
    uint32_t m_frame_count = 0;
    uint32_t m_fps = 0;

  public:
    void next_frame()
    {
        m_frame_count++;
        auto current_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - m_previous_time);
        if (duration.count() >= IntervalMS)
        {
            m_fps = m_rolling_average(m_frame_count - m_previous_frame_count, 1000.0 / IntervalMS);
            m_previous_frame_count = m_frame_count;
            m_previous_time = current_time;
        }
    }

    uint32_t fps() const
    {
        return m_fps;
    }
};
} // namespace steeplejack