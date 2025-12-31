#pragma once
#ifndef _SIGNAL_H
#define _SIGNAL_H

#include "Event.h"
#include "Order.h"

struct Signal : public Event
{
    Signal(Type type, Side side, int32_t instrument_id, double price, double quantity, uint64_t timestamp)
        : Event(SIGNAL, timestamp), type_(type), side_(side), instrument_id_(instrument_id), price_(price),
          quantity_(quantity), timestamp_(timestamp)
    {
    }

    Type type_;
    Side side_;
    int32_t instrument_id_;
    double price_;
    double quantity_;
    uint64_t timestamp_;
};

#endif //_SIGNAL_H
