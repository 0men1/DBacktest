#pragma once

#ifndef _SIGNAL_H
#define _SIGNAL_H

#include <string>

class Signal {
public:
  enum class Type { SHORT, LONG };
  Signal(Type type, uint64_t timestamp, double quantity, double price,
         std::string symbol)
      : type_(type), timestamp_(timestamp), symbol_(symbol),
        quantity_(quantity), price_(price) {}

  uint64_t timestamp() { return timestamp_; }
  Type type() { return type_; }
  double &quantity() { return quantity_; }
  double price() { return price_; }
  std::string symbol() { return symbol_; }

private:
  Type type_;
  uint64_t timestamp_;
  std::string symbol_;
  double quantity_;
  double price_;
};

#endif //_SIGNAL_H
