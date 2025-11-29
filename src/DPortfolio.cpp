#include "DPortfolio.h"

DPortfolio::DPortfolio(int net_liquidity, int commission = 1)
    : m_fNetLiquidity(net_liquidity), m_fCommission(commission) {
  m_Summary.starting_liquidty = net_liquidity;
}

const Position &DPortfolio::get_position(const std::string &symbol) {
  return m_Positions[symbol];
}

PortfolioSummary DPortfolio::summary() {
  m_Summary.realized_pnl = m_fRealizedPnl;
  m_Summary.commission = m_fCommission;
  m_Summary.ending_liquidity = m_fNetLiquidity;
  return m_Summary;
}

bool DPortfolio::close_position(const std::string &symbol, double price) {
  Position &p = m_Positions[symbol];
  if (p.quantity <= 0) {
    return false;
  }
  Order o = Order{-1, 1, Order::Type::MARKET, p.quantity, price, symbol};

  if (p.quantity > 0) {
    process_sell_order(o);
  } else {
    process_buy_order(o);
  }

  return true;
}

void DPortfolio::process_sell_order(Order &o) {
  if (o.quantity() <= 0 || o.price() <= 0 || o.symbol().empty()) {
    throw std::invalid_argument("error: invalid order data");
  }

  Position &p = m_Positions[o.symbol()];

  if (p.quantity > 0 && p.quantity >= o.quantity()) {
    p.quantity -= o.quantity();
    int realized_pnl = (o.price() - p.avg_price) * o.quantity();
    p.realized_pnl += realized_pnl;

    m_fNetLiquidity +=
        o.price() * o.quantity() -
        (m_fCommission > 0 ? (m_fCommission * o.quantity())
                           : 0); // If commision is not 0 then include it

    m_fRealizedPnl += realized_pnl;
    if (p.quantity == 0) {
      p.avg_price = 0;
      // m_Positions.erase(o.symbol());
    }
  } else {
    throw std::invalid_argument("error: invalid sell request");
  }
  m_Summary.num_trades++;
  m_Summary.num_sells++;
}

void DPortfolio::process_buy_order(Order &o) {
  if (o.quantity() <= 0 || o.price() <= 0 || o.symbol().empty()) {
    throw std::invalid_argument("error: invalid order data");
  }

  Position &p = m_Positions[o.symbol()];

  float cost = o.price() * o.quantity(); // Cost of the order
  float cost_comission =
      cost + (m_fCommission > 0 ? (m_fCommission * o.quantity()) : 0);

  if (m_fNetLiquidity < cost_comission) {
    throw std::invalid_argument("error: no funds left");
  }
  // If position quantity > 0 -> update the average price, increase quantity,
  // decrease net liquid
  if (p.quantity > 0) {
    float total_cost =
        cost + (p.avg_price *
                p.quantity); // Total cost of entire position + order cost
    float new_quantity = p.quantity + o.quantity();
    float new_avg_price = total_cost / new_quantity;

    p.avg_price = new_avg_price;
    p.quantity = new_quantity;
  }
  // If no position (e.g. quantity = 0) Assign average price, quantity, and
  // decrease from net liquid
  else if (p.quantity == 0) {
    p.avg_price = o.price();
    p.quantity = o.quantity();
    m_Summary.num_positions++;
  } else {
    throw std::invalid_argument("error: invalid buy request");
  }
  m_fNetLiquidity -= cost_comission; // Subtract cost + commission&fees amount
  m_Summary.num_trades++;
  m_Summary.num_buys++;
}

// TODO
void DPortfolio::process_stop_order(Order &o) {
  if (o.quantity() <= 0 || o.price() <= 0 || o.symbol().empty()) {
    throw std::invalid_argument("error: invalid order data");
  }

  Position &p = m_Positions[o.symbol()];
}
