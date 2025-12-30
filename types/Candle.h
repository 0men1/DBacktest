#pragma once

#include "Event.h"
#include <cstdint>
#include <iostream>

#pragma pack(push, 1)
struct CandleData {
  uint64_t timestamp;           // 8 bytes
  float open, high, low, close; // 16 bytes
  double volume;                // 8 bytes
                                // = 32 bytes
};
#pragma pack(pop)

class Candle : public Event {

public:
  Candle(CandleData data) : Event(CANDLE), m_data(data) {}

  float open() { return m_data.open; }
  float high() { return m_data.high; }
  float low() { return m_data.low; }
  float close() { return m_data.close; }
  double volume() { return m_data.volume; }
  uint64_t timestamp() { return m_data.timestamp; }

private:
  CandleData m_data;
};

inline std::ostream &operator<<(std::ostream &os, const CandleData &c) {
  os << c.timestamp << ": " << "o: " << c.open << " h: " << c.high
     << " l: " << c.low << " c: " << c.close << " v: " << c.volume << std::endl;
  return os;
}
