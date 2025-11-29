# DBacktest - public

A multi-threaded C++ backtesting engine.

This repository serves as a public demonstration of a very very basic core backtesting engine architecture. Proprietary strategies and optimization are maintained in private repositories. This project is intended for educational purposes.

## Data Format

The engine ingests historical data via binary files. Ensure your data files are structured as a contiguous sequence of candle records matching the following layout:

**Record Size:** 32 bytes (LittleEndian)

| Field | Type | Size |
| :--- | :--- | :--- |
| Timestamp | `uint64_t` | 8 bytes |
| Open | `float` | 4 bytes |
| High | `float` | 4 bytes |
| Low | `float` | 4 bytes |
| Close | `float` | 4 bytes |
| Volume | `double` | 8 bytes |

## How to Run

1.  Place your binary data files in the `./data` directory (or configure the path in `main.cpp`).
2.  Build & Run the project:
    ```bash
    make run
    ```

## Strategy Development

Strategies are implemented as state machines inheriting from the `IStrategy` interface. The engine is single-threaded per strategy instance, allowing safe internal state management (e.g., tracking swing highs/lows) across ticks.

### 1. The Interface (`IStrategy.h`)

All strategies must implement the pure virtual methods defined in the interface:

```cpp
class IStrategy {
public:
    virtual ~IStrategy() = default;
    // Core logic loop called on every candle
    virtual std::vector<Signal> process(const Candle &candle, const Position &p) = 0;
    // Metadata
    virtual const std::string &name() = 0;
    virtual const std::string &symbol() = 0;
};
