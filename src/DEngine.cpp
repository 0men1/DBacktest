#include "DEngine.h"

#include "types/Order.h"
#include "types/Signal.h"
#include "utils/SymbolRegistry.h"

DEngine::DEngine(std::unique_ptr<IStrategy> strategy, const std::string &symbol, const std::string &filepath,
                 uint32_t buffer_size, double net_liquidity)
    : m_pStrategy(std::move(strategy)),
      m_pReader(std::make_unique<DReader>(SymbolRegistry::instance().registerSymbol(symbol), filepath, buffer_size)),
      m_pPortfolio(std::make_unique<DPortfolio>(net_liquidity)), m_pOrderBook(std::make_unique<DOrderBook>(10))
{
    m_pEventBus = std::make_shared<DEventBus>();
    m_pStrategy->init(m_pEventBus);
    m_pPortfolio->init(m_pEventBus);
    m_pOrderBook->init(m_pEventBus);
}

void DEngine::run()
{
    bool is_reader_active = true;

    while (is_reader_active || !m_pEventBus->m_events.empty())
    {
        if (is_reader_active && m_pEventBus->m_events.size() < 500)
        {
            int loaded_count = 0;
            while (loaded_count < 1000 && m_pReader->has_next())
            {
                auto candle = m_pReader->next();
                m_pEventBus->m_events.push(candle);
                loaded_count++;
            }
            if (loaded_count == 0 && !m_pReader->has_next())
            {
                is_reader_active = false;
            }
        }

        if (m_pEventBus->m_events.empty())
        {
            break;
        }

        std::shared_ptr<Event> event = m_pEventBus->m_events.top();
        m_pEventBus->m_events.pop();

        switch (event->type())
        {
        case CANDLE: {
            std::shared_ptr<Candle> candle = std::static_pointer_cast<Candle>(event);
            m_pStrategy->onCandle(candle);
            m_pOrderBook->onCandle(candle);
            m_pPortfolio->update_metrics(candle->timestamp(), candle->instrument_id(), candle->close());
            break;
        }

        case ORDER: {
            std::shared_ptr<Order> order = std::static_pointer_cast<Order>(event);
            m_pOrderBook->onOrder(order);
            break;
        }

        case SIGNAL: {
            std::shared_ptr<Signal> signal = std::static_pointer_cast<Signal>(event);
            m_pPortfolio->onSignal(signal);
            break;
        }
        case FILL: {
            std::shared_ptr<Fill> fill = std::static_pointer_cast<Fill>(event);
            m_pPortfolio->onFill(fill);
            break;
        }
        }
    }

    print_final_report();
}

void DEngine::print_final_report() const
{
    std::cout << "===========================" << std::endl;
    std::cout << "      BACKTEST FINISHED    " << std::endl;
    std::cout << "===========================" << std::endl;

    m_pReader->get_results().print_results();
    m_pPortfolio->getResults().print_results();
    m_pOrderBook->getResults().print_results();
}
