#pragma once

#include <cstdint>
#include <iostream>
#include <memory>

#include "Event.h"

#pragma pack(push, 1)
struct CandleData
{
    uint64_t timestamp;           // 8 bytes
    float open, high, low, close; // 16 bytes
    double volume;                // 8 bytes
                                  // = 32 bytes
};
#pragma pack(pop)

class Candle : public Event
{
  public:
    Candle(CandleData data, int32_t instrument_id)
        : Event(CANDLE, data.timestamp), data_(data), instrument_id_(instrument_id)
    {
    }

    float open() const
    {
        return data_.open;
    }
    float high() const
    {
        return data_.high;
    }
    float low() const
    {
        return data_.low;
    }
    float close() const
    {
        return data_.close;
    }
    double volume() const
    {
        return data_.volume;
    }
    uint64_t timestamp() const
    {
        return data_.timestamp;
    }
    int32_t instrument_id() const
    {
        return instrument_id_;
    }

  private:
    CandleData data_;
    int32_t instrument_id_;
};

inline std::ostream &operator<<(std::ostream &os, std::shared_ptr<Candle> c)
{
    os << c->timestamp() << ": " << "o: " << c->open() << " h: " << c->high() << " l: " << c->low()
       << " c: " << c->close() << " v: " << c->volume() << std::endl;
    return os;
}
