#pragma once

#ifndef _DORDERBOOK_H_
#define _DORDERBOOK_H_

#include "DEventBus.h"
#include "types/Candle.h"
#include "types/Order.h"
#include <memory>

class DOrderBook
{

  public:
    DOrderBook()
    {
    }
    ~DOrderBook() = default;

    void onOrder(std::shared_ptr<Order> order);
    void onCandle(std::shared_ptr<Candle> candle);

    void init(DEventBus *pEventBus)
    {
        m_pEventBus = pEventBus;
    }

  private:
    DEventBus *m_pEventBus;
};
#endif //_DORDERBOOK_H_
