#include "src/DEngine.h"
#include <cstdio>
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc < 5) {
    std::cerr << "error: incorrect amount of args passed" << std::endl;
    std::cerr << "usage: ./program asset exchange symbol timeframe"
              << std::endl;
    return 1;
  }

  char *asset = argv[1];
  char *exchange = argv[2];
  char *symbol = argv[3];
  char *timeframe = argv[4];

  char go_file_path[256];
  snprintf(go_file_path, sizeof(go_file_path), "../data/go/%s_%s_%s.bin",
           symbol, exchange, timeframe);

  /*
   *
  std::unique_ptr<YOUR_STRATEGY> strat = std::make_unique<YOUR_STRATEGY>();

  DOrderbook orderbook(DPortfolio(10000, 0));

  DEngine engine(std::move(strat), orderbook, go_file_path, 100000);

  engine.set_verbose(true);
  BacktestResults result = engine.run();
   * */

  return 0;
}
