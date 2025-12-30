#include "DReader.h"
#include <memory>

DReader::DReader(std::string filepath, uint32_t m_vBuffersize) : m_sFilepath(filepath), m_uiBufferSize(m_vBuffersize)
{
    m_Summary.filePath = filepath;
    m_Summary.bufferSize = m_uiBufferSize;
    m_Summary.numCandlesProcessed = 0;

    static_assert(sizeof(CandleData) == 32);

    m_consumerBuffer = std::make_unique<CandleData[]>(m_uiBufferSize);
    m_producerBuffer = std::make_unique<CandleData[]>(m_uiBufferSize);

    m_Filestream.open(m_sFilepath, std::ios::binary | std::ios::in);
    if (!m_Filestream || !m_Filestream.is_open())
        throw std::runtime_error("error: could not open file");

    m_producerThread = std::thread(&DReader::producerTask, this);
}

DReader::~DReader()
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_bStop = true;
        m_cv.notify_all();
    }
    if (m_producerThread.joinable())
        m_producerThread.join();
}

void DReader::producerTask()
{
    while (!m_bStop)
    {
        // read into the producer buffer. we own this buffer exclusively right now,
        // so no lock is needed
        m_Filestream.read(reinterpret_cast<char *>(m_producerBuffer.get()), m_uiBufferSize * sizeof(CandleData));
        std::streamsize bytes_read = m_Filestream.gcount();
        size_t read_count = bytes_read / sizeof(CandleData);

        bool local_eof = (bytes_read == 0);

        // now we need to swap so we must acquire the lock
        {
            std::unique_lock<std::mutex> lock(m_mutex);

            // wait until consumer buffer is empty
            m_cv.wait(lock, [&] { return !m_bBufferReady || m_bStop; });

            if (m_bStop)
                break;

            if (local_eof)
            {
                m_bEof = true;
                m_cv.notify_all();
                break;
            }

            std::swap(m_producerBuffer, m_consumerBuffer);
            m_uiConsumerPos = 0;
            m_uiConsumerCount = read_count;

            m_bBufferReady = true;
            m_cv.notify_all();
        }
    }
}

bool DReader::has_next()
{
    if (m_uiConsumerPos < m_uiConsumerCount)
        return true;

    // current buffer exhausted - need to swap.
    std::unique_lock<std::mutex> lock(m_mutex);

    if (m_bEof && !m_bBufferReady)
        return false;

    m_bBufferReady = false;
    m_cv.notify_all();

    // wait for producer to fill the back buffer
    m_cv.wait(lock, [&] { return m_bBufferReady || m_bEof; });

    // swap happens in the Producer thread.
    return m_uiConsumerPos < m_uiConsumerCount;
}

std::shared_ptr<Candle> DReader::next()
{
    if (!has_next())
        throw std::out_of_range("no more candles in file");

    m_Summary.numCandlesProcessed++;
    CandleData &data = m_consumerBuffer[m_uiConsumerPos++];

    return std::make_shared<Candle>(data);
}
