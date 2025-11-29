#include "src/DOrderbook.h"
#include <gtest/gtest.h>
#include <memory>

class DOrderbookTest : public ::testing::Test {
protected:
  void SetUp() override {
    orderbook = std::make_unique<DOrderbook>(DPortfolio(10000.0f, 1));
  }

  std::unique_ptr<DOrderbook> orderbook;

  Signal createSignal(Signal::Type type, const std::string &symbol,
                      double quantity, double price) {
    Signal signal(type, 1, quantity, price, symbol);
    return signal;
  }

  Candle createCandle(uint64_t timestamp, float open, float high, float low,
                      float close, double volume = 1000.0) {
    Candle candle;
    candle.timestamp = timestamp;
    candle.open = open;
    candle.high = high;
    candle.low = low;
    candle.close = close;
    candle.volume = volume;
    return candle;
  }
};

TEST_F(DOrderbookTest, OrderQuantityTest_One) {
  Signal s = createSignal(Signal::Type::LONG, "GOOG", 1, 100);
  orderbook->process_signal(s);

  OrderbookSummary summary = orderbook->summary();

  EXPECT_EQ(summary.portfolioSummary.ending_liquidity, 9899);
  EXPECT_EQ(summary.portfolioSummary.num_buys, 1);
  EXPECT_EQ(summary.portfolioSummary.num_trades, 1);
}

TEST_F(DOrderbookTest, OrderQuantityTest_Multiple) {
  std::vector<Signal> signals = {
      createSignal(Signal::Type::LONG, "GOOG", 1, 100),
      createSignal(Signal::Type::LONG, "GOOG", 1, 100),
      createSignal(Signal::Type::LONG, "GOOG", 1, 100),
      createSignal(Signal::Type::LONG, "GOOG", 1, 100),
  };

  for (auto &s : signals) {
    orderbook->process_signal(s);
  }

  OrderbookSummary summary = orderbook->summary();

  EXPECT_EQ(summary.portfolioSummary.ending_liquidity, 9596.0);
  EXPECT_EQ(summary.portfolioSummary.num_buys, 4);
  EXPECT_EQ(summary.portfolioSummary.num_trades, 4);
}

TEST_F(DOrderbookTest, OrderQuantityTest_Mixed) {
  std::vector<Signal> long_signals = {
      createSignal(Signal::Type::LONG, "GOOG", 1, 100),
      createSignal(Signal::Type::LONG, "GOOG", 1, 100),
      createSignal(Signal::Type::LONG, "GOOG", 1, 100),
      createSignal(Signal::Type::LONG, "GOOG", 1, 100),
  };

  for (auto &s : long_signals) {
    orderbook->process_signal(s);
  }

  OrderbookSummary summary = orderbook->summary();

  EXPECT_EQ(summary.portfolioSummary.ending_liquidity, 9596.0);
  EXPECT_EQ(summary.portfolioSummary.num_buys, 4);
  EXPECT_EQ(summary.portfolioSummary.num_trades, 4);

  std::vector<Signal> short_signals = {
      createSignal(Signal::Type::SHORT, "GOOG", 1, 105),
      createSignal(Signal::Type::SHORT, "GOOG", 1, 105),
  };

  for (auto &s : short_signals) {
    orderbook->process_signal(s);
  }

  OrderbookSummary summary2 = orderbook->summary();
  EXPECT_EQ(summary2.portfolioSummary.num_sells, 2);
  EXPECT_EQ(summary2.portfolioSummary.num_trades, 6);
}

TEST_F(DOrderbookTest, LongPosition_Creation) {
  Signal longSignal = createSignal(Signal::Type::LONG, "AAPL", 10, 100.0);
  orderbook->process_signal(longSignal);

  Position position = orderbook->portfolio().get_position("AAPL");
  EXPECT_EQ(position.quantity, 10);
  EXPECT_EQ(position.avg_price, 100.0);

  OrderbookSummary summary = orderbook->summary();
  EXPECT_EQ(summary.portfolioSummary.num_buys, 1);
  EXPECT_EQ(summary.portfolioSummary.num_positions, 1);
  EXPECT_EQ(summary.portfolioSummary.ending_liquidity,
            9000.0 - 10); // 10000 - (100*10) - commission
}

