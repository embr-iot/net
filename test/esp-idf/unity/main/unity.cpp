#include <unity.h>

#include <embr/net/fwd.h>
#include <embr/esp-idf/net/fwd.h>

extern "C" void app_main(void)
{
    embr::esp_idf::simple_flash_init();

    UNITY_BEGIN();
    unity_run_all_tests();
    UNITY_END();
}
