#pragma once

#include "fwd.h"

#include <estd/charconv.h>

namespace embr::ethernet {

// DEBT: Consider a concat-helper for these special first/last cases.  I've done
// it a billion times before yet can't remember where any of them live now.

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
estd::errc mac_from_chars(CharIt in, uint8_t* mac)
{
    using namespace estd;

    const uint8_t* end = mac + 5;

    auto from_hex = [](CharIt o, uint8_t* out) EMBR_NET_INLINE_BASE
    {
        // DEBT: estd needs better documentation on 'sto_mode' and also
        // we'd prefer a detail:: flavor of this if we could
        detail::from_chars_result<CharIt> r = estd::internal::from_chars_integer<16>(o, o + 2, *out);

        return r.ec;
    };
    

    for(; mac < end; ++mac, in += 3)
    {
        errc err = from_hex(in, mac);
        
        if(err != errc{})  return err;

        if(*(in + 2) != ':')    return errc::invalid_argument;
    }

    return from_hex(in, mac);
}

}