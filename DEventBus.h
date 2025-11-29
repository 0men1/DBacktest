#pragma once

#ifndef _EVENT_BUS_H_
#define _EVENT_BUS_H_

#include <queue>
#include <memory>
#include "DEvent.h"

class DEventBus
{
public:
    void enqueue_event(std::unique_ptr<Event> event)
    {
        m_qEventQueue.push(std::move(event));
    }

    std::unique_ptr<Event> dequeue_event()
    {
        if (m_qEventQueue.empty())
        {
            return nullptr;
        }
        std::unique_ptr<Event> top_event = std::move(const_cast<std::unique_ptr<Event> &>(m_qEventQueue.top()));
        m_qEventQueue.pop();
        return top_event;
    }

    bool empty()
    {
        return m_qEventQueue.empty();
    }
    // std::priority_queue<std::unique_ptr<Event>> queue()
    // {
    //     return m_qEventQueue;
    // }

private:
    std::priority_queue<std::unique_ptr<Event>> m_qEventQueue;
};

#endif //_EVENT_BUS_H_
