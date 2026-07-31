#include <esp_log.h>
#include <nvs_flash.h>

namespace embr::inline _net {

esp_err_t simple_flash_init(bool force_erase)
{
    esp_err_t ret = nvs_flash_init();
    if (force_erase ||
        ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    return ret;
}

}