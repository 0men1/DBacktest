#include "ExampleStrategy.h"
#include "src/DEngine.h"
#include <iostream>

int main(int argc, char *argv[])
{
    if (argc < 6)
    {
        std::cerr << "error: incorrect amount of args passed" << std::endl;
        return 0;
    }

    // char *asset = argv[1];
    char *exchange = argv[2];
    char *symbol = argv[3];
    char *timeframe = argv[4];

    char go_file_path[256];
    snprintf(go_file_path, sizeof(go_file_path), "../data/go/data_binaries/%s_%s_%s.bin", symbol, exchange, timeframe);

    DEngine engine(std::make_unique<ExampleStrategy>(), symbol, go_file_path, 100, 10000);
    engine.run();

    return 0;
}
