#pragma once

#ifndef _DENGINE_H_
#define _DENGINE_H_

#include "DContext.h"
#include "IStrategy.h"
#include <memory>

class DEngine {
public:
  DEngine(std::unique_ptr<IStrategy> strategy, const std::string &filepath,
          uint32_t buffer_size);
  ~DEngine() = default;
  void run();

private:
  std::shared_ptr<BacktestContext> m_context;
};

#endif // __DENGINE_H_
