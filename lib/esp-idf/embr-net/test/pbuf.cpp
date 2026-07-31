#include <embr/lwip/shared_pbuf.h>
#include <embr/lwip/pbuf/ostreambuf.h>

#include <unity.h>

using namespace embr;

TEST_CASE("embr::lwip pbuf", "[pbuf]")
{
    auto p = lwip::shared_pbuf::alloc(10);
    lwip::impl::pbuf_ostreambuf test(p);
}