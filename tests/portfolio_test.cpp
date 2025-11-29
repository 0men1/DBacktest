#include "src/DPortfolio.h"
#include <cstdint>
#include <gtest/gtest.h>

class DPortfolioTest : public ::testing::Test {
protected:
  void SetUp() override {
    portfolio = std::make_unique<DPortfolio>(10000.0f, 1);
  }

  Order createBuyOrder(OrderId id, uint64_t timestamp, double quantity,
                       double price, const std::string &symbol) {
    return Order{id, timestamp, Order::Type::MARKET, quantity, price, symbol};
  }

  Order createSellOrder(OrderId id, uint64_t timestamp, double quantity,
                        double price, const std::string &symbol) {
    return Order{id, timestamp, Order::Type::MARKET, quantity, price, symbol};
  }

  int orderId = 0;
  std::unique_ptr<DPortfolio> portfolio;
};

TEST_F(DPortfolioTest, InitialStateTest) {
  PortfolioSummary summary = portfolio->summary();

  EXPECT_EQ(summary.starting_liquidty, 10000.0f);
  EXPECT_EQ(summary.ending_liquidity, 10000.0f);
  EXPECT_EQ(summary.num_positions, 0);
  EXPECT_EQ(summary.num_trades, 0);
  EXPECT_EQ(summary.num_buys, 0);
  EXPECT_EQ(summary.num_sells, 0);
  EXPECT_EQ(summary.realized_pnl, 0.0f);
  EXPECT_EQ(summary.commission, 1.0f);
}

TEST_F(DPortfolioTest, GetNonExistentPositionTest) {
  const Position &pos = portfolio->get_position("AAPL");

  EXPECT_EQ(pos.quantity, 0.0f);
  EXPECT_EQ(pos.avg_price, 0.0f);
  EXPECT_EQ(pos.realized_pnl, 0.0f);
}

TEST_F(DPortfolioTest, BuyOrderSuccessTest) {
  Order order = createBuyOrder(orderId++, 1, 10.0, 150.0, "AAPL");
  portfolio->process_buy_order(order);

  const Position &pos = portfolio->get_position("AAPL");
  EXPECT_EQ(pos.quantity, 10.0f);
  EXPECT_EQ(pos.avg_price, 150.0f);
  EXPECT_EQ(pos.realized_pnl, 0.0f);

  auto summary = portfolio->summary();
  EXPECT_EQ(summary.ending_liquidity,
            8490.0f); // 10000 - (10*150) - 10 (commission)
  EXPECT_EQ(summary.num_positions, 1);
  EXPECT_EQ(summary.num_trades, 1);
  EXPECT_EQ(summary.num_buys, 1);
}

TEST_F(DPortfolioTest, AddToExistingPositionTest) {
  Order order1 = createBuyOrder(orderId++, 1, 10.0, 100.0, "AAPL");
  portfolio->process_buy_order(order1);

  Order order2 = createBuyOrder(orderId++, 2, 20.0, 200.0, "AAPL");
  portfolio->process_buy_order(order2);

  const Position &pos = portfolio->get_position("AAPL");
  EXPECT_EQ(pos.quantity, 30.0f);
  EXPECT_FLOAT_EQ(pos.avg_price, 166.66666f);

  auto summary = portfolio->summary();
  EXPECT_EQ(summary.ending_liquidity, 4970.0f); // 10000 - 1000 - 4000 - 10 - 20
  EXPECT_EQ(summary.num_positions, 1);          // Still only 1 position
  EXPECT_EQ(summary.num_trades, 2);
  EXPECT_EQ(summary.num_buys, 2);
}

