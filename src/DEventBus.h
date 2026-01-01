#pragma once

#ifndef _DEVENTBUS_H_
#define _DEVENTBUS_H_

#include "types/Event.h"
#include <memory>
#include <queue>

struct DEventBus
{
    DEventBus() = default;
    ~DEventBus() = default;

    std::priority_queue<std::shared_ptr<Event>, std::vector<std::shared_ptr<Event>>, EventComparator> m_events;
};

#endif // _DEVENTBUS_H_
