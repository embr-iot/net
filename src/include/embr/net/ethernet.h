#pragma once

#include "fwd.h"
#include "ethernet/mac.h"
#include "ethernet/mac-conv.hpp"

#include <estd/charconv.h>
#include <estd/string.h>

#include <algorithm>

namespace embr::ethernet {

constexpr mac make_mac(const uint8_t* copy_from)
{
    mac mac;
    std::copy_n(copy_from, 6, mac.begin());
    return mac;
}

namespace internal {

constexpr char mac_string_reference[] = "0A:00:00:00:00:00";

using mac_string = estd::layer1::string<sizeof(internal::mac_string_reference)>;

}

// DEBT: Somewhere buried in estd::string land is a fixed-width layer1 string
// to avoid the extra null termination part
template <bool upper = false>
EMBR_NET_INLINE constexpr internal::mac_string mac_to_string(const uint8_t* mac)
{
    constexpr unsigned max_size = sizeof(internal::mac_string_reference);
    internal::mac_string s;

    mac_to_chars(mac, s.data());
    s[max_size - 1] = 0;

    return s;
}

template <bool upper = false>
EMBR_NET_INLINE constexpr internal::mac_string mac_to_string(const mac& mac)
{
    return mac_to_string(mac.data());
}


}

