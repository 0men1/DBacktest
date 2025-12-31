#pragma once

#include "src/DOrderBook.h"
#ifndef _DENGINE_H_
#define _DENGINE_H_

#include "DEventBus.h"
#include "DPortfolio.h"
#include "DReader.h"
#include "IStrategy.h"
#include <memory>

class DEngine
{
  public:
    DEngine(std::unique_ptr<IStrategy> strategy, const std::string &filepath, uint32_t buffer_size, int net_liquidty);
    ~DEngine() = default;
    void run();

  private:
    std::shared_ptr<DEventBus> m_pEventBus;
    std::unique_ptr<IStrategy> m_pStrategy;
    std::unique_ptr<DReader> m_pReader;
    std::unique_ptr<DPortfolio> m_pPortfolio;
    std::unique_ptr<DOrderBook> m_pOrderBook;
};

#endif // __DENGINE_H_
