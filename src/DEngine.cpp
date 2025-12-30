#include "DEngine.h"

#include "DReader.h"
#include <memory>

DEngine::DEngine(std::unique_ptr<IStrategy> strategy, const std::string &filepath, uint32_t buffer_size)
{
    m_context = std::make_shared<DContext>(std::make_unique<DReader>(filepath, buffer_size), std::move(strategy));
    m_context->m_pStrategy->init(m_context.get());
}

void DEngine::run()
{
    bool is_reader_active = true;

    while (is_reader_active || !m_context->m_events.empty())
    {
        if (is_reader_active && m_context->m_events.size() < 500)
        {
            int loaded_count = 0;
            while (loaded_count < 1000 && m_context->m_reader->has_next())
            {
                auto candle = m_context->m_reader->next();
                m_context->m_events.push(candle);
                loaded_count++;
            }
            if (loaded_count == 0 && !m_context->m_reader->has_next())
            {
                is_reader_active = false;
            }
        }

        if (m_context->m_events.empty())
        {
            break;
        }

        std::shared_ptr<Event> event = m_context->m_events.top();
        m_context->m_events.pop();

        switch (event->type())
        {
        case CANDLE:
            m_context->m_pStrategy->onCandle(std::static_pointer_cast<Candle>(event));
            break;

        case ORDER:
            break;

        case SIGNAL:
            break;
        }
    }
}