TEST_F(DOrderbookTest, LargePosition_Creation) {
  Signal longSignal = createSignal(Signal::Type::LONG, "TSLA", 10, 250.0);
  orderbook->process_signal(longSignal);

  Position position = orderbook->portfolio().get_position("TSLA");
  EXPECT_EQ(position.quantity, 10);
  EXPECT_EQ(position.avg_price, 250.0);

  OrderbookSummary summary = orderbook->summary();
  EXPECT_EQ(summary.portfolioSummary.ending_liquidity,
            10000.0 - (250.0 * 10) - 10); // Starting - cost - commission
}

TEST_F(DOrderbookTest, FractionalPosition_Creation) {
  Signal longSignal = createSignal(Signal::Type::LONG, "BTC", 0.5, 5000.0);
  orderbook->process_signal(longSignal);

  Position position = orderbook->portfolio().get_position("BTC");
  EXPECT_EQ(position.quantity, 0.5);
  EXPECT_EQ(position.avg_price, 5000.0);

  OrderbookSummary summary = orderbook->summary();
  EXPECT_EQ(summary.portfolioSummary.num_positions, 1);
  EXPECT_NEAR(summary.portfolioSummary.ending_liquidity,
              10000.0 - (5000.0 * 0.5) - 0.5, 0.01);
}

TEST_F(DOrderbookTest, MultiplePositions_Independent) {
  Signal appleSignal = createSignal(Signal::Type::LONG, "AAPL", 25, 15.0);
  Signal googleSignal = createSignal(Signal::Type::LONG, "GOOGL", 5, 280.0);
  Signal microsoftSignal = createSignal(Signal::Type::LONG, "MSFT", 100, 30.0);

  orderbook->process_signal(appleSignal);
  orderbook->process_signal(googleSignal);
  orderbook->process_signal(microsoftSignal);

  EXPECT_EQ(orderbook->portfolio().get_position("AAPL").quantity, 25);
  EXPECT_EQ(orderbook->portfolio().get_position("GOOGL").quantity, 5);
  EXPECT_EQ(orderbook->portfolio().get_position("MSFT").quantity, 100);

  OrderbookSummary summary = orderbook->summary();
  EXPECT_EQ(summary.portfolioSummary.num_positions, 3);
  EXPECT_EQ(summary.portfolioSummary.num_buys, 3);

  Signal msftSell = createSignal(Signal::Type::SHORT, "MSFT", 50, 35.0);
  orderbook->process_signal(msftSell);

  EXPECT_EQ(orderbook->portfolio().get_position("MSFT").quantity, 50);
  EXPECT_EQ(orderbook->portfolio().get_position("AAPL").quantity, 25);
  EXPECT_EQ(orderbook->portfolio().get_position("GOOGL").quantity, 5);
}

TEST_F(DOrderbookTest, Position_PartialClose) {
  // Create position
  Signal longSignal = createSignal(Signal::Type::LONG, "SPY", 50, 100.0);
  orderbook->process_signal(longSignal);

  Position position = orderbook->portfolio().get_position("SPY");
  EXPECT_EQ(position.quantity, 50);

  // Partially close position
  Signal shortSignal = createSignal(Signal::Type::SHORT, "SPY", 30, 110.0);
  orderbook->process_signal(shortSignal);

  // Verify position is partially closed
  Position updatedPosition = orderbook->portfolio().get_position("SPY");
  EXPECT_EQ(updatedPosition.quantity, 20);

  OrderbookSummary summary = orderbook->summary();
  EXPECT_EQ(summary.portfolioSummary.num_sells, 1);
}

TEST_F(DOrderbookTest, Position_ProfitRealization) {
  // Create position
  Signal longSignal = createSignal(Signal::Type::LONG, "QQQ", 10, 350.0);
  orderbook->process_signal(longSignal);

  Position position = orderbook->portfolio().get_position("QQQ");
  EXPECT_EQ(position.quantity, 10);
  EXPECT_EQ(position.avg_price, 350.0);

  // Sell at a profit
  Signal shortSignal = createSignal(Signal::Type::SHORT, "QQQ", 10, 400.0);
  orderbook->process_signal(shortSignal);

  Position updatedPosition = orderbook->portfolio().get_position("QQQ");
  EXPECT_EQ(updatedPosition.quantity, 0);

  OrderbookSummary summary = orderbook->summary();
  EXPECT_GT(summary.portfolioSummary.realized_pnl, 0);
}

