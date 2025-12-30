#pragma once

#include <memory>
#ifndef _ISTRATEGY_H_
#define _ISTRATEGY_H_

struct BacktestContext;
class Candle;

class IStrategy {
public:
  virtual ~IStrategy() = default;
  virtual void onCandle(std::shared_ptr<Candle> candle) = 0;
  virtual void init(BacktestContext *context) { m_context = context; };

private:
  BacktestContext *m_context = nullptr;
};

#endif // _ISTRATEGY_H
