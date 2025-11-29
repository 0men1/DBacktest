#include "DEngine.h"

/**
 * @brief Construct a new DEngine::DEngine object
 *
 * @param net_liquidty
 * @param strategy
 * @param filepath
 * @param buffer_size
 */
DEngine::DEngine(
    std::unique_ptr<IStrategy> strategy,
    DOrderbook orderbook,
    const std::string &filepath,
    uint32_t buffer_size)
    : m_Reader(filepath, buffer_size),
      m_pStrategy(std::move(strategy)),
      m_Monitor(),
      m_orderBook(orderbook) {}

BacktestResults DEngine::compile_results()
{
    m_Results.orderbookSummary = m_orderBook.summary();
    m_Results.readerSummary = m_Reader.summary();

    return m_Results;
}

/**
 * @brief Overloaded function to print statistics to a file instead of console
 *
 * @param results
 * @param filename
 */
void DEngine::print_statistics(BacktestResults results, std::string filename) const
{
    std::ofstream fs(filename, std::ios::app);

    if (!fs || !fs.is_open())
    {
        throw std::runtime_error("error: could not open output file");
    }
}

/**
 * @brief Overloaded function to print statistics to console
 *
 * @param results
 * @param filename
 */
void DEngine::print_statistics(BacktestResults results) const
{
    printf("Reader Summary: \n");
    printf("    filepath: %s\n", m_Results.readerSummary.filePath.c_str());
    printf("    buffer size: %zu\n", m_Results.readerSummary.bufferSize);
    printf("    # Candles Processed: %d\n", m_Results.readerSummary.numCandlesProcessed);

    printf("Orderbook Summary: \n");
    printf("    # Limit Orders: %d\n", m_Results.orderbookSummary.numLimitOrders);
    printf("    # Market Orders: %d\n", m_Results.orderbookSummary.numMarketOrders);

    printf("    Portfolio Summary: \n");
    printf("        # Positions: %d\n", m_Results.orderbookSummary.portfolioSummary.num_positions);
    printf("        # Trades: %d\n", m_Results.orderbookSummary.portfolioSummary.num_trades);
    printf("        # Buys: %d\n", m_Results.orderbookSummary.portfolioSummary.num_buys);
    printf("        # Sells: %d\n", m_Results.orderbookSummary.portfolioSummary.num_sells);
    printf("        # Stopped: %d\n", m_Results.orderbookSummary.portfolioSummary.num_stopped);
    printf("        Realized PNL: %f\n", m_Results.orderbookSummary.portfolioSummary.realized_pnl);
    printf("        Commission: %f\n", m_Results.orderbookSummary.portfolioSummary.commission);
    printf("        Starting Liquidity: %f\n", m_Results.orderbookSummary.portfolioSummary.starting_liquidty);
    printf("        Ending Liquidity: %f\n", m_Results.orderbookSummary.portfolioSummary.ending_liquidity);
}

/**
 * @brief Run the engine + benchmarking
 *
 * When running a strategy, make a new thread and add it to a list of threads? Wait until all the threads are finished, then end the process and produce results for each thread
 *
 */
BacktestResults DEngine::run()
{
    if (m_bVerboseOutput)
    {
        std::cout << "Starting backtest..." << std::endl;
        std::cout << "Strategy: " << m_pStrategy->name() << std::endl
                  << std::endl;
    }

    m_Monitor.start("Session Timer");

    Candle curr_candle = Candle{};


    while (m_Reader.has_next())
    {
        m_Monitor.start("Candle Timer");
        const Candle &c = m_Reader.next();

        curr_candle = c;
        try
        {

            std::vector<Signal> signals = m_pStrategy->process(c, m_orderBook.portfolio().get_position(m_pStrategy->symbol()));

            // Add order to book then execute
            for (auto &s : signals)
            {
                m_orderBook.process_signal(s);
            }
            // m_orderBook.process_candle(c);

            m_Monitor.stop("Candle Timer");
        }
        catch (std::exception &e)
        {
            // CLOSE POSITION
            m_orderBook.flatten(m_pStrategy->symbol(), c.close);

            if (m_bVerboseOutput)
            {
                std::cerr << "Backtest error: " << e.what() << std::endl;
            }
            return BacktestResults{};
        }
    }

    // CLOSE POSITION
    m_orderBook.flatten(m_pStrategy->symbol(), curr_candle.close);

    m_Monitor.stop("Session Timer");

    BacktestResults results = compile_results();

    if (m_bVerboseOutput)
    {
        print_statistics(results);

        m_Monitor.print_timers();

        std::cout << "Backtest complete!" << std::endl;
    }

    return results;
}
