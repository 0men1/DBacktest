#pragma once

#ifndef _DPORTFOLIO_H_
#define _DPORTFOLIO_H_

#include "DEventBus.h"
#include "types/Fill.h"
#include "types/Order.h"
#include "types/Position.h"
#include "types/Signal.h"
#include <iostream>
#include <memory>
#include <unordered_map>

struct PortfolioResults
{
    struct DataPoint
    {
        long timestamp;
        double equity;
    };

    std::vector<DataPoint> equity_curve;
    double high_water_mark = -999999.0;
    double max_drawdown = 0.0;
    double starting_liquidity = 0.0;
    double ending_liquidity = 0.0;

    void record_snapshot(long timestamp, double current_equity)
    {
        equity_curve.push_back({timestamp, current_equity});
        if (current_equity > high_water_mark)
        {
            high_water_mark = current_equity;
        }

        if (high_water_mark > 0)
        {
            double dd = (high_water_mark - current_equity) / high_water_mark;
            if (dd > max_drawdown)
                max_drawdown = dd;
        }
    }

    void print_results() const
    {
        double final_eq = equity_curve.empty() ? starting_liquidity : equity_curve.back().equity;
        double total_return =
            (starting_liquidity != 0) ? ((final_eq - starting_liquidity) / starting_liquidity) * 100.0 : 0.0;

        std::cout << "\n--- PORTFOLIO RESULTS ---\n";
        std::cout << "Starting Liquidity: " << starting_liquidity << "\n";
        std::cout << "Ending Liquidity:   " << ending_liquidity << "\n";
        std::cout << "Total Return:       " << total_return << "%\n";
        std::cout << "Max Drawdown:       " << (max_drawdown * 100.0) << "%\n";
    }
};

class DPortfolio
{

  public:
    DPortfolio(double net_liquidity) : m_fNetLiquidity(net_liquidity)
    {
        m_results.starting_liquidity = m_fNetLiquidity;
        m_results.high_water_mark = m_fNetLiquidity;
    }
    ~DPortfolio() = default;

    void onSignal(std::shared_ptr<Signal> signal);
    void onFill(std::shared_ptr<Fill> order);

    double getNetLiquidity()
    {
        return m_fNetLiquidity;
    }
    double getRealizedPnl()
    {
        return m_fRealizedPnl;
    }

    void init(std::shared_ptr<DEventBus> eventBus)
    {
        m_pEventBus = eventBus;
    }

    Position &getPosition(int32_t instrument_id);

    const PortfolioResults &getResults() const
    {
        return m_results;
    }

    void update_metrics(long timestamp, int32_t instrument_id, double close_price);

  private:
    bool hasSufficientFunds(double price, double quantity);

    void process_sell_order(int32_t instrument_id, double price, double quantity);
    void process_buy_order(int32_t instrument_id, double price, double quantity);

    Order createOrder(Type type, Side side, int32_t instrument_id, double price, double quantity, uint64_t timestamp);

    double m_fNetLiquidity;
    double m_fRealizedPnl{0};
    OrderId m_orderId{0};

    std::unordered_map<int32_t, Position> m_positions;
    std::shared_ptr<DEventBus> m_pEventBus;

    PortfolioResults m_results;
    std::unordered_map<int32_t, double> m_last_prices;
};

#endif //_DPORTFOLIO_H
