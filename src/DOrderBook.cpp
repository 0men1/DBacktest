#include "DOrderBook.h"
#include "types/Fill.h"

void DOrderBook::onOrder(std::shared_ptr<Order> order)
{
    switch (order->type_)
    {
    case Type::MARKET:
        break;

    case Type::LIMIT:
        if (order->side_ == Side::BUY)
        {
            m_bids[order->price_].push_back(order);
        }
        else
        {
            m_asks[order->price_].push_back(order);
        }
        break;

    case Type::STOP:
        break;
    }
}

void DOrderBook::onCandle(std::shared_ptr<Candle> candle)
{
    auto bids_end = m_bids.upper_bound(candle->low());
    for (auto it = m_bids.begin(); it != bids_end; ++it)
    {
        for (const auto &order : it->second)
        {
            m_pEventBus->m_events.push(std::make_shared<Fill>(order->orderId_, order->type_, order->side_,
                                                              order->instrument_id_, order->price_, order->quantity_,
                                                              m_fCommission, candle->timestamp()));
        }
    }
    m_bids.erase(m_bids.begin(), bids_end);

    auto asks_end = m_asks.upper_bound(candle->high());
    for (auto it = m_asks.begin(); it != asks_end; ++it)
    {
        for (const auto &order : it->second)
        {
            m_pEventBus->m_events.push(std::make_shared<Fill>(order->orderId_, order->type_, order->side_,
                                                              order->instrument_id_, order->price_, order->quantity_,
                                                              m_fCommission, candle->timestamp()));
        }
    }
    m_asks.erase(m_asks.begin(), asks_end);
}
