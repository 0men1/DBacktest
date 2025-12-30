#pragma once

#include "Event.h"
#ifndef _ORDER_H_
#define _ORDER_H_

typedef long OrderId;

class Order : public Event
{
  public:
    enum class Type
    {
        MARKET,
        LIMIT,
        STOP,
    };

    Order(OrderId orderId, Type type, double quantity, double price, int32_t instrument_id)
        : Event(ORDER), orderId_(orderId), type_(type), quantity_(quantity), price_(price),
          m_instrument_id(instrument_id)
    {
    }

    OrderId orderId()
    {
        return orderId_;
    }
    Type type()
    {
        return type_;
    }
    double &quantity()
    {
        return quantity_;
    }
    double price()
    {
        return price_;
    }
    int32_t instrument_id()
    {
        return m_instrument_id;
    }

  private:
    OrderId orderId_;
    Type type_;
    double quantity_;
    double price_;
    int32_t m_instrument_id;
};

#endif
