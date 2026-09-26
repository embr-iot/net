#pragma once

#include "fwd.h"

#include <algorithm>

namespace embr::ethernet {

constexpr mac make_mac(const uint8_t* copy_from)
{
    mac mac;
    std::copy_n(copy_from, 6, mac.begin());
    return mac;
}

}

