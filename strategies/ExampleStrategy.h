#pragma once

#ifndef _EXAMPLE_STRATEGY_H
#define _EXAMPLE_STRATEGY_H

#include "src/IStrategy.h"
#include "types/Candle.h"

class ExampleStrategy : public IStrategy
{
    virtual void onCandle(std::shared_ptr<Candle> candle) override;
};

#endif
