#pragma once

#ifndef _DMONITOR_H_
#define _DMONITOR_H_

#include <chrono>
#include <unordered_map>

struct Timer {
  std::chrono::time_point<std::chrono::high_resolution_clock>
      startTimePoint;      // 8
  double last_duration_ms; // 8
  double last_duration_us; // 8
  double avg_duration_ms;  // 8
  double avg_duration_us;  // 8
  int num_taken;           // 4
};

class DMonitor {
public:
  DMonitor();
  ~DMonitor() = default;

  void print_timers();

  void start(std::string name);
  void stop(std::string name);

private:
  std::unordered_map<std::string, Timer> m_Timers;
};

#endif // _DMONITOR_H
