#pragma once

#ifndef _DENGINE_H_
#define _DENGINE_H_

#include "DReader.h"
#include "IStrategy.h"
#include "DMonitor.h"
#include "DOrderbook.h"

#include <memory>

struct BacktestResults
{
    OrderbookSummary orderbookSummary;
    ReaderSummary readerSummary;
};

class DEngine
{

public:
    DEngine(std::unique_ptr<IStrategy> strategy, DOrderbook orderbook, const std::string &filepath, uint32_t buffer_size);
    ~DEngine() = default;

public:
    BacktestResults run();

public:
    void print_statistics(BacktestResults results, std::string filename) const;
    void print_statistics(BacktestResults results) const;
    void set_verbose(bool verbose) { m_bVerboseOutput = verbose; }

private:
    bool m_bVerboseOutput = false;
    BacktestResults m_Results;
    BacktestResults compile_results();

private:
    DReader m_Reader;
    std::unique_ptr<IStrategy> m_pStrategy;
    DMonitor m_Monitor;
    DOrderbook m_orderBook;
};

#endif // __DENGINE_H_
