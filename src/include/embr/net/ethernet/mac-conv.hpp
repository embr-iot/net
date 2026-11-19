#pragma once

#include "fwd.h"

#include <estd/charconv.h>

namespace embr::ethernet {

template <bool upper = false, class CharIt>
constexpr void mac_to_chars(const uint8_t* mac, CharIt out)
{
    using namespace estd;   // Needs estd >= v0.8.13-alpha2 due to bug in to_chars
    const uint8_t* end = mac + 5;

    auto to_hex = [](CharIt o, uint8_t m) EMBR_NET_INLINE_BASE
    {
        // estd::detail::to_chars operates in reverse.  In other words, it is
        // naturally right justified.
        detail::to_chars_result<CharIt> rc;
        
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

template <class CharIt>
void mac_from_chars(const CharIt in, uint8_t* mac)
{
    auto from_hex = [](const CharIt o) EMBR_NET_INLINE_BASE
    {

    };
}

}