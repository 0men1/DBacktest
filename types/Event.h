#pragma once
#include <memory>
#ifndef EVENT_H
#define EVENT_H

#include <chrono>

enum EventType {
  CANDLE,
  ORDER,
  SIGNAL,
};

class Event {
public:
  Event(EventType type, std::chrono::system_clock::time_point timestamp =
                            std::chrono::system_clock::now())
      : m_type(type), m_timestamp(timestamp) {}
  virtual ~Event() = default;

  EventType type() const { return m_type; }
  auto timestamp() const { return m_timestamp; }

private:
  EventType m_type;
  std::chrono::time_point<std::chrono::system_clock> m_timestamp;
};

#endif // EVENT_H

struct EventComparator {
  bool operator()(const std::shared_ptr<Event> &lhs,
                  const std::shared_ptr<Event> &rhs) const {
    return lhs->timestamp() > rhs->timestamp();
  }
};
