#include <unity.h>

#include <embr/esp-idf/net/fwd.h>

extern "C" void app_main(void)
{
    embr::simple_flash_init();

    UNITY_BEGIN();
    unity_run_all_tests();
    UNITY_END();
}
