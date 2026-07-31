#include <esp_wifi.h>

const char* to_string(wifi_event_t event_id);
const char* to_string(wifi_err_reason_t reason);

namespace embr::wifi {

esp_err_t simple_init(esp_netif_t** wifi_netif = nullptr);

}