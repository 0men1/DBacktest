#pragma once

#include "types/Event.h"
#include <memory>
#include <queue>

class DReader;
class IStrategy;
class DOrderbook;
class DMonitor;

struct BacktestContext {
  BacktestContext(std::unique_ptr<DReader> reader,
                  std::unique_ptr<IStrategy> strategy);
  ~BacktestContext();

  std::priority_queue<std::shared_ptr<Event>,
                      std::vector<std::shared_ptr<Event>>, EventComparator>
      m_events;

  std::unique_ptr<DReader> m_reader;
  std::unique_ptr<IStrategy> m_pStrategy;
  std::unique_ptr<DMonitor> m_monitor;
  std::unique_ptr<DOrderbook> m_orderBook;
};
