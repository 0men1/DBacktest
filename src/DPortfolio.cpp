#include "DPortfolio.h"

void DPortfolio::onSignal(std::shared_ptr<Signal> signal)
{
    switch (signal->type())
    {
    case Signal::Type::LONG: {
        if (hasSufficientFunds(signal->price(), signal->quantity()))
        {
            int32_t orderQuantity = std::abs(signal->quantity());
            std::shared_ptr<Order> order = std::make_shared<Order>(createOrder(
                Signal::Type::SHORT, signal->instrument_id(), signal->price(), orderQuantity, signal->timestamp()));

            m_pEventBus->m_events.push(order);
        }
        break;
    }
    case Signal::Type::SHORT: {
        int32_t orderQuantity = -1 * std::abs(signal->quantity());
        std::shared_ptr<Order> order = std::make_shared<Order>(createOrder(
            Signal::Type::SHORT, signal->instrument_id(), signal->price(), orderQuantity, signal->timestamp()));

        m_pEventBus->m_events.push(order);
        break;
    }
    }
}

Position &DPortfolio::getPosition(int instrument_id)
{
    return m_positions[instrument_id];
}

bool DPortfolio::hasSufficientFunds(float price, float quantity)
{
    return m_fNetLiquidity < quantity * price;
}

Order DPortfolio::createOrder(Signal::Type type, int32_t instrument_id, float price, float quantity, uint64_t timestamp)
{
    return Order(m_orderId, Order::Type::MARKET, instrument_id, quantity, price, timestamp);
}

/*
void DPortfolio::process_sell_order(Order &o)
{
    if (o.quantity() <= 0 || o.price() <= 0 || o.instrument_id())
    {
        throw std::invalid_argument("error: invalid order data");
    }

    Position &p = m_Positions[o.instrument_id()];

    if (p.quantity > 0 && p.quantity >= o.quantity())
    {
        p.quantity -= o.quantity();
        int realized_pnl = (o.price() - p.avg_price) * o.quantity();
        p.realized_pnl += realized_pnl;

        m_fNetLiquidity += o.price() * o.quantity() - (m_fCommission > 0 ? (m_fCommission * o.quantity())
                                                                         : 0); // If commision is not 0 then include it

        m_fRealizedPnl += realized_pnl;
        if (p.quantity == 0)
        {
            p.avg_price = 0;
            // m_Positions.erase(o.symbol());
        }
    }
    else
    {
        throw std::invalid_argument("error: invalid sell request");
    }
    m_Summary.num_trades++;
    m_Summary.num_sells++;
}

void DPortfolio::process_buy_order(Order &o)
{
    if (o.quantity() <= 0 || o.price() <= 0 || o.instrument_id())
    {
        throw std::invalid_argument("error: invalid order data");
    }

    Position &p = m_Positions[o.instrument_id()];

    float cost = o.price() * o.quantity(); // Cost of the order
    float cost_comission = cost + (m_fCommission > 0 ? (m_fCommission * o.quantity()) : 0);

    if (m_fNetLiquidity < cost_comission)
    {
        throw std::invalid_argument("error: no funds left");
    }
    // If position quantity > 0 -> update the average price, increase quantity,
    // decrease net liquid
    if (p.quantity > 0)
    {
        float total_cost = cost + (p.avg_price * p.quantity); // Total cost of entire position + order cost
        float new_quantity = p.quantity + o.quantity();
        float new_avg_price = total_cost / new_quantity;

        p.avg_price = new_avg_price;
        p.quantity = new_quantity;
    }
    // If no position (e.g. quantity = 0) Assign average price, quantity, and
    // decrease from net liquid
    else if (p.quantity == 0)
    {
        p.avg_price = o.price();
        p.quantity = o.quantity();
        m_Summary.num_positions++;
    }
    else
    {
        throw std::invalid_argument("error: invalid buy request");
    }
    m_fNetLiquidity -= cost_comission; // Subtract cost + commission&fees amount
    m_Summary.num_trades++;
    m_Summary.num_buys++;
}
 * */
