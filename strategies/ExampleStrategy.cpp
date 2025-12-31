#include "ExampleStrategy.h"
#include "types/Signal.h"
#include <ctime>
#include <memory>

void ExampleStrategy::onCandle(std::shared_ptr<Candle> candle)
{
    int randomNumber = (std::rand() % 5) + 1;

    if (randomNumber == 3)
    {
        m_pEventBus->m_events.push(std::make_shared<Signal>(Type::MARKET, Side::BUY, candle->instrument_id(),
                                                            candle->close(), 3.0, candle->timestamp()));
    }
}
