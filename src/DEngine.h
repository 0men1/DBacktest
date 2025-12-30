#pragma once

#ifndef _DENGINE_H_
#define _DENGINE_H_

#include <memory>

#include "DContext.h"
#include "IStrategy.h"

class DEngine
{
  public:
    DEngine(std::unique_ptr<IStrategy> strategy, const std::string &filepath, uint32_t buffer_size);
    ~DEngine() = default;
    void run();

  private:
    std::shared_ptr<DContext> m_context;
};

#endif // __DENGINE_H_
