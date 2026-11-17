#include <embr/esp-idf/wifi/fwd.h>

#include <unity.h>

using namespace embr;

TEST_CASE("embr::wifi", "[wifi]")
{
    // Naturally this crashes QEMU, so disabled
    //ESP_ERROR_CHECK(wifi::simple_init());
}