TEST_F(DOrderbookTest, Position_Averaging) {
  Signal signal1 = createSignal(Signal::Type::LONG, "AMD", 10, 100.0);
  orderbook->process_signal(signal1);

  Position position1 = orderbook->portfolio().get_position("AMD");
  EXPECT_EQ(position1.quantity, 10);
  EXPECT_EQ(position1.avg_price, 100.0);

  Signal signal2 = createSignal(Signal::Type::LONG, "AMD", 10, 120.0);
  orderbook->process_signal(signal2);

  Position position2 = orderbook->portfolio().get_position("AMD");
  EXPECT_EQ(position2.quantity, 20);
  EXPECT_NEAR(position2.avg_price, 110.0, 0.01); // (100*10 + 120*10) / 20 = 110

  OrderbookSummary summary = orderbook->summary();
  EXPECT_EQ(summary.portfolioSummary.num_buys, 2);
  EXPECT_EQ(summary.portfolioSummary.num_positions, 1);
}

TEST_F(DOrderbookTest, AccumulatedPosition_MultipleOrders) {
  Signal order1 = createSignal(Signal::Type::LONG, "META", 10, 300.0); // 3000
  Signal order2 = createSignal(Signal::Type::LONG, "META", 5, 310.0);  // 4650
  Signal order3 = createSignal(Signal::Type::LONG, "META", 5, 320.0);  // 9

  orderbook->process_signal(order1);
  orderbook->process_signal(order2);
  orderbook->process_signal(order3);

  Position position = orderbook->portfolio().get_position("META");
  EXPECT_EQ(position.quantity, 20);

  EXPECT_NEAR(position.avg_price, 307.5, 0.01);

  OrderbookSummary summary = orderbook->summary();
  EXPECT_EQ(summary.portfolioSummary.num_buys, 3);
  EXPECT_EQ(summary.portfolioSummary.num_positions, 1);

  Signal sellSignal = createSignal(Signal::Type::SHORT, "META", 20, 330.0);
  orderbook->process_signal(sellSignal);

  Position updatedPosition = orderbook->portfolio().get_position("META");
  EXPECT_EQ(updatedPosition.quantity, 0);
}

TEST_F(DOrderbookTest, ShortSignal_NoPosition_Error) {
  Signal shortSignal = createSignal(Signal::Type::SHORT, "NFLX", 10, 400.0);

  EXPECT_THROW(orderbook->process_signal(shortSignal), std::invalid_argument);

  Position position = orderbook->portfolio().get_position("NFLX");
  EXPECT_EQ(position.quantity, 0);

  OrderbookSummary summary = orderbook->summary();
  EXPECT_EQ(summary.portfolioSummary.num_trades, 0);
}

TEST_F(DOrderbookTest, Flatten_Position) {
  Signal longSignal = createSignal(Signal::Type::LONG, "AAPL", 20, 150.0);
  orderbook->process_signal(longSignal);

  Position position = orderbook->portfolio().get_position("AAPL");
  EXPECT_EQ(position.quantity, 20);

  bool flattened = orderbook->flatten("AAPL", 100.0);
  EXPECT_TRUE(flattened);

  Position updatedPosition = orderbook->portfolio().get_position("AAPL");
  EXPECT_EQ(updatedPosition.quantity, 0);
}

TEST_F(DOrderbookTest, Flatten_NonExistentPosition) {
  bool flattened = orderbook->flatten("NONEXISTENT", 100);
  EXPECT_FALSE(flattened);
}

TEST_F(DOrderbookTest, ComplexTrading_BuySellCycles) {
  Signal buy1 = createSignal(Signal::Type::LONG, "TSLA", 10, 200.0);
  orderbook->process_signal(buy1);

  Signal sell1 = createSignal(Signal::Type::SHORT, "TSLA", 3, 220.0);
  orderbook->process_signal(sell1);

  Signal buy2 = createSignal(Signal::Type::LONG, "TSLA", 5, 210.0);
  orderbook->process_signal(buy2);

  Signal sell2 = createSignal(Signal::Type::SHORT, "TSLA", 4, 230.0);
  orderbook->process_signal(sell2);

  Position position = orderbook->portfolio().get_position("TSLA");
  EXPECT_EQ(position.quantity, 8); // 10 - 3 + 5 - 4

  OrderbookSummary summary = orderbook->summary();
  EXPECT_EQ(summary.portfolioSummary.num_buys, 2);
  EXPECT_EQ(summary.portfolioSummary.num_sells, 2);
  EXPECT_EQ(summary.portfolioSummary.num_trades, 4);
}

