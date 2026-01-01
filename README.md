# DBacktest - Public

A multi-threaded C++ backtesting engine.

This repository serves as a public demonstration of a core backtesting engine architecture. Proprietary strategies and optimization engines are maintained in private repositories. This project is intended for educational purposes.

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

Strategies are implemented by inheriting from the `IStrategy` interface. The engine utilizes an **Event Bus** architecture: rather than returning objects, strategies are initialized with a pointer to the main event bus, allowing them to push `Signal` events asynchronously during the `onCandle` execution.

### 1. The Interface (`IStrategy.h`)

All strategies must implement the `onCandle` method. The `init` method is handled by the base class to inject the event bus dependency.

```cpp
#pragma once

#ifndef _ISTRATEGY_H_
#define _ISTRATEGY_H_

#include <memory>

struct DEventBus;
class Candle;

class IStrategy
{
  public:
    virtual ~IStrategy() = default;
    
    // Core logic loop called on every candle
    virtual void onCandle(std::shared_ptr<Candle> candle) = 0;

    // Dependency injection for the Event Bus
    virtual void init(std::shared_ptr<DEventBus> eventBus)
    {
        m_pEventBus = eventBus;
    };

  protected:
    std::shared_ptr<DEventBus> m_pEventBus = nullptr;
};

#endif // _ISTRATEGY_H
```

### 2. Example Implementation

Below is a basic random-entry strategy. It inherits the `m_pEventBus` directly from `IStrategy`, so no manual initialization code is required in the derived class.

**Header (`ExampleStrategy.h`):**

```cpp
#pragma once

#ifndef _EXAMPLE_STRATEGY_H
#define _EXAMPLE_STRATEGY_H

#include "src/IStrategy.h"
#include "types/Candle.h"

class ExampleStrategy : public IStrategy
{
    // We only need to implement the abstract method.
    // m_pEventBus and init() are inherited from IStrategy.
    void onCandle(std::shared_ptr<Candle> candle) override;
};

#endif
```

**Source (`ExampleStrategy.cpp`):**

```cpp
#include "ExampleStrategy.h"
#include "src/DEventBus.h" // EventBus definition needed for push()
#include "types/Signal.h"
#include <ctime>
#include <memory>

void ExampleStrategy::onCandle(std::shared_ptr<Candle> candle)
{
    int randomNumber = (std::rand() % 5) + 1;

    // Example logic: Buy if condition is met
    if (randomNumber == 3)
    {
        // Accessing the protected m_pEventBus from the base class
        m_pEventBus->m_events.push(std::make_shared<Signal>(
            Type::MARKET, 
            Side::BUY, 
            candle->instrument_id(),
            candle->close(), 
            3.0, // Quantity
            candle->timestamp()
        ));
    }
}
```
