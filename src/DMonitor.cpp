#include "DMonitor.h"

DMonitor::DMonitor()
{
}

void DMonitor::stop(std::string name)
{
    if (m_Timers.find(name) == m_Timers.end())
    {
        return;
    }

    Timer &timer = m_Timers[name];

    auto endTimePoint = std::chrono::high_resolution_clock::now();

    auto start = std::chrono::time_point_cast<std::chrono::microseconds>(timer.startTimePoint).time_since_epoch();
    auto end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimePoint).time_since_epoch();

    auto duration = end - start;
    double ms = duration.count() * 0.001;

    int old_sum = (timer.last_duration_ms * timer.num_taken);

    timer.num_taken++;

    timer.last_duration_ms = ms;
    timer.last_duration_us = duration.count();

    timer.avg_duration_ms = (old_sum + timer.last_duration_ms) / timer.num_taken;
    timer.avg_duration_us = (old_sum + timer.last_duration_us) / timer.num_taken;
}

void DMonitor::start(std::string name)
{
    m_Timers[name].startTimePoint = std::chrono::high_resolution_clock().now();
}

void DMonitor::print_timers()
{
    for (const auto &[name, timer] : m_Timers)
    {
        printf("(average) %s: %f (ms) %f (us)\n", name.c_str(), timer.avg_duration_ms, timer.avg_duration_us);
        printf("(last) %s: %f (ms) %f (us)\n", name.c_str(), timer.last_duration_ms, timer.last_duration_us);
    }
}
