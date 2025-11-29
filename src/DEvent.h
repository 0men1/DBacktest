#pragma once

#ifndef _EVENT_H_
#define _EVENT_H_

#include "Candle.h"
#include "Order.h"
#include <string>

class MarketDataEvent;
class OrderEvent;
class SignalEvent;

class Event {
public:
  enum class Type { MARKETDATA, SIGNAL, ORDER };

  virtual ~Event() = default;

  virtual Type get_type() = 0;
};

class MarketDataEvent : Event {
public:
  MarketDataEvent(const Candle &candle, uint64_t timestamp)
      : candle_(candle), timestamp_(timestamp) {}

  Type get_type() override { return Type::MARKETDATA; }

  const Candle &get_candle() { return candle_; }
  const uint64_t get_timestamp() { return timestamp_; }

private:
  const Candle &candle_;
  uint64_t timestamp_;
};

class OrderEvent : Event {
public:
  enum class OrderType { MARKET, LIMIT, STOP };

  OrderEvent(OrderId orderId, uint64_t timestamp, double quantity, double price,
             OrderType order_type, std::string symbol)
      : orderId_(orderId), timestamp_(timestamp), quantity_(quantity),
        price_(price), order_type_(order_type), symbol_(symbol) {}

  Type get_type() override { return Type::ORDER; }
  OrderId get_orderId() { return orderId_; }
  uint64_t get_timestamp() { return timestamp_; }
  double get_quantity() { return quantity_; }
  double get_price() { return price_; }
  OrderType get_orderType() { return order_type_; }
  std::string get_symbol() { return symbol_; }

private:
  OrderId orderId_;
  uint64_t timestamp_;
  double quantity_;
  double price_;
  OrderType order_type_;
  std::string symbol_;
};

class SignalEvent : Event {
public:
  enum class SignalType { SHORT, LONG };

  SignalEvent(uint64_t timestamp, double quantity, double price,
              SignalType signal_type, std::string symbol)
      : timestamp_(timestamp), quantity_(quantity), price_(price),
        signal_type_(signal_type), symbol_(symbol) {}

  uint64_t get_timestamp() { return timestamp_; }
  double get_quantity() { return quantity_; }
  double get_price() { return price_; }
  SignalType get_signalType() { return signal_type_; }
  std::string get_symbol() { return symbol_; }

private:
  uint64_t timestamp_;
  double quantity_;
  double price_;
  SignalType signal_type_; // "SHORT" or "LONG"
  std::string symbol_;
};

#endif //_EVENT_H_