TEST_F(DPortfolioTest, PartialSellTest) {
  // First buy
  Order buyOrder = createBuyOrder(orderId++, 1, 20.0, 100.0, "AAPL");
  portfolio->process_buy_order(buyOrder);

  // Partial sell at higher price
  Order sellOrder = createSellOrder(orderId++, 2, 10.0, 150.0, "AAPL");
  portfolio->process_sell_order(sellOrder);

  const Position &pos = portfolio->get_position("AAPL");
  EXPECT_EQ(pos.quantity, 10.0f);
  EXPECT_EQ(pos.avg_price, 100.0f);    // Original avg price unchanged
  EXPECT_EQ(pos.realized_pnl, 500.0f); // (150-100) * 10

  auto summary = portfolio->summary();
  EXPECT_EQ(summary.ending_liquidity, 9470.0f);
  EXPECT_EQ(summary.realized_pnl, 500.0f);
  EXPECT_EQ(summary.num_trades, 2);
  EXPECT_EQ(summary.num_buys, 1);
  EXPECT_EQ(summary.num_sells, 1);
}

TEST_F(DPortfolioTest, CompleteSellTest) {
  // Buy position
  Order buyOrder = createBuyOrder(orderId++, 1, 10.0, 100.0, "AAPL");
  portfolio->process_buy_order(buyOrder);

  // Sell entire position at higher price
  Order sellOrder = createSellOrder(orderId++, 2, 10.0, 120.0, "AAPL");
  portfolio->process_sell_order(sellOrder);

  const Position &pos = portfolio->get_position("AAPL");
  EXPECT_EQ(pos.quantity, 0.0f); // Position should be cleared
  EXPECT_EQ(pos.avg_price, 0.0f);
  EXPECT_GT(pos.realized_pnl, 0.0f);

  auto summary = portfolio->summary();
  EXPECT_EQ(summary.ending_liquidity, 10180.0f);
  EXPECT_EQ(summary.realized_pnl, 200.0f); // (120-100) * 10
  EXPECT_EQ(summary.num_trades, 2);
}

/**
 * @brief Test insufficient liquidity for buy order
 *
 * Verifies that orders requiring more liquidity than available throw an
 * exception.
 */
TEST_F(DPortfolioTest, InsufficientLiquidityTest) {
  Order order = createBuyOrder(orderId++, 1, 1.0, 15000.0, "EXPENSIVE");
  EXPECT_THROW(portfolio->process_buy_order(order), std::invalid_argument);
}

TEST_F(DPortfolioTest, InvalidOrderZeroQuantityTest) {
  Order order = createBuyOrder(orderId++, 1, 0.0, 100.0, "AAPL");
  EXPECT_THROW(portfolio->process_buy_order(order), std::invalid_argument);
}

TEST_F(DPortfolioTest, InvalidOrderNegativeQuantityTest) {
  Order order = createBuyOrder(orderId++, 1, -5.0, 100.0, "AAPL");
  EXPECT_THROW(portfolio->process_buy_order(order), std::invalid_argument);
}

TEST_F(DPortfolioTest, InvalidOrderZeroPriceTest) {
  Order order = createBuyOrder(orderId++, 1, 10.0, 0.0, "AAPL");
  EXPECT_THROW(portfolio->process_buy_order(order), std::invalid_argument);
}

TEST_F(DPortfolioTest, InvalidOrderNegativePriceTest) {
  Order order = createBuyOrder(orderId++, 1, 10.0, -100.0, "AAPL");
  EXPECT_THROW(portfolio->process_buy_order(order), std::invalid_argument);
}

TEST_F(DPortfolioTest, InvalidOrderEmptySymbolTest) {
  Order order = createBuyOrder(orderId++, 1, 10.0, 100.0, "");
  EXPECT_THROW(portfolio->process_buy_order(order), std::invalid_argument);
}

TEST_F(DPortfolioTest, SellMoreThanAvailableTest) {
  // Buy small position
  Order buyOrder = createBuyOrder(orderId++, 1, 5.0, 100.0, "AAPL");
  portfolio->process_buy_order(buyOrder);

  // Try to sell more than owned
  Order sellOrder = createSellOrder(orderId++, 2, 10.0, 120.0, "AAPL");

  EXPECT_THROW(portfolio->process_sell_order(sellOrder), std::invalid_argument);
}

