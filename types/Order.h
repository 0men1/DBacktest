#pragma once

#ifndef _ORDER_H_
#define _ORDER_H_

#include <cstdint>
#include <string>

typedef long OrderId;

class Order {
public:
  enum class Type {
    MARKET,
    LIMIT,
    STOP,
  };

  Order() = default;

  Order(OrderId orderId, uint64_t timestamp, Type type, double quantity,
        double price, std::string symbol)
      : orderId_(orderId), timestamp_(timestamp), type_(type),
        quantity_(quantity), price_(price), symbol_(symbol) {}

  OrderId orderId() { return orderId_; }
  uint64_t timestamp() { return timestamp_; }
  Type type() { return type_; }
  double &quantity() { return quantity_; }
  double price() { return price_; }
  std::string symbol() { return symbol_; }

private:
  OrderId orderId_;
  uint64_t timestamp_;
  Type type_;
  double quantity_;
  double price_;
  std::string symbol_;
};

#endif
