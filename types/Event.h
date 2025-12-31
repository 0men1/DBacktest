#pragma once
#ifndef EVENT_H
#define EVENT_H

#include <memory>

enum EventType
{
    CANDLE,
    ORDER,
    SIGNAL,
    FILL
};

class Event
{
  public:
    Event(EventType type, uint64_t timestamp) : m_type(type), m_timestamp(timestamp)
    {
    }
    virtual ~Event() = default;

    EventType type() const
    {
        return m_type;
    }
    auto timestamp() const
    {
        return m_timestamp;
    }

  private:
    EventType m_type;
    uint64_t m_timestamp;
};

#endif // EVENT_H

struct EventComparator
{
    bool operator()(const std::shared_ptr<Event> &lhs, const std::shared_ptr<Event> &rhs) const
    {
        if (lhs->timestamp() != rhs->timestamp())
        {
            return lhs->timestamp() < rhs->timestamp();
        }

        return getPriority(lhs->type()) > getPriority(rhs->type());
    }

    int getPriority(EventType type) const
    {
        switch (type)
        {
        case CANDLE:
            return 1;
        case ORDER:
            return 2;
        case SIGNAL:
            return 3;
        default:
            return 0;
        }
    }
};
