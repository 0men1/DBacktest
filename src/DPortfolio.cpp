#include "DPortfolio.h"
#include <iostream>

void DPortfolio::onSignal(std::shared_ptr<Signal> signal)
{
    switch (signal->side_)
    {
    case Side::BUY: {
        if (hasSufficientFunds(signal->price_, signal->quantity_))
        {
            int32_t orderQuantity = std::abs(signal->quantity_);
            std::shared_ptr<Order> order = std::make_shared<Order>(createOrder(
                Type::MARKET, Side::BUY, signal->instrument_id_, signal->price_, orderQuantity, signal->timestamp_));

            m_pEventBus->m_events.push(order);
        }
        break;
    }
    case Side::SELL: {
        int32_t orderQuantity = std::abs(signal->quantity_);
        std::shared_ptr<Order> order = std::make_shared<Order>(createOrder(
            Type::MARKET, Side::SELL, signal->instrument_id_, signal->price_, orderQuantity, signal->timestamp_));

        m_pEventBus->m_events.push(order);
        break;
    }
    }
}

void DPortfolio::onFill(std::shared_ptr<Fill> fill)
{
    double orderQuantity = std::abs(fill->quantity_);
    switch (fill->side_)
    {
    case Side::SELL:
        process_sell_order(fill->instrument_id_, fill->price_, orderQuantity);
        break;
    case Side::BUY:
        process_buy_order(fill->instrument_id_, fill->price_, orderQuantity);
        break;
    default:
        std::cout << "Filled quantity is 0" << std::endl;
        break;
    }

    m_results.ending_liquidity = m_fNetLiquidity;
}

Position &DPortfolio::getPosition(int32_t instrument_id)
{
    return m_positions[instrument_id];
}

bool DPortfolio::hasSufficientFunds(double price, double quantity)
{
    return m_fNetLiquidity > (quantity * price);
}

Order DPortfolio::createOrder(Type type, Side side, int32_t instrument_id, double price, double quantity,
                              uint64_t timestamp)
{
    return Order(m_orderId++, type, side, instrument_id, price, quantity, timestamp);
}

void DPortfolio::process_sell_order(int32_t instrument_id, double price, double quantity)
{
    if (quantity <= 0 || price <= 0 || instrument_id < 0)
    {
        throw std::invalid_argument("error: invalid order data");
    }

    Position &p = getPosition(instrument_id);

    if (p.quantity > 0 && p.quantity >= quantity)
    {
        p.quantity -= quantity;
        double realized_pnl = (price - p.avg_price) * quantity;
        p.realized_pnl += realized_pnl;

        m_fNetLiquidity += price * quantity;
        m_fRealizedPnl += realized_pnl;

        if (p.quantity == 0)
        {
            p.avg_price = 0;
        }
    }
    else
    {
        throw std::invalid_argument("error: invalid sell request");
    }
}

void DPortfolio::process_buy_order(int32_t instrument_id, double price, double quantity)
{
    if (quantity <= 0 || price <= 0 || instrument_id < 0)
    {
        throw std::invalid_argument("error: invalid order data");
    }

    Position &p = getPosition(instrument_id);

    double cost = price * quantity;                        // Cost of the order
    double total_cost = cost + (p.avg_price * p.quantity); // Total cost of entire position + order cost
    double new_quantity = p.quantity + quantity;
    double new_avg_price = total_cost / new_quantity;

    p.avg_price = new_avg_price;
    p.quantity = new_quantity;

    m_fNetLiquidity -= cost;
}

void DPortfolio::update_metrics(long timestamp, int32_t instrument_id, double close_price)
{
    m_last_prices[instrument_id] = close_price;
    double total_equity = m_fNetLiquidity;

    for (auto &[id, pos] : m_positions)
    {
        if (m_last_prices.find(id) != m_last_prices.end())
        {
            total_equity += (pos.quantity * m_last_prices[id]);
        }
    }
    m_results.record_snapshot(timestamp, total_equity);
}
