#include "ExampleStrategy.h"
#include <iostream>

void ExampleStrategy::onCandle(std::shared_ptr<Candle> candle)
{
    std::cout << candle << std::endl;
}
