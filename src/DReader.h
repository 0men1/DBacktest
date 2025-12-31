#pragma once

#ifndef _DREADER_H_
#define _DREADER_H_

#include "types/Candle.h"
#include <fstream>
#include <memory>
#include <thread>

struct ReaderResults
{
    int numCandlesProcessed;
    std::string filePath;
    size_t bufferSize;

    void print_results() const
    {
        std::cout << "\n--- READER RESULTS ---\n";
        std::cout << "Num Candles Processed:  " << numCandlesProcessed << "\n";
        std::cout << "File Path:  " << filePath << "\n";
        std::cout << "Buffer Size:  " << bufferSize << "\n";
    }
};

class DReader
{
  public:
    DReader(int32_t instrument_id, std::string filepath, uint32_t buffer_size);
    ~DReader();

  public:
    const ReaderResults &get_results()
    {
        return m_Summary;
    }
    std::shared_ptr<Candle> next();
    bool has_next();

  private:
    std::string m_sFilepath;
    std::ifstream m_Filestream;

    int32_t m_iInstrumentId;

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

    ReaderResults m_Summary;
};

#endif // DREADER_H
