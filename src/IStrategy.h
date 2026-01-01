#pragma once

#ifndef _ISTRATEGY_H_
#define _ISTRATEGY_H_

#include <memory>

struct DEventBus;
class Candle;

class IStrategy
{
  public:
    virtual ~IStrategy() = default;
    virtual void onCandle(std::shared_ptr<Candle> candle) = 0;

    virtual void init(std::shared_ptr<DEventBus> eventBus)
    {
        m_pEventBus = eventBus;
    };

  protected:
    std::shared_ptr<DEventBus> m_pEventBus = nullptr;
};

#endif // _ISTRATEGY_H
