#pragma once

#include <iostream>
#include <functional>

#include <ixwebsocket/IXNetSystem.h>
#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXUserAgent.h>

extern std::function<void(const ix::WebSocketMessagePtr& msg)> OnReceivedMessageCallback;