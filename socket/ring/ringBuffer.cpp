#include "socket/ring/ringBuffer.hpp"

namespace oraculum {
    RingBuffer::RingBuffer(int capacity){
        BUFFER_SIZE_ = capacity;
    }

    bool RingBuffer::isBufferFull_(){
        return buffer_.size() == BUFFER_SIZE_;
    }

    bool RingBuffer::isBufferEmpty_(){
        return buffer_.empty();
    }

    void RingBuffer::bufferAdd_(DepthUpdate& upd){

        if (isBufferFull_()){
            throw std::runtime_error("Buffer overflow, consider changing buffer capacity.");
        }
        
        buffer_.emplace_back(upd);
    }

    DepthUpdate RingBuffer::bufferGet_(){

        if (isBufferEmpty_()){
            throw std::runtime_error("Buffer underflow, empty buffer.");
        }

        DepthUpdate upd = std::move(buffer_.front());
        buffer_.pop_front();
        return upd;
    }
}