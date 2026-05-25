#pragma once

#include <iostream>
#include <functional>
#include <string>

#include <ixwebsocket/IXNetSystem.h>
#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXUserAgent.h>

extern std::function<std::string(const ix::WebSocketMessagePtr& msg)> OnReceivedMessageCallback;