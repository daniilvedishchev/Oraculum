#include <string>
#include <nlohmann/json.hpp>

struct DepthUpdate {
    std::string u;
    std::string U;
    nlohmann::json raw;
};