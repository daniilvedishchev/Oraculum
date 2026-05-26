#pragma once
#include <deque>
#include <mutex>
#include <condition_variable>

#include "orderbook/depth/depthUpdate.hpp"

#define DEFAULT_BUFFER_SIZE 1024
namespace oraculum {
    template<typename BufferItem>
    class RingBuffer {
    private:

        int BUFFER_SIZE_;

        bool isBufferFull_();
        bool isBufferEmpty_();

        std::deque<BufferItem> buffer_;
        
        std::mutex mutex_;
        std::condition_variable cv_;

    public:
        RingBuffer(size_t capacity = DEFAULT_BUFFER_SIZE);
        bool push(BufferItem& upd);
        BufferItem pop();
        size_t getSize();
    };
}

#include "socket/ring/ringBuffer.tpp"

