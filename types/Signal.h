#pragma once
#ifndef _SIGNAL_H
#define _SIGNAL_H

#include "Event.h"

class Signal : public Event
{
  public:
    enum class Type
    {
        SHORT,
        LONG
    };
    Signal(Type type, int32_t instrument_id, float quantity, double price,
           uint64_t timestamp)
        : Event(SIGNAL, timestamp), m_quantity(quantity), m_price(price), m_type(type), m_instrument_id(instrument_id)
    {
    }

    Type type()
    {
        return m_type;
    }
    float quantity()
    {
        return m_quantity;
    }
    float price()
    {
        return m_price;
    }
    int32_t instrument_id()
    {
        return m_instrument_id;
    }

  private:
    float m_quantity;
    float m_price;
    Type m_type;
    int32_t m_instrument_id;
};

#endif //_SIGNAL_H
