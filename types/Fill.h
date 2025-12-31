#pragma once

#ifndef _FILL_H_
#define _FILL_H_

#include "Order.h"
#include <cstdint>

struct Fill
{
    Fill(OrderId order_id, uint64_t instrument_id, Order::Type type, float price, float quantity, float fee,
         uint64_t timestamp)
        : order_id(order_id), instrument_id(instrument_id), type(type), price(price), quantity(quantity), fee(fee),
          timestamp(timestamp)
    {
    }

    OrderId order_id;
    uint64_t instrument_id;
    Order::Type type;

    float price;
    float quantity;
    float fee;
    uint64_t timestamp;
};

#endif // _FILL_H_
