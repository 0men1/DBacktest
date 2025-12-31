#include "DEngine.h"

#include "types/Order.h"
#include "types/Signal.h"

DEngine::DEngine(std::unique_ptr<IStrategy> strategy, const std::string &filepath, uint32_t buffer_size,
                 int net_liquidity)
    : m_pStrategy(std::move(strategy)), m_pReader(std::make_unique<DReader>(filepath, buffer_size)),
      m_pPortfolio(std::make_unique<DPortfolio>(net_liquidity))
{
    m_pEventBus = std::make_shared<DEventBus>();
    m_pStrategy->init(m_pEventBus);
    m_pPortfolio->init(m_pEventBus);
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
            std::cout << "CANDLE" << std::endl;
            break;
        }

        case ORDER: {
            std::shared_ptr<Order> order = std::static_pointer_cast<Order>(event);
            std::cout << "ORDER" << std::endl;
            break;
        }

        case SIGNAL: {
            std::shared_ptr<Signal> signal = std::static_pointer_cast<Signal>(event);
            m_pPortfolio->onSignal(signal);
            std::cout << "SIGNAL" << std::endl;
            break;
        }
        case FILL: {
            std::cout << "FILL" << std::endl;
            break;
        }
        }
    }
}
