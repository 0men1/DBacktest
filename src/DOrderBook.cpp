#include "DOrderBook.h"

void DOrderBook::onOrder(std::shared_ptr<Order> order)
{
    // Fill the order
}

void DOrderBook::onCandle(std::shared_ptr<Candle> candle)
{
    // Fil any limit/stop orders
}
