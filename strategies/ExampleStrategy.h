#pragma once

#ifndef _EXAMPLE_STRATEGY_H
#define _EXAMPLE_STRATEGY_H

#include "src/DEventBus.h"
#include "src/IStrategy.h"
#include "types/Candle.h"

class ExampleStrategy : public IStrategy
{
    virtual void onCandle(std::shared_ptr<Candle> candle) override;

    virtual void init(std::shared_ptr<DEventBus> eventBus) override
    {
        m_pEventBus = eventBus;
    }

  private:
    std::shared_ptr<DEventBus> m_pEventBus;
};

#endif
