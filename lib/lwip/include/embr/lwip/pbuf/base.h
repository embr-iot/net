#pragma once

#include <lwip/pbuf.h>

#include <estd/utility.h>

namespace embr::lwip::inline _pbuf::inline v1 {

template <bool owning>
class pbuf_base
{
protected:
    pbuf* pbuf_;

    pbuf_base(pbuf* p) : pbuf_{p} {}

    void ref() { pbuf_ref(pbuf_); }

    pbuf_base(const pbuf_base& copy_from) :
        pbuf_{copy_from.pbuf_}
    {
        if(owning)  ref();
    }

public:
    pbuf_base(pbuf_base&& move_from) :
        pbuf_{move_from.pbuf_}
    {
        move_from.pbuf_ = nullptr;
    }

    ~pbuf_base()
    {
        if(owning && pbuf_)   pbuf_free(pbuf_);
    }

    // orly, type not there?
    //pbuf_type type() const { return pbuf_->type; }
    uint16_t length() const { return pbuf_->len; }
    uint16_t total_length() const { return pbuf_->tot_len; }

    unsigned use_count() const { return pbuf_->ref; }

    void* payload() const { return pbuf_->payload; }

    constexpr bool valid() const { return pbuf_; }

    void realloc(uint16_t new_len) const
    {
        pbuf_realloc(pbuf_, new_len);
    }

    uint16_t copy_partial(void* dest, uint16_t len, uint16_t offset) const
    {
        return pbuf_copy_partial(pbuf_, dest, len, offset);
    }

    err_t take(const void* src, uint16_t len) const
    {
        return pbuf_take(pbuf_, src, len);
    }

    uint8_t free()
    {
        uint8_t r = pbuf_free(pbuf_);

        if(owning)  pbuf_ = nullptr;

        return r;
    }

    // EXPERIMENTAL
    template <class F>
    void walk(F&& f)
    {
        for(pbuf* i = pbuf_; i != nullptr; i = i->next)
        {
            f(i->payload, i->len);
        }
    }
};

using owning_pbuf = pbuf_base<true>;

}
