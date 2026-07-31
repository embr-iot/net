#include <unity.h>

// DEBT: Too generic of a location
#include <embr/esp-idf/fwd.h>

extern "C" void app_main(void)
{
    embr::simple_flash_init();

    UNITY_BEGIN();
    unity_run_all_tests();
    UNITY_END();
}
