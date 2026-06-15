#include "trades/aggregated/aggregateTradesStream.hpp"

#include "datasrc/endpoints/endpoints.hpp"
#include "datasrc/providers/baseUrl.hpp"
#include "datasrc/resolvers/providerResolver.hpp"
#include "filemanager/registry/registry.hpp"
#include "ixwebsocket/IXWebSocketMessage.h"
#include "nlohmann/json_fwd.hpp"
#include "trades/structure/trades.h"
#include <chrono>
#include <cstdint>
#include <thread>

namespace oraculum {
    AggregateTradeStream::AggregateTradeStream(Config& config,FileRegistry& registry):socket_{},cfg_(config),registry_(registry){

        status_ = Status::RUNNING;

        aggregateTradesMsgCallback = [this](const ix::WebSocketMessagePtr& msg) -> void {
            if (msg->type == ix::WebSocketMessageType::Message){
                socket_->buffer_.push(makeTrade(msg));
            }
        };

        socket_.emplace(cfg_.provider,
                        buildUrl(resolveProviderOrThrow(cfg_.provider),
                        Connection::WebSocket,
                        makeAggregateTradeEndpoint(cfg_)),
                        aggregateTradesMsgCallback);

    };

    AggregateTrade AggregateTradeStream::makeTrade(const ix::WebSocketMessagePtr& msg){
        const auto json = nlohmann::json::parse(msg->str);
        const auto event_time_ms = json.at("E").get<int64_t>();
        const auto local_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();

        return AggregateTrade{
                            .event_time_ms = event_time_ms,
                            .trade_time_ms = json.at("T").get<int64_t>(),
                            .latency_ms = local_time_ms - event_time_ms,
                            .aggregate_trade_id = json.at("a").get<int64_t>(),
                            .price = std::llround(std::stod(json.at("p").get<std::string>())/cfg_.tickSize),
                            .quantity = std::llround(std::stod(json.at("q").get<std::string>())/cfg_.stepSize),
                            .is_buyer_maker = json.at("m").get<bool>()
        };
    }

    void AggregateTradeStream::consumeTrades(){
        while (status_ == Status::RUNNING){
            auto trade_from_buffer = socket_.value().buffer_.pop();
            if (trade_from_buffer){
                registry_.files.trades.writeLine(tradeToCsv(trade_from_buffer.value()));
            } else break;
        }
    }

    std::string AggregateTradeStream::tradeToCsv(AggregateTrade& trade){
        std::ostringstream oss;
        oss << trade.event_time_ms << ','
            << trade.trade_time_ms << ','
            << trade.latency_ms << ','
            << trade.aggregate_trade_id << ','
            << trade.price << ","
            << trade.quantity << ','
            << trade.is_buyer_maker;
        return oss.str();
    }

    void AggregateTradeStream::start(){
        std::cout<<"[ORACULUM] Starting writing trade data."<<std::endl;
        socket_->socket_.start();
        thread_ = std::thread([this]()->void{
            consumeTrades();
        });
    }

    void AggregateTradeStream::stop(){
        status_ = Status::STOPPED;
        socket_->socket_.stop(1000, "Normal closure");
        socket_->buffer_.close();
        if (thread_.joinable()){
            thread_.join();
        }
    }
}
