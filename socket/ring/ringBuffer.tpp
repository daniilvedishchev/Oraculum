#pragma once 
#include "socket/ring/ringBuffer.hpp"

namespace oraculum {

    template<typename BufferItem>
    RingBuffer<BufferItem>::RingBuffer(size_t capacity)
        : BUFFER_SIZE_(capacity), closed_(false) {}

    template<typename BufferItem>
    bool RingBuffer<BufferItem>::isBufferFull_() {
        return buffer_.size() >= BUFFER_SIZE_;
    }

    template<typename BufferItem>
    void RingBuffer<BufferItem>::close() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            closed_ = true;
        }

        cv_.notify_all();
    }

    template<typename BufferItem>
    bool RingBuffer<BufferItem>::isBufferEmpty_() {
        return buffer_.empty();
    }

    template<typename BufferItem>
    size_t RingBuffer<BufferItem>::getSize() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            return buffer_.size();
        }
    }

    template<typename BufferItem>
    bool RingBuffer<BufferItem>::push(BufferItem&& item) {
        {
            std::lock_guard<std::mutex> lock(mutex_);

            if (isBufferFull_() || closed_) {
                return false;
            }

            buffer_.emplace_back(std::move(item));
        }
        cv_.notify_one();
        return true;
    }

    template<typename BufferItem>
    std::optional<BufferItem> RingBuffer<BufferItem>::pop() {
        std::unique_lock<std::mutex> lock(mutex_);

        cv_.wait(lock, [this]() {
            return closed_ || !buffer_.empty();
        });

        if (buffer_.empty()){
            return std::nullopt;
        }

        BufferItem out = std::move(buffer_.front());
        buffer_.pop_front();

        return out;
    }
}