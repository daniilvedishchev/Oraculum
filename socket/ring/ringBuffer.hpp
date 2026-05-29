#pragma once
#include <deque>
#include <mutex>
#include <condition_variable>
#include <optional>

#include "orderbook/depth/depthUpdate.hpp"

#define DEFAULT_BUFFER_SIZE 1024
namespace oraculum {
    template<typename BufferItem>
    class RingBuffer {
    private:

        int BUFFER_SIZE_;

        bool isBufferFull_();
        bool isBufferEmpty_();

        bool closed_;

        std::deque<BufferItem> buffer_;
        
        std::mutex mutex_;
        std::condition_variable cv_;

    public:
        RingBuffer(size_t capacity = DEFAULT_BUFFER_SIZE);
        bool push(BufferItem&& upd);
        std::optional<BufferItem> pop();
        size_t getSize();
        void close();
    };
}

#include "socket/ring/ringBuffer.tpp"

