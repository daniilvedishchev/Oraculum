#pragma once

#include <cpr/cpr.h>

#include "providers/providers.hpp"
#include "providers/binance/binance.hpp"


class cache {
    private:
        void _updateCache();
        void _createCache();

        void _request(providers& provider);
    public:
        cache(providers& provider);
        ~cache();
};
