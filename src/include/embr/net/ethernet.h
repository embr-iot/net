#pragma once

#include "fwd.h"

#include <estd/charconv.h>

#include <algorithm>

namespace embr::ethernet {

constexpr mac make_mac(const uint8_t* copy_from)
{
    mac mac;
    std::copy_n(copy_from, 6, mac.begin());
    return mac;
}

template <class Char>
constexpr bool mac_to_chars(const uint8_t* mac, Char* out)
{
    const uint8_t* end = mac + 5;
    auto to_hex = [&](Char* o, uint8_t m)
    {
        // estd::detail::to_chars operates in reverse.  In other words, it is
        // naturally right justified.
        estd::detail::to_chars_result<Char*> rc =
            estd::detail::to_chars<16>(o, o + 2, m);

        if(rc.ptr != o)   *o = '0';

        // FIX: Not ready yet
        //assert(rc.ec == estd::errc{});
    };

    for(; mac < end; ++mac, out += 3)
    {
        to_hex(out, *mac);
        *(out + 2) = ':';
    }

    to_hex(out, *mac);

    return {};
}

}

