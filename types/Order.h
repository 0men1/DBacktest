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

    Order(OrderId orderId, Type type, int32_t instrument_id, float price, float quantity, uint64_t timestamp)
        : Event(ORDER, timestamp), orderId_(orderId), type_(type), quantity_(quantity), price_(price),
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
    float &quantity()
    {
        return quantity_;
    }
    float price()
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
    float quantity_;
    float price_;
    int32_t m_instrument_id;
};

#endif
