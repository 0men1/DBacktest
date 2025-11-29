#pragma once

#ifndef _DORDERBOOK_H
#define _DORDERBOOK_H

#include "DPortfolio.h"
#include "types/Candle.h"
#include "types/Order.h"
#include "types/Signal.h"
#include <map>

struct OrderbookSummary {
  PortfolioSummary portfolioSummary;
  int numLimitOrders;
  int numMarketOrders;
  int numStopOrders;
};

class DOrderbook {
public:
  OrderbookSummary summary() {
    m_Summary.portfolioSummary = m_Portfolio.summary();
    return m_Summary;
  }

public:
  DOrderbook(DPortfolio portfolio);
  ~DOrderbook() = default;

  void process_signal(Signal &signal);
  void process_candle(const Candle &candle);
  bool flatten(const std::string &symbol, double price);

private:
  DPortfolio m_Portfolio;
  OrderId m_orderId = 1;
  std::map<double, Order> m_sellOrders;
  std::map<double, Order> m_buyOrders;
  std::map<double, Order> m_stopOrders;

public: // getters & setters
  DPortfolio &portfolio() { return m_Portfolio; }

private:
  OrderbookSummary m_Summary;
};

#endif //_DORDERBOOK_H
