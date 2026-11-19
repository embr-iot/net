#pragma once

#include "fwd.h"

#include <estd/algorithm.h>

#include <array>

namespace embr::ethernet {

//using mac = std::array<uint8_t, 6>;
// Using a wrapper so that we can activate ADL
struct mac : std::array<uint8_t, 6>
{
    using base_type = std::array<uint8_t, 6>;

    // NOT WELL TESTED
#if __cplusplus < 202002L
    mac() = default;

    constexpr mac(std::initializer_list<uint8_t> values)
    {
        assert(values.size() == 6);
        estd::copy(values.begin(), values.end(), begin());
    }
#endif
};

namespace addr {

constexpr mac broadcast { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

}

}
