#pragma once

#ifndef _DREADER_H_
#define _DREADER_H_

#include "types/Candle.h"
#include <fstream>
#include <memory>
#include <thread>

struct ReaderSummary
{
    int numCandlesProcessed;
    std::string filePath;
    size_t bufferSize;
};

class DReader
{
  public:
    DReader(std::string filepath, uint32_t buffer_size);
    ~DReader();

  public:
    ReaderSummary summary()
    {
        return m_Summary;
    }
    std::shared_ptr<Candle> next();
    bool has_next();

  private:
    ReaderSummary m_Summary;
    std::string m_sFilepath;
    std::ifstream m_Filestream;

    void producerTask();

    size_t m_uiConsumerPos{0}; // Candle reading position in consumer buffer
    size_t m_uiBufferSize;     // Max candles to read from file
    size_t m_uiConsumerCount;  // How many candles to be read by consumer

    std::unique_ptr<CandleData[]> m_producerBuffer;
    std::unique_ptr<CandleData[]> m_consumerBuffer;

    std::thread m_producerThread;

    bool m_bEof = false;         // producer reached end of file
    bool m_bStop = false;        // Stop everything
    bool m_bBufferReady = false; // when producer buffer is ready

    std::condition_variable m_cv;
    std::mutex m_mutex;
};

#endif // DREADER_H
