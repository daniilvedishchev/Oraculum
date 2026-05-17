#pragma once
#include <deque>
#include "orderbook/depth/depthUpdate.hpp"

namespace oraculum {
    class RingBuffer {
    private:

        int BUFFER_SIZE_;

        bool isBufferFull_();
        bool isBufferEmpty_();
        
        void bufferAdd_(DepthUpdate& upd);

        std::deque<DepthUpdate> buffer_;
        DepthUpdate bufferGet_();

    public:
        RingBuffer(int capacity);
    };
}

