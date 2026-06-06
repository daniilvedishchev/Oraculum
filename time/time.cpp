#include "time/time.hpp"

namespace oraculum {
    time::time(){}
    uint32_t time::DAYS_SINCE_UNIX(uint32_t ts){
        return ts/MS_TO_DAYS;
    }
    uint32_t time::HOURS_SINCE_UNIX(uint32_t ts){
        return ts/MS_TO_HOURS;
    }
    uint32_t time::MINUTES_SINCE_UNIX(uint32_t ts){
        return ts/MS_TO_MINUTES;
    }
}