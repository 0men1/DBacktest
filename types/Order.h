#pragma once

#include "Event.h"
#ifndef _ORDER_H_
#define _ORDER_H_

typedef long OrderId;

enum class Type
{
    MARKET,
    LIMIT,
    STOP
};
enum class Side
{
    BUY,
    SELL
};

struct Order : public Event
{
    Order(OrderId orderId, Type type, Side side, int32_t instrument_id, float price, float quantity, uint64_t timestamp)
        : Event(ORDER, timestamp), orderId_(orderId), type_(type), side_(side), instrument_id_(instrument_id),
          price_(price), quantity_(quantity), timestamp_(timestamp)
    {
    }

    OrderId orderId_;
    Type type_;
    Side side_;
    int32_t instrument_id_;
    float price_;
    float quantity_;
    uint64_t timestamp_;
};

#endif
