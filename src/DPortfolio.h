#pragma once

#ifndef _DPORTFOLIO_H_
#define _DPORTFOLIO_H_

#include "types/Order.h"
#include "types/Position.h"
#include "types/Signal.h"
#include <memory>
#include <unordered_map>

struct PortfolioSummary
{
    int num_positions{0};
    int num_trades{0};
    int num_buys{0};
    int num_sells{0};
    int num_stopped{0};

    float realized_pnl{0.0};
    float commission{0.0};

    float starting_liquidty{0};
    float ending_liquidity{0};
};

class DPortfolio
{

  public:
    DPortfolio(int net_liquidity, int commission);
    ~DPortfolio() = default;

    const Position &get_position(int32_t instrument_id);
    bool close_position(int32_t &instrument_id, double price);
    PortfolioSummary summary();

    void onSignal(std::shared_ptr<Signal> signal);

  public:
    void process_sell_order(Order &o);
    void process_buy_order(Order &o);
    void process_stop_order(Order &o);

  private:
    float m_fNetLiquidity{10000};
    float m_fCommission;
    float m_fRealizedPnl{0};

    std::unordered_map<int32_t, Position> m_Positions;

  private:
    PortfolioSummary m_Summary;
};

#endif //_DPORTFOLIO_H
