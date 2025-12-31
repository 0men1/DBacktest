#pragma once

#include "Event.h"
#ifndef _FILL_H_
#define _FILL_H_

#include "Order.h"
#include <cstdint>

struct Fill : public Event
{
    Fill(OrderId order_id, Type type, Side side, uint32_t instrument_id, float price, float quantity, float fee,
         uint64_t timestamp)
        : Event(FILL, timestamp), order_id_(order_id), type_(type), side_(side), instrument_id_(instrument_id),
          price_(price), quantity_(quantity), fee_(fee), timestamp_(timestamp)
    {
    }

    OrderId order_id_;
    Type type_;
    Side side_;
    uint32_t instrument_id_;
    float price_;
    float quantity_;
    float fee_;
    uint64_t timestamp_;
};

#endif // _FILL_H_
