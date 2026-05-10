#include "cache/cache.hpp"
#include "providers/binance/URLs.hpp"

cache::cache(providers& provider){

}

void cache::_request(providers& provider, std::string& typeURL){
    if (URLs.find(provider) == URLs.end()){
        throw std::runtime_error("This provider isn't suppported.");
    }
    auto response = cpr::Get(cpr::Url{URLs.find(provider)->second["typeURL"]});
}