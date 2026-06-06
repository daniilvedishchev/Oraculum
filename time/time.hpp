#include <cstdint>

#include "time/constants.hpp"

class time
{
private:
    uint32_t DAYS;
    uint32_t HOURS;
    uint32_t MINUTES;
public:
    time(/* args */);
    uint32_t DAYS_SINCE_UNIX(uint32_t ts);
    uint32_t HOURS_SINCE_UNIX(uint32_t ts);
    uint32_t MINUTES_SINCE_UNIX(uint32_t ts);
};


