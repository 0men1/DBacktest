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

struct OrderBookResults
{
    int total_orders = 0;
    int total_fills = 0;
    double volume_traded = 0.0;

    void print_results() const
    {
        std::cout << "\n--- EXECUTION RESULTS ---\n";
        std::cout << "Orders: " << total_orders << "\n";
        std::cout << "Fills:  " << total_fills << "\n";
        std::cout << "Volume: " << volume_traded << "\n";
    }
};

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

    const OrderBookResults &getResults() const
    {
        return m_results;
    }

  protected:
    void record_fill_event(double price, double quantity)
    {
        m_results.total_fills++;
        m_results.volume_traded += (quantity * price);
    }

  private:
    double m_fCommission{10};
    std::shared_ptr<DEventBus> m_pEventBus;
    std::map<double, std::deque<std::shared_ptr<Order>>, std::greater<double>> m_bids;
    std::map<double, std::deque<std::shared_ptr<Order>>, std::less<double>> m_asks;

    OrderBookResults m_results;
};

#endif //_DORDERBOOK_H_
