#pragma once

#ifndef _DCONTEXT_H_
#define _DCONTEXT_H_

#include "DMonitor.h"
#include "DReader.h"
#include "IStrategy.h"
#include "types/Event.h"
#include <memory>
#include <queue>

struct DContext
{
    DContext(std::unique_ptr<DReader> reader, std::unique_ptr<IStrategy> strategy);
    ~DContext() = default;

    std::priority_queue<std::shared_ptr<Event>, std::vector<std::shared_ptr<Event>>, EventComparator> m_events;

    std::unique_ptr<DReader> m_reader;
    std::unique_ptr<IStrategy> m_pStrategy;
    std::unique_ptr<DMonitor> m_monitor;
};

#endif // _DCONTEXT_H_
