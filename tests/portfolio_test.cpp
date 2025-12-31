#include "src/DEventBus.h"
#include "src/DPortfolio.h"
#include "types/Fill.h"
#include <gtest/gtest.h>
#include <memory>

class MockEventBus : public DEventBus
{
};

class PortfolioFillTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        portfolio = std::make_unique<DPortfolio>(10000);
        bus = std::make_shared<MockEventBus>();
        portfolio->init(bus);
    }

    std::unique_ptr<DPortfolio> portfolio;
    std::shared_ptr<MockEventBus> bus;
};

TEST_F(PortfolioFillTest, HandlesNewBuyFillCorrectly)
{
    int32_t instrument_id = 1;
    OrderId order_id = 1;
    float price = 100.0f;
    float qty = 10.0f;

    auto fill = std::make_shared<Fill>(order_id, Type::MARKET, Side::BUY, instrument_id, price, qty, 0, 0);
    portfolio->onFill(fill);

    Position &pos = portfolio->getPosition(instrument_id);
    EXPECT_EQ(pos.quantity, 10.0f);
    EXPECT_FLOAT_EQ(pos.avg_price, 100.0f);
}

TEST_F(PortfolioFillTest, CalculatesWeightedAveragePrice)
{
    int32_t instrument_id = 2;
    OrderId order_id = 2;

    portfolio->onFill(std::make_shared<Fill>(order_id, Type::MARKET, Side::BUY, instrument_id, 100.0f, 10.0f, 0, 0));
    portfolio->onFill(std::make_shared<Fill>(order_id, Type::MARKET, Side::BUY, instrument_id, 200.0f, 10.0f, 0, 0));

    Position &pos = portfolio->getPosition(instrument_id);
    EXPECT_EQ(pos.quantity, 20.0f);
    EXPECT_FLOAT_EQ(pos.avg_price, 150.0f);
}

TEST_F(PortfolioFillTest, HandlesPartialSellAndPnL)
{
    int32_t instrument_id = 3;
    OrderId order_id = 3;

    portfolio->onFill(std::make_shared<Fill>(order_id, Type::MARKET, Side::BUY, instrument_id, 100.0f, 10.0f, 0, 0));
    portfolio->onFill(std::make_shared<Fill>(order_id, Type::MARKET, Side::SELL, instrument_id, 120.0f, -5.0f, 0, 0));

    Position &pos = portfolio->getPosition(instrument_id);
    EXPECT_EQ(pos.quantity, 5.0f);
    EXPECT_FLOAT_EQ(pos.avg_price, 100.0f);
    EXPECT_FLOAT_EQ(pos.realized_pnl, 100.0f);
}

TEST_F(PortfolioFillTest, ClosesPositionCompletely)
{
    int32_t instrument_id = 4;
    OrderId order_id = 4;

    portfolio->onFill(std::make_shared<Fill>(order_id, Type::MARKET, Side::BUY, instrument_id, 50.0f, 5.0f, 0, 0));
    portfolio->onFill(std::make_shared<Fill>(order_id, Type::MARKET, Side::SELL, instrument_id, 50.0f, -5.0f, 0, 0));

    Position &pos = portfolio->getPosition(instrument_id);
    EXPECT_EQ(pos.quantity, 0.0f);
    EXPECT_FLOAT_EQ(pos.avg_price, 0.0f);
}

TEST_F(PortfolioFillTest, ThrowsOnInvalidSell)
{
    int32_t instrument_id = 5;
    OrderId order_id = 5;

    auto bad_fill = std::make_shared<Fill>(order_id, Type::MARKET, Side::SELL, instrument_id, 100.0f, -5.0f, 0, 0);
    EXPECT_THROW(portfolio->onFill(bad_fill), std::invalid_argument);
}
