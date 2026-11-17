#include <embr/net/ethernet.h>

#include <unity.h>

using namespace embr;

TEST_CASE("embr::ethernet", "[ethernet]")
{
    char out[32]{};
    constexpr uint8_t mac1[] { 1, 2, 3, 0x10, 0x20, 0x30 };

    ethernet::mac_to_chars(mac1, out);

    // FIX: Not ready yet
    TEST_ASSERT_EQUAL_STRING("01:02:03:", out);
}