/**
 * @brief Test selling from non-existent position
 *
 * Verifies that attempting to sell a symbol with no position throws an
 * exception.
 */
TEST_F(DPortfolioTest, SellNonExistentPositionTest) {
  Order sellOrder = createSellOrder(orderId++, 1, 10.0, 100.0, "NONEXISTENT");
  EXPECT_THROW(portfolio->process_sell_order(sellOrder), std::invalid_argument);
}

TEST_F(DPortfolioTest, MultiplePositionsTest) {
  Order order1 = createBuyOrder(orderId++, 1, 10.0, 150.0, "AAPL");
  portfolio->process_buy_order(order1);

  Order order2 = createBuyOrder(orderId++, 2, 5.0, 200.0, "TSLA");
  portfolio->process_buy_order(order2);

  const Position &applePos = portfolio->get_position("AAPL");
  const Position &teslaPos = portfolio->get_position("TSLA");

  EXPECT_EQ(applePos.quantity, 10.0f);
  EXPECT_EQ(applePos.avg_price, 150.0f);
  EXPECT_EQ(teslaPos.quantity, 5.0f);
  EXPECT_EQ(teslaPos.avg_price, 200.0f);

  auto summary = portfolio->summary();
  EXPECT_EQ(summary.num_positions, 2);
  EXPECT_EQ(summary.num_trades, 2);
  EXPECT_EQ(summary.ending_liquidity, 7485.0f); // 10000 - 1500 - 1000 - 10 - 5
}

TEST_F(DPortfolioTest, LossScenarioTest) {
  Order buyOrder = createBuyOrder(orderId++, 1, 10.0, 150.0, "AAPL");
  portfolio->process_buy_order(buyOrder);

  Order sellOrder = createSellOrder(orderId++, 2, 10.0, 120.0, "AAPL");
  portfolio->process_sell_order(sellOrder);

  auto summary = portfolio->summary();
  EXPECT_EQ(summary.realized_pnl, -300.0f);     // (120-150) * 10
  EXPECT_EQ(summary.ending_liquidity, 9680.0f); // 10000 - 1500 - 10 + 1200 - 10
}

TEST_F(DPortfolioTest, ExactLiquidityTest) {
  Order order = createBuyOrder(orderId++, 1, 100.0, 100.0, "AAPL");
  EXPECT_THROW(portfolio->process_buy_order(order), std::invalid_argument);
}

TEST_F(DPortfolioTest, CommissionCalculationTest) {
  Order buyOrder = createBuyOrder(orderId++, 2, 10.0, 100.0, "AAPL");
  portfolio->process_buy_order(buyOrder);

  auto summary1 = portfolio->summary();
  EXPECT_EQ(summary1.ending_liquidity, 8990.0f); // 10000 - 1000 - 10

  Order sellOrder = createSellOrder(orderId++, 2, 10.0, 100.0, "AAPL");
  portfolio->process_sell_order(sellOrder);

  auto summary2 = portfolio->summary();
  EXPECT_EQ(summary2.ending_liquidity, 9980.0f); // 8990 + 1000 - 10
  EXPECT_EQ(summary2.commission, 1.0f);
}

/**
 * @brief Test fractional share handling
 */
TEST_F(DPortfolioTest, FractionalSharesTest) {
  Order buyOrder = createBuyOrder(orderId++, 1, 0.5, 1000.0, "BTC");
  portfolio->process_buy_order(buyOrder);

  const Position &pos = portfolio->get_position("BTC");
  EXPECT_EQ(pos.quantity, 0.5f);
  EXPECT_EQ(pos.avg_price, 1000.0f);

  auto summary = portfolio->summary();
  EXPECT_EQ(summary.ending_liquidity, 9499.5f); // 10000 - 500 - 0.5
}

/**
 * @brief Test very large position handling
 */
