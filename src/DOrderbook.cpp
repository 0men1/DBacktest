#include "DOrderbook.h"

DOrderbook::DOrderbook(DPortfolio portfolio)
    : m_Portfolio(std::move(portfolio)) {
  m_Summary.numLimitOrders = 0;
  m_Summary.numMarketOrders = 0;
  m_Summary.numStopOrders = 0;
}

bool DOrderbook::flatten(const std::string &symbol, double price) {
  if (m_Portfolio.close_position(symbol, price)) {
    m_buyOrders.clear();
    m_sellOrders.clear();
    m_stopOrders.clear();
    return true;
  } else {
    return false;
  }
}

/**
 * @brief Parse signal and create order with orderId and place into book
 *
 * @param signal
 */
void DOrderbook::process_signal(Signal &signal) {
  Order o = Order{m_orderId++,       signal.timestamp(), Order::Type::MARKET,
                  signal.quantity(), signal.price(),     signal.symbol()};

  switch (signal.type()) {
  case Signal::Type::LONG:
    m_Portfolio.process_buy_order(o);
    break;
  case Signal::Type::SHORT:
    m_Portfolio.process_sell_order(o);
    break;
  }
}

/**
 * @brief Find all orders that are triggered by the range of the candle and
 * execute them
 *
 * @param candle
 */
void DOrderbook::process_candle(const Candle &candle) {

  // HANDLE LIMIT SELL ORDERS
  auto itSell = m_sellOrders.lower_bound(candle.low);
  for (auto it = m_sellOrders.begin(); it != itSell;) {
    auto price = it->first;
    m_Portfolio.process_sell_order(m_sellOrders[price]);
    it = m_sellOrders.erase(it);
  }

  // HANDLE LIMIT BUY ORDERS
  auto itBuy = m_buyOrders.lower_bound(candle.high);
  for (auto it = m_buyOrders.begin(); it != itBuy;) {
    auto price = it->first;
    m_Portfolio.process_buy_order(m_buyOrders[price]);
    it = m_buyOrders.erase(it);
  }

  // HANDLE STOP ORDERS
  auto itStopLow = m_stopOrders.lower_bound(candle.low);
  auto itStopHigh = m_stopOrders.upper_bound(candle.high);
  for (auto it = itStopLow; it != itStopHigh;) {
    auto price = it->first;
    m_Portfolio.process_stop_order(m_stopOrders[price]);
    it = m_stopOrders.erase(it);
  }
}
