#pragma once

#ifndef _DPORTFOLIO_H_
#define _DPORTFOLIO_H_

#include "DEventBus.h"
#include "types/Fill.h"
#include "types/Order.h"
#include "types/Position.h"
#include "types/Signal.h"
#include <memory>
#include <unordered_map>

class DPortfolio
{

  public:
    DPortfolio(int net_liquidity) : m_fNetLiquidity(net_liquidity)
    {
    }
    ~DPortfolio() = default;

    void onSignal(std::shared_ptr<Signal> signal);
    void onFill(std::shared_ptr<Fill> order);

    float getNetLiquidity()
    {
        return m_fNetLiquidity;
    }
    float getRealizedPnl()
    {
        return m_fRealizedPnl;
    }

    void init(std::shared_ptr<DEventBus> eventBus)
    {
        m_pEventBus = eventBus;
    }

    Position &getPosition(int instrument_id);

  private:
    bool hasSufficientFunds(float price, float quantity);

    void process_sell_order(int32_t instrument_id, float price, float quantity);
    void process_buy_order(int32_t instrument_id, float price, float quantity);

    Order createOrder(Type type, Side side, int32_t instrument_id, float price, float quantity, uint64_t timestamp);

    float m_fNetLiquidity{10000};
    float m_fRealizedPnl{0};
    OrderId m_orderId{0};

    std::unordered_map<int32_t, Position> m_positions;
    std::shared_ptr<DEventBus> m_pEventBus;
};

#endif //_DPORTFOLIO_H
