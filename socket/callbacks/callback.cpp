#include "socket/callbacks/callback.hpp"

std::function<void(const ix::WebSocketMessagePtr& msg)> OnReceivedMessageCallback = [](const ix::WebSocketMessagePtr& msg)->void{
    if (msg->type == ix::WebSocketMessageType::Message){
        std::cout << "received message: " << msg->str << std::endl;
    }
    if (msg->type == ix::WebSocketMessageType::Open){
        std::cout << "Connection established." << std::endl;
    }
    if (msg->type == ix::WebSocketMessageType::Error){
        std::cout << "Connection error occured:" << msg->errorInfo.reason << std::endl;
    }
};