TEST_F(DOrderbookTest, VerySmallFractionalQuantity) {
  Signal longSignal = createSignal(Signal::Type::LONG, "BTC", 0.001, 50000.0);
  orderbook->process_signal(longSignal);

  Position position = orderbook->portfolio().get_position("BTC");
  EXPECT_NEAR(position.quantity, 0.001, 0.0001);
  EXPECT_EQ(position.avg_price, 50000.0);
}

TEST_F(DOrderbookTest, LiquidityExhaustion) {
  // Buy enough to nearly exhaust liquidity (10000 starting)
  Signal buy1 = createSignal(Signal::Type::LONG, "EXPENSIVE", 1, 9000.0);
  orderbook->process_signal(buy1);

  OrderbookSummary summary1 = orderbook->summary();
  EXPECT_EQ(summary1.portfolioSummary.ending_liquidity,
            999.0); // 10000 - 9000 - 1

  // This should succeed
  Signal buy2 =
      createSignal(Signal::Type::LONG, "CHEAP", 1.0, 998); // 999.0 - 998 - 1
  EXPECT_NO_THROW(orderbook->process_signal(buy2));

  OrderbookSummary summary2 = orderbook->summary();
  EXPECT_NEAR(summary2.portfolioSummary.ending_liquidity, 0.0, 0.1);
}

TEST_F(DOrderbookTest, PreventOverselling_MultipleSells) {
  Signal longSignal = createSignal(Signal::Type::LONG, "STOCK", 100, 50.0);
  orderbook->process_signal(longSignal);

  Signal sell1 = createSignal(Signal::Type::SHORT, "STOCK", 60, 55.0);
  orderbook->process_signal(sell1);

  Position position = orderbook->portfolio().get_position("STOCK");
  EXPECT_EQ(position.quantity, 40);

  Signal sell2 = createSignal(Signal::Type::SHORT, "STOCK", 50, 60.0);
  EXPECT_THROW(orderbook->process_signal(sell2), std::invalid_argument);

  // Position should be unchanged
  Position position2 = orderbook->portfolio().get_position("STOCK");
  EXPECT_EQ(position2.quantity, 40);
}

TEST_F(DOrderbookTest, ZeroCommission_Trading) {
  auto zeroCommOrderbook =
      std::make_unique<DOrderbook>(DPortfolio(10000.0f, 0));

  Signal buySignal = createSignal(Signal::Type::LONG, "AAPL", 10, 100.0);
  zeroCommOrderbook->process_signal(buySignal);

  OrderbookSummary summary = zeroCommOrderbook->summary();
  EXPECT_EQ(summary.portfolioSummary.ending_liquidity, 9000.0); // No commission
}

TEST_F(DOrderbookTest, ManySmallBuys_Averaging) {
  // Buy in 10 small increments at increasing prices
  for (int i = 0; i < 10; i++) {
    Signal buy = createSignal(Signal::Type::LONG, "STOCK", 5, 100.0 + i * 5.0);
    orderbook->process_signal(buy);
  }

  Position position = orderbook->portfolio().get_position("STOCK");
  EXPECT_EQ(position.quantity, 50);

  // Average should be around 122.5
  EXPECT_NEAR(position.avg_price, 122.5, 0.1);

  OrderbookSummary summary = orderbook->summary();
  EXPECT_EQ(summary.portfolioSummary.num_buys, 10);
}

/**
 * @brief Test complete position closure and reopening
 *
 * Tests that a position can be fully closed and then reopened.
 */
TEST_F(DOrderbookTest, CloseAndReopen_Position) {
  // Initial position
  Signal buy1 = createSignal(Signal::Type::LONG, "AAPL", 10, 150.0);
  orderbook->process_signal(buy1);

  // Close completely
  Signal sell1 = createSignal(Signal::Type::SHORT, "AAPL", 10, 160.0);
  orderbook->process_signal(sell1);

  Position position1 = orderbook->portfolio().get_position("AAPL");
  EXPECT_EQ(position1.quantity, 0);

  // Reopen position
  Signal buy2 = createSignal(Signal::Type::LONG, "AAPL", 5, 155.0);
  orderbook->process_signal(buy2);

  Position position2 = orderbook->portfolio().get_position("AAPL");
  EXPECT_EQ(position2.quantity, 5);
  EXPECT_EQ(position2.avg_price, 155.0); // new average, not influenced by old
}

/**
 * @brief Test trading multiple different symbols simultaneously
 *
 * Tests that orderbook handles portfolio diversification correctly.
 */
