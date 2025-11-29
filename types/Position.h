#pragma once

#include <string>

struct Position
{
    std::string symbol;
    float avg_price{0};
    float realized_pnl{0};
    float quantity{0};
};
