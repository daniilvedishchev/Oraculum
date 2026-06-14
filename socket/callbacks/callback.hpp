#pragma once

#include <iostream>
#include <functional>
#include <string>

#include <ixwebsocket/IXNetSystem.h>
#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXUserAgent.h>

extern std::function<void(const ix::WebSocketMessagePtr& msg, std::function<void(const ix::WebSocketMessagePtr&)> callback)> OnReceivedMessageCallback;