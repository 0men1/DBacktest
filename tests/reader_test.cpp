#include "src/DReader.h"
#include <gtest/gtest.h>
#include <memory>

#ifndef TEST_RESOURCE_DIR
#define TEST_RESOURCE_DIR
#endif

class DReaderTest : public ::testing::Test {
protected:
  void SetUp() override {
    std::string filepath =
        std::string(TEST_RESOURCE_DIR) + "sol-usd_coinbase_1m_100.bin";
    reader = std::make_unique<DReader>(filepath, 100);
  }

  std::unique_ptr<DReader> reader;
};

TEST_F(DReaderTest, VerifyContinuityAndDuration) {
  auto prev_candle = reader->next();
  ASSERT_NE(prev_candle, nullptr);
  EXPECT_GT(prev_candle->close(), 0);

  std::shared_ptr<Candle> curr_candle = nullptr;
  size_t count = 1;

  while (reader->has_next()) {
    curr_candle = reader->next();
    count++;

    int64_t step = curr_candle->timestamp() - prev_candle->timestamp();
    EXPECT_EQ(step, 60) << "Gap detected at candle index " << count;

    prev_candle = curr_candle;
  }

  EXPECT_EQ(count, 100);
}

TEST_F(DReaderTest, VerifyOHLCVInvariants) {
  while (reader->has_next()) {
    auto candle = reader->next();
    EXPECT_GE(candle->high(), candle->low()) << "High is lower than Low";
    EXPECT_GE(candle->high(), candle->open()) << "High is lower than Open";
    EXPECT_GE(candle->high(), candle->close()) << "High is lower than Close";
    EXPECT_LE(candle->low(), candle->open()) << "Low is higher than Open";
    EXPECT_LE(candle->low(), candle->close()) << "Low is higher than Close";
    EXPECT_GE(candle->volume(), 0);
    EXPECT_EQ(candle->timestamp() % 60, 0) << "Timestamp not aligned to minute";
  }
}
