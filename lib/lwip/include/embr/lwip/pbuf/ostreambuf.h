#pragma once

#include "fwd.h"
#include "../shared_pbuf.h"

#include <estd/streambuf.h>

namespace embr::lwip::inline v1 {

namespace impl {

class pbuf_streambuf
{
    shared_pbuf pbuf_;

public:
    pbuf_streambuf(shared_pbuf&& pbuf) :
        pbuf_(std::move(pbuf))
    {

    }

    pbuf_streambuf(const shared_pbuf& pbuf) :
        pbuf_(pbuf)
    {

    }

    shared_pbuf pbuf() { return pbuf_; }
    const shared_pbuf& pbuf() const { return pbuf_; }
};

using pbuf_ostreambuf = pbuf_streambuf;

}

}
