#include "orderbook/constructor/orderBookConstructor.hpp"

#include <chrono>
#include <cstdint>
#include <exception>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "cacheservice/CacheService.hpp"
#include "connector/builder/builder.hpp"
#include "connector/retry/retry.hpp"
#include "datasrc/endpoints/endpoints.hpp"
#include "filemanager/registry/registry.hpp"

namespace oraculum{
    std::int64_t nowMs() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
    }

    OrderBookConstructor::OrderBookConstructor(Config& cfg, FileManager& fm, CacheService& cache, FileRegistry& registry) : 
            cfg_(cfg), 
            fm_(fm),
            cache_(cache),
            registry_(registry)
        {
        
        firstUpdateReceived_= false;

        orderBookUpdateMsgCallback = [this](const ix::WebSocketMessagePtr& msg) {
            try {
                if (msg->type == ix::WebSocketMessageType::Message) {
                    if (auto upd = getOrderBookUpdate(msg)) {
                        bool ok = socket_.value().buffer_.push(std::move(*upd));
                        if (!ok) {
                            std::cerr << "Buffer overflow, missed updates.";
                        }
                    }
                }
            } catch (const std::exception& e) {
                std::cerr << e.what() << std::endl;
            }
        };

        socket_.emplace(cfg_.provider,
                        buildUrl(resolveProviderOrThrow(cfg_.provider),
                        Connection::WebSocket,
                        makeOrderBookUpdateEndpoint(cfg_)),
                        orderBookUpdateMsgCallback);
        
    }

    nlohmann::json OrderBookConstructor::parseOrderBookSnapshot(){
        std::string endpoint = makeOrderBookSnapshotEndpoint(cfg_);

        std::string url = buildUrl(resolveProviderOrThrow(cfg_.provider),Connection::Api,endpoint); 
        const auto response = requestRetryOrThrow(url);

        nlohmann::json snapshot;
        
        try {
            snapshot = nlohmann::json::parse(response.text);
        } catch (const std::exception& e) {
            throw std::runtime_error(std::string("Failed to parse Json snapshot:")+ e.what());
        }

        if (!snapshot.contains("lastUpdateId")) {
            throw std::runtime_error("Snapshot JSON does not contain lastUpdateId");
        }

        return snapshot;
    }
    
    /** 
        * @brief This function sets up the first updateID , usefull for orderbook reconstruction
        * and moves update to the buffer
    */
    std::optional<DepthUpdate> OrderBookConstructor::getOrderBookUpdate(const ix::WebSocketMessagePtr& msg){
        {
            std::lock_guard<std::mutex> lock(firstUpdateMutex_);
            const auto message = nlohmann::json::parse(msg->str);

            if (!firstUpdateReceived_){
                firstUpdateReceived_ = true;
                firstUpdateCv_.notify_all();
            }

            DepthUpdate update;
            update.raw = msg->str;
            update.firstUpdateId = message.at("U").get<long long>();
            update.lastUpdateId = message.at("u").get<long long>();
            update.lastUpdateTs = message.at("E").get<long long>();
            update.bids = message.at("b").get<std::vector<std::vector<std::string>>>();
            update.asks = message.at("a").get<std::vector<std::vector<std::string>>>();
            return update;
        }
    }

    void OrderBookConstructor::startSocket(){
        socket_.value().socket_.start();
    }

    void OrderBookConstructor::start(){
        std::cout<<"[ORACULUM] Starting writing orderbook data."<<std::endl;
        try {
            running_ = true;

            startSocket();

            auto snapshot = OrderBookSnapshotAfterFirstUpdate();
            MetaData meta = cache_.getMetaBySymbolOrThrow(cfg_.symbol); 
            localBook_.emplace(std::move(snapshot),meta.tickSize,meta.stepSize);

            if (cfg_.features) featureEngine_.emplace(*localBook_,registry_.files.features);

            consumerThread_ = std::thread([this]() {
                consume();
            });

        } catch (std::exception& e){
            std::cerr << "[start error] " << e.what() << std::endl;
            stop();
        }
    }

    OrderBookConstructor::~OrderBookConstructor() {
        std::cerr << "[DEBUG] destructor called\n";
        stop();
    }

    void OrderBookConstructor::stop(){
        std::cerr << "[DEBUG] stop called\n";
        running_= false;

        socket_.value().socket_.stop(1000, "Normal closure");
        socket_.value().buffer_.close();

        if (consumerThread_.joinable()) {
            consumerThread_.join();
        }
    }

    void OrderBookConstructor::consume(){
        while (running_){
            auto update = socket_.value().buffer_.pop();
            if (!update) break;

            bool needResync = update.value().firstUpdateId > localBook_->LAST_UPDATE_ID + 1;
            bool oldUpdate = update.value().lastUpdateId <= localBook_->LAST_UPDATE_ID;
            
            if (oldUpdate) continue;
            else if (needResync){
                {
                    std::lock_guard<std::mutex> lock(firstUpdateMutex_);
                    firstUpdateReceived_ = false;
                }
                auto snap = OrderBookSnapshotAfterFirstUpdate(); 
                localBook_.emplace(std::move(snap),cfg_.tickSize,cfg_.stepSize);

                if (cfg_.features) featureEngine_.emplace(*localBook_,registry_.files.features);

                continue;
            } else {
                localBook_->applyUpdate(update.value());
                if (cfg_.features) featureEngine_.value().features_1s(update.value());
                registry_.files.updates.writeLine(update->raw);
            }
        }
    }
    
    nlohmann::json OrderBookConstructor::OrderBookSnapshotAfterFirstUpdate() {   
        
        {   
            std::unique_lock<std::mutex> lock(firstUpdateMutex_);
            firstUpdateCv_.wait(lock,[this]{
                return firstUpdateReceived_ || !running_;
            });
        }

        snapshot = parseOrderBookSnapshot();
        SNAPSHOT_ID = snapshot.at("lastUpdateId").get<long long>();

        nlohmann::json storedSnapshot = {
            {"provider", cfg_.provider},
            {"symbol", cfg_.symbol},
            {"market", "spot"},
            {"type", cfg_.type},
            {"depth", cfg_.depth},
            {"local_ts_ms", nowMs()},
            {"last_update_id", SNAPSHOT_ID},
            {"snapshot", snapshot}
        };

        registry_.files.snapshots.writeLine(storedSnapshot.dump(2));

        return storedSnapshot;
    }
}
