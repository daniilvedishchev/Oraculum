#include "socket/callbacks/callback.hpp"
#include <functional>



std::function<void(const ix::WebSocketMessagePtr&, std::function<void(const ix::WebSocketMessagePtr&)> callback)> OnReceivedMessageCallback = [](const ix::WebSocketMessagePtr& msg,std::function<void(const ix::WebSocketMessagePtr&)> callback)->void {
    if (msg->type == ix::WebSocketMessageType::Message){
        callback(msg);
    }
    if (msg->type == ix::WebSocketMessageType::Open){
        std::cout << "Connection established." << std::endl;
    }
    if (msg->type == ix::WebSocketMessageType::Error){
        throw std::runtime_error("Connection error occured:" + msg->errorInfo.reason + "\n");
    }

    return;
};