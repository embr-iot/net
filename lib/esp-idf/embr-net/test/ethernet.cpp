#include <embr/net/ethernet.h>

#include <unity.h>

using namespace embr;

TEST_CASE("embr::ethernet", "[ethernet]")
{
    char out[32]{};
    constexpr uint8_t mac1[] { 1, 2, 3, 0x10, 0x20, 0x3a };
    constexpr ethernet::mac mac2 { 0x0A, 0, 0, 0, 0, 0xF1 };

    ethernet::mac_to_chars(mac1, out);

    TEST_ASSERT_EQUAL_STRING("01:02:03:10:20:3a", out);

    ethernet::mac_to_chars<true>(mac1, out);

    TEST_ASSERT_EQUAL_STRING("01:02:03:10:20:3A", out);

    estd::layer1::string<32> s;

    s += ethernet::mac_to_string(mac1);

    TEST_ASSERT_EQUAL_STRING("01:02:03:10:20:3a", s.c_str());

    s = mac_to_string(mac2);

    TEST_ASSERT_EQUAL_STRING("0a:00:00:00:00:f1", s.c_str());

    uint8_t mac3[6];

    estd::errc err = ethernet::mac_from_chars(out, mac3);

    TEST_ASSERT_EQUAL(0, (int)err);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(mac3, mac1, 6);
}