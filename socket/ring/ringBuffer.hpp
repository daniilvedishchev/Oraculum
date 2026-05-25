#pragma once
#include <deque>
#include "orderbook/depth/depthUpdate.hpp"

namespace oraculum {
    template<typename BufferItem>
    class RingBuffer {
    private:

        int BUFFER_SIZE_;

        bool isBufferFull_();
        bool isBufferEmpty_();

        std::deque<BufferItem> buffer_;

    public:
        RingBuffer(int capacity);
        void push(BufferItem& upd);
        BufferItem pop();
    };
}

