#include "socket/callbacks/callback.hpp"

std::function<std::string(const ix::WebSocketMessagePtr& msg)> OnReceivedMessageCallback = [](const ix::WebSocketMessagePtr& msg)->std::string{
    if (msg->type == ix::WebSocketMessageType::Message){
        return msg->str;
    }
    if (msg->type == ix::WebSocketMessageType::Open){
        std::cout << "Connection established." << std::endl;
    }
    if (msg->type == ix::WebSocketMessageType::Error){
        throw std::runtime_error("Connection error occured:" + msg->errorInfo.reason + "\n");
    }

    return "";
};