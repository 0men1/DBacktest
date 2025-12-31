#include "ExampleStrategy.h"
#include "types/Signal.h"
#include <ctime>
#include <memory>

void ExampleStrategy::onCandle(std::shared_ptr<Candle> candle)
{
    int randomNumber = (std::rand() % 5) + 1;

    if (randomNumber == 1)
    {
        m_pEventBus->m_events.push(
            std::make_shared<Signal>(Type::MARKET, Side::BUY, 0, 3.0, candle->close(), candle->timestamp()));
    }
    else if (randomNumber == 2)
    {
        m_pEventBus->m_events.push(
            std::make_shared<Signal>(Type::MARKET, Side::SELL, 0, 3.0, candle->close(), candle->timestamp()));
    }
}