TEST_F(DPortfolioTest, LargeQuantityTest) {
  Order buyOrder = createBuyOrder(orderId++, 1, 1000.0, 9.0, "PENNY");
  portfolio->process_buy_order(buyOrder);

  const Position &pos = portfolio->get_position("PENNY");
  EXPECT_EQ(pos.quantity, 1000.0f);
  EXPECT_EQ(pos.avg_price, 9.0f);

  auto summary = portfolio->summary();
  EXPECT_EQ(summary.ending_liquidity, 0.0f); // 10000 - 9000 - 1000
}

/**
 * @brief Test position averaging with multiple different prices
 */
TEST_F(DPortfolioTest, ComplexAveragingTest) {
  Order order1 = createBuyOrder(orderId++, 1, 10.0, 100.0, "AAPL");
  portfolio->process_buy_order(order1);

  Order order2 = createBuyOrder(orderId++, 2, 5.0, 120.0, "AAPL");
  portfolio->process_buy_order(order2);

  Order order3 = createBuyOrder(orderId++, 3, 15.0, 80.0, "AAPL");
  portfolio->process_buy_order(order3);

  const Position &pos = portfolio->get_position("AAPL");
  EXPECT_EQ(pos.quantity, 30.0f);
  EXPECT_NEAR(pos.avg_price, 93.333333f, 0.001f);
}

/**
 * @brief Test multiple buy and sell cycles
 */
TEST_F(DPortfolioTest, MultipleCyclesTest) {
  // Cycle 1: Buy and sell
  Order buy1 = createBuyOrder(orderId++, 1, 10.0, 100.0, "AAPL");
  portfolio->process_buy_order(buy1);

  Order sell1 = createSellOrder(orderId++, 2, 10.0, 110.0, "AAPL");
  portfolio->process_sell_order(sell1);

  // Cycle 2: Buy and sell again
  Order buy2 = createBuyOrder(orderId++, 3, 20.0, 105.0, "AAPL");
  portfolio->process_buy_order(buy2);

  Order sell2 = createSellOrder(orderId++, 4, 20.0, 115.0, "AAPL");
  portfolio->process_sell_order(sell2);

  auto summary = portfolio->summary();
  EXPECT_EQ(summary.realized_pnl,
            300.0f); // (110-100)*10 + (115-105)*20 = 100 + 200
  EXPECT_EQ(summary.num_trades, 4);
  EXPECT_EQ(summary.num_buys, 2);
  EXPECT_EQ(summary.num_sells, 2);
}

/**
 * @brief Test position close using close_position method
 */
TEST_F(DPortfolioTest, ClosePositionTest) {
  Order buyOrder = createBuyOrder(orderId++, 1, 15.0, 200.0, "TSLA");
  portfolio->process_buy_order(buyOrder);

  const Position &pos1 = portfolio->get_position("TSLA");
  EXPECT_EQ(pos1.quantity, 15.0f);

  bool closed = portfolio->close_position("TSLA", 100);
  EXPECT_TRUE(closed);

  const Position &pos2 = portfolio->get_position("TSLA");
  EXPECT_EQ(pos2.quantity, 0.0f);
}

/**
 * @brief Test close_position on non-existent position
 */
TEST_F(DPortfolioTest, CloseNonExistentPositionTest) {
  bool closed = portfolio->close_position("NONEXISTENT", 100);
  EXPECT_FALSE(closed);
}

/**
 * @brief Test commission impact on multiple trades
 */
TEST_F(DPortfolioTest, CommissionImpactTest) {
  // Buy 5 positions - each should cost commission
  for (int i = 0; i < 5; i++) {
    Order buyOrder =
        createBuyOrder(orderId++, i, 1.0, 100.0, "TEST" + std::to_string(i));
    portfolio->process_buy_order(buyOrder);
  }

  auto summary = portfolio->summary();
  EXPECT_EQ(summary.num_positions, 5);
  // 10000 - (5 * 100) - (5 * 1) = 9495
  EXPECT_EQ(summary.ending_liquidity, 9495.0f);
}

/**
 * @brief Test selling entire position in multiple parts
 */
