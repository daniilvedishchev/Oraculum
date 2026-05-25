#include <string>
#include <nlohmann/json.hpp>

struct DepthUpdate {
    long long firstUpdateId;
    long long lastUpdateId;
    nlohmann::json raw;
};