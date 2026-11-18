#pragma once

#include "fwd.h"

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

template <bool upper = false, class Char>
constexpr void mac_to_chars(const uint8_t* mac, Char* out)
{
    using namespace estd;   // Needs estd >= v0.8.13-alpha2 due to bug in to_chars
    const uint8_t* end = mac + 5;

    auto to_hex = [](Char* o, uint8_t m) EMBR_NET_INLINE_BASE
    {
        // estd::detail::to_chars operates in reverse.  In other words, it is
        // naturally right justified.
        detail::to_chars_result<Char*> rc;
        
        if constexpr(upper)
            rc = detail::to_chars_upper<16>(o, o + 2, m);
        else
            rc = detail::to_chars<16>(o, o + 2, m);

        assert(rc.ec == errc{});

        if(rc.ptr != o)   *o = '0';
    };

    for(; mac < end; ++mac, out += 3)
    {
        to_hex(out, *mac);
        *(out + 2) = ':';
    }

    to_hex(out, *mac);
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
    char s[max_size];

    mac_to_chars(mac, s);
    s[max_size - 1] = 0;

    return { s, max_size };
}

template <bool upper = false>
EMBR_NET_INLINE constexpr internal::mac_string mac_to_string(const mac& mac)
{
    return mac_to_string(mac.data());
}


}

