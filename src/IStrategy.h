#pragma once

#ifndef _ISTRATEGY_H_
#define _ISTRATEGY_H_

#include <memory>

struct DContext;
class Candle;

class IStrategy
{
  public:
    virtual ~IStrategy() = default;
    virtual void onCandle(std::shared_ptr<Candle> candle) = 0;

    virtual void init(DContext *context)
    {
        m_context = context;
    };

  private:
    DContext *m_context = nullptr;
};

#endif // _ISTRATEGY_H
