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
        int32_t orderQuantity = -1 * std::abs(signal->quantity_);
        std::shared_ptr<Order> order = std::make_shared<Order>(createOrder(
            Type::MARKET, Side::SELL, signal->instrument_id_, signal->price_, orderQuantity, signal->timestamp_));

        m_pEventBus->m_events.push(order);
        break;
    }
    }
}

void DPortfolio::onFill(std::shared_ptr<Fill> fill)
{
    float filled_quantity = fill->quantity_;

    if (filled_quantity < 0) // short
    {
        process_sell_order(fill->instrument_id_, fill->price_, -1 * filled_quantity);
    }
    else if (filled_quantity > 0) // long
    {
        process_buy_order(fill->instrument_id_, fill->price_, filled_quantity);
    }
    else
    {
        std::cout << "Filled quantity is 0" << std::endl;
    }
}

Position &DPortfolio::getPosition(int instrument_id)
{
    return m_positions[instrument_id];
}

bool DPortfolio::hasSufficientFunds(float price, float quantity)
{
    return m_fNetLiquidity > (quantity * price);
}

Order DPortfolio::createOrder(Type type, Side side, int32_t instrument_id, float price, float quantity,
                              uint64_t timestamp)
{
    return Order(m_orderId, type, side, instrument_id, quantity, price, timestamp);
}

void DPortfolio::process_sell_order(int32_t instrument_id, float price, float quantity)
{
    if (quantity <= 0 || price <= 0 || instrument_id < 0)
    {
        throw std::invalid_argument("error: invalid order data");
    }

    Position &p = m_positions[instrument_id];

    if (p.quantity > 0 && p.quantity >= quantity)
    {
        p.quantity -= quantity;
        int realized_pnl = (price - p.avg_price) * quantity;
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

void DPortfolio::process_buy_order(int32_t instrument_id, float price, float quantity)
{
    if (quantity <= 0 || price <= 0 || instrument_id < 0)
    {
        throw std::invalid_argument("error: invalid order data");
    }

    Position &p = m_positions[instrument_id];

    float cost = price * quantity; // Cost of the order

    // If position quantity > 0 -> update the average price, increase quantity,
    // decrease net liquid
    if (p.quantity > 0)
    {
        float total_cost = cost + (p.avg_price * p.quantity); // Total cost of entire position + order cost
        float new_quantity = p.quantity + quantity;
        float new_avg_price = total_cost / new_quantity;

        p.avg_price = new_avg_price;
        p.quantity = new_quantity;

        m_fNetLiquidity -= cost;
    }
    // If no position (e.g. quantity = 0) Assign average price, quantity, and
    // decrease from net liquid
    else if (p.quantity == 0)
    {
        p.avg_price = price;
        p.quantity = quantity;

        m_fNetLiquidity -= cost;
    }
    else
    {
        throw std::invalid_argument("error: invalid buy request");
    }
}
