#include "trades/liquidations/liquidationsStream.hpp"

#include "datasrc/endpoints/endpoints.hpp"
#include "datasrc/providers/baseUrl.hpp"
#include "datasrc/resolvers/providerResolver.hpp"
#include "filemanager/registry/registry.hpp"
#include "ixwebsocket/IXWebSocketMessage.h"
#include "trades/liquidations/liquidations.hpp"

#include <cstdint>
#include <iostream>
#include <string>
#include <thread>

namespace oraculum {
    LiquidationsStream::LiquidationsStream(Config& config,FileRegistry& registry):socket_{},cfg_(config),registry_(registry){

        status_ = Status::RUNNING;

        liquidationsMsgCallback = [this](const ix::WebSocketMessagePtr& msg) -> void {
            if (msg->type == ix::WebSocketMessageType::Message){
                socket_->buffer_.push(makeLiquidation(msg));
            }
        };

        socket_.emplace(cfg_.provider,
                        buildUrl(resolveProviderOrThrow(cfg_.provider),
                        Connection::FuturesMarketWebSocket,
                        makeLiquidationEndpoint(cfg_)),
                        liquidationsMsgCallback);

    };

    Liquidation LiquidationsStream::makeLiquidation(const ix::WebSocketMessagePtr& msg){
        const auto json = nlohmann::json::parse(msg->str);
        const auto& order = json.at("o");

        const std::string side = order.at("S").get<std::string>();
        const double avg_price = std::stod(order.at("ap").get<std::string>());
        const double quantity = std::stod(order.at("z").get<std::string>());

        return Liquidation{.event_time_ms = json.at("E").get<int64_t>(),
                            .trade_time_ms = order.at("T").get<int64_t>(),
                            .side = side == "SELL"
                                ? LiquidationSide::LongLiquidation
                                : LiquidationSide::ShortLiquidation,
                            .price_ticks = std::llround(avg_price/cfg_.tickSize),
                            .quantity_steps = std::llround(quantity/cfg_.stepSize),
                            .avg_price = avg_price,
                            .quantity = quantity,
                            .notional = avg_price * quantity
        };
    }

    void LiquidationsStream::consumeLiquidations(){
        while (status_ == Status::RUNNING){
            auto liquidation_from_buffer = socket_.value().buffer_.pop();
            if (liquidation_from_buffer){
                registry_.files.liquidations.writeLine(liquidationToCsv(liquidation_from_buffer.value()));
            } else break;
        }
    }

    std::string LiquidationsStream::liquidationToCsv(Liquidation& liquidation){
        std::ostringstream oss;
        oss << liquidation.event_time_ms << ','
            << liquidation.trade_time_ms << ","
            << (liquidation.side == LiquidationSide::LongLiquidation ? "LONG" : "SHORT") << ","
            << liquidation.price_ticks << ","
            << liquidation.quantity_steps << ","
            << liquidation.avg_price << ","
            << liquidation.quantity << ","
            << liquidation.notional;
        return oss.str();
    }

    void LiquidationsStream::start(){
        std::cout<<"[ORACULUM] Starting writing liquidation data."<<std::endl;
        socket_->socket_.start();
        thread_ = std::thread([this]()->void{
            consumeLiquidations();
        });
    }

    void LiquidationsStream::stop(){
        status_ = Status::STOPPED;
        socket_->socket_.stop();
        socket_->buffer_.close();
        if (thread_.joinable()){
            thread_.join();
        }
    }
}
