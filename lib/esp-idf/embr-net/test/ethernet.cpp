#include <embr/net/ethernet.h>

#include <unity.h>

using namespace embr;

TEST_CASE("embr::ethernet", "[ethernet]")
{
    char out[32]{};
    constexpr uint8_t mac1[] { 1, 2, 3, 0x10, 0x20, 0x3a };

    ethernet::mac_to_chars(mac1, out);

    TEST_ASSERT_EQUAL_STRING("01:02:03:10:20:3a", out);

    ethernet::mac_to_chars<true>(mac1, out);

    TEST_ASSERT_EQUAL_STRING("01:02:03:10:20:3A", out);
}