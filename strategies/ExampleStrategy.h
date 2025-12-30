#ifndef _EXAMPLE_STRATEGY_H
#define _EXAMPLE_STRATEGY_H

#include <memory>
#include "types/Candle.h"

#include "src/IStrategy.h"
class ExampleStrategy : public IStrategy
{
    virtual void onCandle(std::shared_ptr<Candle> candle) override;
};

#endif