TEST_F(DPortfolioTest, PartialSellMultipleTest) {
  Order buyOrder = createBuyOrder(orderId++, 1, 100.0, 50.0, "STOCK");
  portfolio->process_buy_order(buyOrder);

  Order sell1 = createSellOrder(orderId++, 2, 25.0, 55.0, "STOCK");
  portfolio->process_sell_order(sell1);

  Order sell2 = createSellOrder(orderId++, 3, 25.0, 60.0, "STOCK");
  portfolio->process_sell_order(sell2);

  Order sell3 = createSellOrder(orderId++, 4, 25.0, 58.0, "STOCK");
  portfolio->process_sell_order(sell3);

  Order sell4 = createSellOrder(orderId++, 5, 25.0, 62.0, "STOCK");
  portfolio->process_sell_order(sell4);

  const Position &pos = portfolio->get_position("STOCK");
  EXPECT_EQ(pos.quantity, 0.0f);

  auto summary = portfolio->summary();
  // Total PnL: (55-50)*25 + (60-50)*25 + (58-50)*25 + (62-50)*25
  // = 125 + 250 + 200 + 300 = 875
  EXPECT_EQ(summary.realized_pnl, 875.0f);
}

/**
 * @brief Test zero commission portfolio
 */
TEST_F(DPortfolioTest, ZeroCommissionTest) {
  // Create portfolio with 0 commission
  auto zeroCommPortfolio = std::make_unique<DPortfolio>(10000.0f, 0);

  Order buyOrder = createBuyOrder(orderId++, 1, 10.0, 100.0, "AAPL");
  zeroCommPortfolio->process_buy_order(buyOrder);

  auto summary = zeroCommPortfolio->summary();
  EXPECT_EQ(summary.ending_liquidity, 9000.0f); // No commission deducted
}

/**
 * @brief Test high commission impact
 */
TEST_F(DPortfolioTest, HighCommissionTest) {
  auto highCommPortfolio = std::make_unique<DPortfolio>(10000.0f, 10);

  Order buyOrder = createBuyOrder(orderId++, 1, 5.0, 100.0, "AAPL");
  highCommPortfolio->process_buy_order(buyOrder);

  auto summary = highCommPortfolio->summary();
  EXPECT_EQ(summary.ending_liquidity, 9450.0f); // 10000 - 500 - 50
}

/**
 * @brief Test realized PnL accumulation across multiple positions
 */
TEST_F(DPortfolioTest, RealizedPnLAccumulationTest) {
  // Position 1: Profit
  Order buy1 = createBuyOrder(orderId++, 1, 10.0, 100.0, "AAPL");
  portfolio->process_buy_order(buy1);
  Order sell1 = createSellOrder(orderId++, 2, 10.0, 120.0, "AAPL");
  portfolio->process_sell_order(sell1);

  // Position 2: Loss
  Order buy2 = createBuyOrder(orderId++, 3, 5.0, 200.0, "TSLA");
  portfolio->process_buy_order(buy2);
  Order sell2 = createSellOrder(orderId++, 4, 5.0, 180.0, "TSLA");
  portfolio->process_sell_order(sell2);

  // Position 3: Profit
  Order buy3 = createBuyOrder(orderId++, 5, 8.0, 50.0, "GOOGL");
  portfolio->process_buy_order(buy3);
  Order sell3 = createSellOrder(orderId++, 6, 8.0, 65.0, "GOOGL");
  portfolio->process_sell_order(sell3);

  auto summary = portfolio->summary();
  // PnL: (120-100)*10 + (180-200)*5 + (65-50)*8 = 200 - 100 + 120 = 220
  EXPECT_EQ(summary.realized_pnl, 220.0f);
}

/**
 * @brief Test exact liquidity boundary
 */
