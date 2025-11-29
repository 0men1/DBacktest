#pragma once

#ifndef _ISTRATEGY_H_
#define _ISTRATEGY_H_

#include "types/Candle.h"
#include "types/Position.h"
#include "types/Signal.h"

class IStrategy {
public:
  virtual ~IStrategy() = default;
  virtual std::vector<Signal> process(const Candle &candle,
                                      const Position &p) = 0;
  virtual const std::string &name() = 0;
  virtual const std::string &symbol() = 0;

private:
};

#endif // _ISTRATEGY_H
