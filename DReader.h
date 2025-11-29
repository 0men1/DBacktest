#pragma once

#ifndef _DREADER_H_
#define _DREADER_H_

#include <fstream>
#include <vector>
#include <thread>
#include "types/Candle.h"

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
    const Candle &next();
    bool has_next();

public:
    ReaderSummary summary() { return m_Summary; }
    std::string &filepath() { return m_sFilepath; }
    size_t bufferSize() { return m_uiBufferSize; }

private:
    ReaderSummary m_Summary;

private:
    std::string m_sFilepath;
    std::ifstream m_Filestream;

    void producerTask();

    size_t m_uiConsumerPos{0}; // Candle reading position in consumer buffer
    size_t m_uiBufferSize;     // Max candles to read from file
    size_t m_uiConsumerCount;  // How many candles to be read by consumer
    std::vector<Candle> m_vBuffer;

    std::unique_ptr<Candle[]> m_producerBuffer;
    std::unique_ptr<Candle[]> m_consumerBuffer;

    std::thread m_producerThread;

    bool m_bEof = false;         // producer reached end of file
    bool m_bStop = false;        // Stop everything
    bool m_bBufferReady = false; // when producer buffer is ready

    std::condition_variable m_cv;
    std::mutex m_mutex;
};

#endif // DREADER_H
