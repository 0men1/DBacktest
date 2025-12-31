#pragma once

#ifndef _DORDERBOOK_H_
#define _DORDERBOOK_H_

#include "DEventBus.h"
#include "types/Candle.h"
#include "types/Order.h"
#include <deque>
#include <functional>
#include <map>
#include <memory>

class DOrderBook
{

  public:
    DOrderBook(int commission) : m_fCommission(commission)
    {
    }
    ~DOrderBook() = default;

    void onOrder(std::shared_ptr<Order> order);
    void onCandle(std::shared_ptr<Candle> candle);

    void init(std::shared_ptr<DEventBus> pEventBus)
    {
        m_pEventBus = pEventBus;
    }

  private:
    float m_fCommission{10};
    std::shared_ptr<DEventBus> m_pEventBus;
    std::map<float, std::deque<std::shared_ptr<Order>>, std::greater<float>> m_bids;
    std::map<float, std::deque<std::shared_ptr<Order>>, std::less<float>> m_asks;
};

#endif //_DORDERBOOK_H_