TEST_F(DOrderbookTest, MultipleSymbols_Diversification) {
  std::vector<std::string> symbols = {"AAPL", "GOOGL", "MSFT", "TSLA", "AMZN"};

  for (const auto &symbol : symbols) {
    Signal buy = createSignal(Signal::Type::LONG, symbol, 10, 100.0);
    orderbook->process_signal(buy);
  }

  OrderbookSummary summary = orderbook->summary();
  EXPECT_EQ(summary.portfolioSummary.num_positions, 5);
  EXPECT_EQ(summary.portfolioSummary.num_buys, 5);

  for (const auto &symbol : symbols) {
    Position pos = orderbook->portfolio().get_position(symbol);
    EXPECT_EQ(pos.quantity, 10);
  }

  Signal sell1 = createSignal(Signal::Type::SHORT, "AAPL", 10, 110.0);
  Signal sell2 = createSignal(Signal::Type::SHORT, "MSFT", 10, 105.0);
  orderbook->process_signal(sell1);
  orderbook->process_signal(sell2);

  OrderbookSummary summary2 = orderbook->summary();
  EXPECT_EQ(summary2.portfolioSummary.num_sells, 2);

  EXPECT_EQ(orderbook->portfolio().get_position("AAPL").quantity, 0);
  EXPECT_EQ(orderbook->portfolio().get_position("GOOGL").quantity, 10);
  EXPECT_EQ(orderbook->portfolio().get_position("MSFT").quantity, 0);
}

/**
 * @brief Test extreme price volatility scenario
 *
 * Tests handling of large price swings.
 */
TEST_F(DOrderbookTest, ExtremePriceVolatility) {
  // Buy at low price
  Signal buy = createSignal(Signal::Type::LONG, "VOLATILE", 10, 50.0);
  orderbook->process_signal(buy);

  // Sell at much higher price (10x)
  Signal sell = createSignal(Signal::Type::SHORT, "VOLATILE", 10, 500.0);
  orderbook->process_signal(sell);

  OrderbookSummary summary = orderbook->summary();
  // PnL should be (500 - 50) * 10 = 4500
  EXPECT_EQ(summary.portfolioSummary.realized_pnl, 4500.0);
}

/**
 * @brief Test loss scenario with complete position closure
 *
 * Tests that losses are calculated correctly.
 */
TEST_F(DOrderbookTest, LossScenario_Complete) {
  Signal buy = createSignal(Signal::Type::LONG, "LOSING", 20, 200.0);
  orderbook->process_signal(buy);

  Signal sell = createSignal(Signal::Type::SHORT, "LOSING", 20, 150.0);
  orderbook->process_signal(sell);

  OrderbookSummary summary = orderbook->summary();
  // PnL should be (150 - 200) * 20 = -1000
  EXPECT_EQ(summary.portfolioSummary.realized_pnl, -1000.0);
}

/**
 * @brief Test mixed profit and loss across multiple positions
 *
 * Tests PnL accumulation with both winning and losing positions.
 */
TEST_F(DOrderbookTest, MixedProfitLoss_Multiple) {
  // Profitable position
  Signal buy1 = createSignal(Signal::Type::LONG, "WINNER", 10, 100.0);
  orderbook->process_signal(buy1);
  Signal sell1 = createSignal(Signal::Type::SHORT, "WINNER", 10, 150.0);
  orderbook->process_signal(sell1);

  // Losing position
  Signal buy2 = createSignal(Signal::Type::LONG, "LOSER", 10, 200.0);
  orderbook->process_signal(buy2);
  Signal sell2 = createSignal(Signal::Type::SHORT, "LOSER", 10, 180.0);
  orderbook->process_signal(sell2);

  // Another profitable position
  Signal buy3 = createSignal(Signal::Type::LONG, "WINNER2", 5, 50.0);
  orderbook->process_signal(buy3);
  Signal sell3 = createSignal(Signal::Type::SHORT, "WINNER2", 5, 80.0);
  orderbook->process_signal(sell3);

  OrderbookSummary summary = orderbook->summary();
  EXPECT_EQ(summary.portfolioSummary.realized_pnl, 450.0);
}

/**
 * @brief Test partial sells at different prices
 *
 * Tests PnL calculation when selling in parts at varying prices.
 */
