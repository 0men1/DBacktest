#include "DContext.h"

DContext::DContext(std::unique_ptr<DReader> reader, std::unique_ptr<IStrategy> strategy)
    : m_reader(std::move(reader)), m_pStrategy(std::move(strategy))
{
}