TEST_F(DPortfolioTest, ExactLiquidityBoundaryTest) {
  // Should succeed: 10000 liquidity - 9999 cost - 1 commission = 0
  Order order1 = createBuyOrder(orderId++, 1, 1.0, 9999.0, "AAPL");
  EXPECT_NO_THROW(portfolio->process_buy_order(order1));

  auto summary = portfolio->summary();
  EXPECT_EQ(summary.ending_liquidity, 0.0f);
}

/**
 * @brief Test buy after complete sell
 */
TEST_F(DPortfolioTest, BuyAfterCompleteSellTest) {
  // Buy, sell all, then buy again
  Order buy1 = createBuyOrder(orderId++, 1, 10.0, 100.0, "AAPL");
  portfolio->process_buy_order(buy1);

  Order sell1 = createSellOrder(orderId++, 2, 10.0, 110.0, "AAPL");
  portfolio->process_sell_order(sell1);

  const Position &pos1 = portfolio->get_position("AAPL");
  EXPECT_EQ(pos1.quantity, 0.0f);

  // Buy again - should create new position
  Order buy2 = createBuyOrder(orderId++, 3, 5.0, 105.0, "AAPL");
  portfolio->process_buy_order(buy2);

  const Position &pos2 = portfolio->get_position("AAPL");
  EXPECT_EQ(pos2.quantity, 5.0f);
  EXPECT_EQ(pos2.avg_price, 105.0f);

  auto summary = portfolio->summary();
  EXPECT_EQ(summary.num_positions, 2);
  EXPECT_EQ(summary.num_trades, 3);
}

/**
 * @brief Test many small trades performance
 */
TEST_F(DPortfolioTest, ManySmallTradesTest) {
  // Execute 50 small buy trades
  for (int i = 0; i < 50; i++) {
    Order buyOrder = createBuyOrder(orderId++, i, 1.0, 10.0, "STOCK");
    portfolio->process_buy_order(buyOrder);
  }

  const Position &pos = portfolio->get_position("STOCK");
  EXPECT_EQ(pos.quantity, 50.0f);
  EXPECT_EQ(pos.avg_price, 10.0f);

  auto summary = portfolio->summary();
  EXPECT_EQ(summary.num_trades, 50);
  EXPECT_EQ(summary.num_positions, 1);
}

/**
 * @brief Test position with extreme price values
 */
TEST_F(DPortfolioTest, ExtremePriceValuesTest) {
  // Very high price
  Order buy1 = createBuyOrder(orderId++, 1, 0.001, 9000.0, "EXPENSIVE");
  portfolio->process_buy_order(buy1);

  const Position &pos1 = portfolio->get_position("EXPENSIVE");
  EXPECT_EQ(pos1.quantity, 0.001f);
  EXPECT_EQ(pos1.avg_price, 9000.0f);

  // Very low price (penny stock)
  Order buy2 = createBuyOrder(orderId++, 2, 100.0, 0.1, "PENNY");
  portfolio->process_buy_order(buy2);

  const Position &pos2 = portfolio->get_position("PENNY");
  EXPECT_EQ(pos2.quantity, 100.0f);
  EXPECT_EQ(pos2.avg_price, 0.1f);
}

/**
 * @brief Test selling with loss scenario
 */
TEST_F(DPortfolioTest, MultipleStockLossTest) {
  Order buy1 = createBuyOrder(orderId++, 1, 10.0, 100.0, "STOCK1");
  portfolio->process_buy_order(buy1);

  Order buy2 = createBuyOrder(orderId++, 2, 10.0, 200.0, "STOCK2");
  portfolio->process_buy_order(buy2);

  Order sell1 = createSellOrder(orderId++, 3, 10.0, 80.0, "STOCK1");
  portfolio->process_sell_order(sell1);

  Order sell2 = createSellOrder(orderId++, 4, 10.0, 180.0, "STOCK2");
  portfolio->process_sell_order(sell2);

  auto summary = portfolio->summary();
  // Loss on STOCK1: (80-100)*10 = -200
  // Loss on STOCK2: (180-200)*10 = -200
  // Total: -400
  EXPECT_EQ(summary.realized_pnl, -400.0f);
}
