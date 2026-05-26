#include "socket/ring/ringBuffer.hpp"

namespace oraculum {

    template<typename BufferItem>
    RingBuffer<BufferItem>::RingBuffer(size_t capacity)
        : BUFFER_SIZE_(capacity) {}

    template<typename BufferItem>
    bool RingBuffer<BufferItem>::isBufferFull_() {
        return buffer_.size() >= BUFFER_SIZE_;
    }

    template<typename BufferItem>
    bool RingBuffer<BufferItem>::isBufferEmpty_() {
        return buffer_.empty();
    }

    template<typename BufferItem>
    size_t RingBuffer<BufferItem>::getSize() {
        std::lock_guard<std::mutex> lock(mutex_);
        return buffer_.size();
    }

    template<typename BufferItem>
    bool RingBuffer<BufferItem>::push(BufferItem& upd) {
        {
            std::lock_guard<std::mutex> lock(mutex_);

            if (isBufferFull_()) {
                return false;
            }

            buffer_.emplace_back(upd);
        }

        cv_.notify_one();
        return true;
    }

    template<typename BufferItem>
    BufferItem RingBuffer<BufferItem>::pop() {
        std::unique_lock<std::mutex> lock(mutex_);

        cv_.wait(lock, [this]() {
            return !buffer_.empty();
        });

        BufferItem out = std::move(buffer_.front());
        buffer_.pop_front();

        return out;
    }

}