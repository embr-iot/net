#pragma once

#include "pbuf/base.h"
#include "pbuf/fwd.h"

namespace embr::lwip::inline _pbuf::inline v1 {

namespace mixin {

template <class Derived>
class pbuf_factory
{
    // IDEA: We might extend pbuf_base with pbuf_base<owning, temp> where
    // temp acts like a non-owning but MUST get consumed by an owning flavor
    // thus removing the need to null out pbuf_.  Alternatively, optimizer
    // might just wipe that away anyway in which case we shouldn't bother.
public:
    static Derived alloc(pbuf_layer layer, uint16_t length, pbuf_type type)
    {
        return { pbuf_alloc(layer, length, type) };
    }

    static Derived alloc(uint16_t length, pbuf_type type = PBUF_RAM)
    {
        return { pbuf_alloc(PBUF_TRANSPORT, length, type) };
    }

    // 03AUG26 MB - UNTESTED
    static Derived alloced(
        pbuf_layer layer, uint16_t length, pbuf_type type,
        pbuf_custom* p, void* payload_mem, uint16_t payload_size)
    {
        return { pbuf_alloced_custom(
            layer, length, type,
            p, payload_mem, payload_size) };
    }
};

}

// EXPERIMENTAL
class unique_pbuf : public owning_pbuf,
    public mixin::pbuf_factory<unique_pbuf>
{
    using base_type = owning_pbuf;

    template <class>
    friend class mixin::pbuf_factory;

protected:
    unique_pbuf(pbuf* p) : base_type{p} {}

public:
    unique_pbuf() = delete;
    unique_pbuf(const unique_pbuf&) = delete;
    unique_pbuf(unique_pbuf&& move_from) :
        base_type(std::move(move_from))
    {}
};

class shared_pbuf : public owning_pbuf,
    public mixin::pbuf_factory<shared_pbuf>
{
    using base_type = owning_pbuf;

    friend class weak_pbuf;

    template <class>
    friend class mixin::pbuf_factory;

protected:
    shared_pbuf(pbuf* p) : base_type{p} {}

public:
    static shared_pbuf take_ownership(pbuf* p)
    {
        return { p };
    }

    // EXPERIMENTAL
    static shared_pbuf move(pbuf*& p)
    {
        shared_pbuf created{ p };

        p = nullptr;

        return created;
    }

    // Perhaps a default-value-init constructor would be OK here, just like
    // shared_ptr
    shared_pbuf() = delete;

    // Takes either a shared_pbuf or unique_pbuf
    shared_pbuf(const owning_pbuf& copy_from) :
        base_type{copy_from}
    {
    }

    // Takes either a shared_pbuf or unique_pbuf
    shared_pbuf(owning_pbuf&& move_from) :
        base_type{std::move(move_from)}
    {
    }

    operator pbuf*() const { return pbuf_; }
};

// EXPERIMENTAL
class weak_pbuf : public pbuf_base<false>
{
    using base_type = pbuf_base<false>;

public:
    weak_pbuf(const shared_pbuf& copy_from) :
        base_type(copy_from)
    {

    }

    shared_pbuf lock()
    {
        // FIX: Not ready - I think we need to bump ref here, but not bump ref if
        // pbuf_->ref is 0?  But if pbuf_->ref is 0, pbuf_ itself might be an invalid pointer
        return shared_pbuf{pbuf_};
    }
};

}

// DEBT: Getting there with naming.  This isn't truly taking ownership of pbuf though.
// It's merely creating a shared_pbuf without bumping the ref ptr.  Still, it's more
// clear than most of the other approaches I've taken
inline embr::lwip::_pbuf::v1::shared_pbuf take_ownership(pbuf* p)
{
    return embr::lwip::_pbuf::v1::shared_pbuf::take_ownership(p);
}

inline embr::lwip::_pbuf::v1::shared_pbuf move(pbuf* p)
{
    return embr::lwip::_pbuf::v1::shared_pbuf::take_ownership(p);
}

template <class ...Args>
inline embr::lwip::_pbuf::v1::unique_pbuf make_unique_pbuf(Args&&...args)
{
    return embr::lwip::_pbuf::v1::unique_pbuf::alloc(std::forward<Args>(args)...);
}
