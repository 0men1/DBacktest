#include "src/DEventBus.h"
#include "src/DOrderBook.h"
#include "types/Candle.h"
#include "types/Fill.h"
#include "types/Order.h"
#include <gtest/gtest.h>
#include <memory>

class OrderBookTest : public ::testing::Test
{
  protected:
    std::unique_ptr<DOrderBook> book;
    std::shared_ptr<DEventBus> bus;

    void SetUp() override
    {
        bus = std::make_shared<DEventBus>();
        book = std::make_unique<DOrderBook>(10);
        book->init(bus);
        while (!bus->m_events.empty())
            bus->m_events.pop();
    }

    std::shared_ptr<Order> makeLimit(OrderId id, Side side, float price, float qty = 1.0f)
    {
        return std::make_shared<Order>(id, Type::LIMIT, side, 1, price, qty, 1000);
    }

    std::shared_ptr<Candle> makeCandle(float low, float high, uint64_t ts = 2000)
    {
        CandleData data{};
        data.timestamp = ts;
        data.low = low;
        data.high = high;
        data.open = (low + high) / 2.0f;
        data.close = (low + high) / 2.0f;
        data.volume = 100.0;

        return std::make_shared<Candle>(data);
    }

    std::shared_ptr<Fill> popFill()
    {
        if (bus->m_events.empty())
            return nullptr;

        auto e = bus->m_events.top();
        bus->m_events.pop();

        return std::static_pointer_cast<Fill>(e);
    }
};

TEST_F(OrderBookTest, BuyLimitFill)
{
    book->onOrder(makeLimit(1, Side::BUY, 100.0f));

    book->onCandle(makeCandle(99.0f, 105.0f));

    ASSERT_FALSE(bus->m_events.empty()) << "Buy order should have filled";
    ASSERT_EQ(bus->m_events.top()->type(), FILL);

    auto fill = popFill();
    EXPECT_EQ(fill->order_id_, 1);
    EXPECT_FLOAT_EQ(fill->price_, 100.0f);
    EXPECT_EQ(fill->side_, Side::BUY);
}

TEST_F(OrderBookTest, ExactBoundaryMatch)
{
    book->onOrder(makeLimit(10, Side::BUY, 100.0f));
    book->onCandle(makeCandle(100.0f, 105.0f));

    auto fillBuy = popFill();
    ASSERT_TRUE(fillBuy != nullptr) << "Buy at 100.0 should fill when Low is 100.0";
    EXPECT_EQ(fillBuy->order_id_, 10);
    EXPECT_EQ(fillBuy->timestamp_, 2000);
}

TEST_F(OrderBookTest, NoFillLogic)
{
    book->onOrder(makeLimit(1, Side::BUY, 100.0f));
    book->onCandle(makeCandle(100.01f, 105.0f));

    ASSERT_TRUE(bus->m_events.empty()) << "Order should NOT fill";
}

TEST_F(OrderBookTest, TimestampCheck)
{
    uint64_t order_ts = 1000;
    uint64_t candle_ts = 5000;

    auto order = std::make_shared<Order>(99, Type::LIMIT, Side::BUY, 1, 100.0f, 1.0f, order_ts);
    book->onOrder(order);

    book->onCandle(makeCandle(90.0f, 110.0f, candle_ts));

    auto fill = popFill();
    ASSERT_TRUE(fill != nullptr);
    EXPECT_EQ(fill->timestamp_, candle_ts);
    EXPECT_NE(fill->timestamp_, order_ts);
}