TEST_F(DOrderbookTest, PartialSells_DifferentPrices) {
  Signal buy = createSignal(Signal::Type::LONG, "STOCK", 100, 90.0);
  orderbook->process_signal(buy);

  Position pos1 = orderbook->portfolio().get_position("STOCK");
  EXPECT_EQ(pos1.quantity, 100);

  // Sell 25 at 110
  Signal sell1 = createSignal(Signal::Type::SHORT, "STOCK", 25, 110.0);
  orderbook->process_signal(sell1);

  // Sell 25 at 120
  Signal sell2 = createSignal(Signal::Type::SHORT, "STOCK", 25, 120.0);
  orderbook->process_signal(sell2);

  // Sell 50 at 105
  Signal sell3 = createSignal(Signal::Type::SHORT, "STOCK", 50, 105.0);
  orderbook->process_signal(sell3);

  Position pos2 = orderbook->portfolio().get_position("STOCK");
  EXPECT_EQ(pos2.quantity, 0);

  OrderbookSummary summary = orderbook->summary();
  EXPECT_EQ(pos2.realized_pnl, 2000.0);
  EXPECT_EQ(summary.portfolioSummary.realized_pnl, 2000.0);
}

/**
 * @brief Test high-frequency trading simulation
 *
 * Tests system stability with rapid order execution.
 */
TEST_F(DOrderbookTest, HighFrequency_Trading) {
  // Execute 100 alternating buy/sell orders
  for (int i = 0; i < 50; i++) {
    Signal buy = createSignal(Signal::Type::LONG, "HFT", 1, 100.0 + i * 0.1);
    orderbook->process_signal(buy);

    Signal sell =
        createSignal(Signal::Type::SHORT, "HFT", 1, 100.0 + i * 0.1 + 0.5);
    orderbook->process_signal(sell);
  }

  OrderbookSummary summary = orderbook->summary();
  EXPECT_EQ(summary.portfolioSummary.num_trades, 100);
  EXPECT_EQ(summary.portfolioSummary.num_buys, 50);
  EXPECT_EQ(summary.portfolioSummary.num_sells, 50);

  Position position = orderbook->portfolio().get_position("HFT");
  EXPECT_EQ(position.quantity, 0); // All positions closed
}

/**
 * @brief Test position with single share
 *
 * Tests handling of single share positions.
 */
TEST_F(DOrderbookTest, SingleShare_Position) {
  Signal buy = createSignal(Signal::Type::LONG, "EXPENSIVE", 1, 5000.0);
  orderbook->process_signal(buy);

  Position position = orderbook->portfolio().get_position("EXPENSIVE");
  EXPECT_EQ(position.quantity, 1);
  EXPECT_EQ(position.avg_price, 5000.0);

  Signal sell = createSignal(Signal::Type::SHORT, "EXPENSIVE", 1, 5500.0);
  orderbook->process_signal(sell);

  Position position2 = orderbook->portfolio().get_position("EXPENSIVE");
  EXPECT_EQ(position2.quantity, 0);
}

/**
 * @brief Test accumulation through many small orders
 *
 * Tests building a large position through many small buys.
 */
TEST_F(DOrderbookTest, Accumulation_ManySmallOrders) {
  // Buy 1 share 100 times
  for (int i = 0; i < 100; i++) {
    Signal buy = createSignal(Signal::Type::LONG, "ACCUMULATE", 1, 50.0);
    orderbook->process_signal(buy);
  }

  Position position = orderbook->portfolio().get_position("ACCUMULATE");
  EXPECT_EQ(position.quantity, 100);
  EXPECT_EQ(position.avg_price, 50.0);

  OrderbookSummary summary = orderbook->summary();
  EXPECT_EQ(summary.portfolioSummary.num_buys, 100);
}

/**
 * @brief Test summary statistics accuracy
 *
 * Tests that all summary statistics are tracked correctly.
 */
TEST_F(DOrderbookTest, Summary_Statistics) {
  Signal buy1 = createSignal(Signal::Type::LONG, "AAPL", 10, 150.0);
  Signal buy2 = createSignal(Signal::Type::LONG, "GOOGL", 4, 2000.0);
  Signal sell1 = createSignal(Signal::Type::SHORT, "AAPL", 5, 160.0);

  orderbook->process_signal(buy1);
  orderbook->process_signal(buy2);
  orderbook->process_signal(sell1);

  OrderbookSummary summary = orderbook->summary();

  EXPECT_EQ(summary.portfolioSummary.num_trades, 3);
  EXPECT_EQ(summary.portfolioSummary.num_buys, 2);
  EXPECT_EQ(summary.portfolioSummary.num_sells, 1);
  EXPECT_EQ(summary.portfolioSummary.num_positions, 2);
  EXPECT_GT(summary.portfolioSummary.realized_pnl,
            0); // Should have some profit
}
