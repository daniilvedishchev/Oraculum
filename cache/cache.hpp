#pragma once

#include "providers/providers.hpp"

class cache {
    private:
        void _updateCache();
        void _createCache();
    public:
        cache(providers& provider);
        ~cache();
};
