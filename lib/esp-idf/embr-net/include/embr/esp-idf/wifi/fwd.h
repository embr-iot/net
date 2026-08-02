#pragma once

#include <esp_wifi.h>

const char* to_string(wifi_event_t event_id);
const char* to_string(wifi_err_reason_t reason);

// DEBT: Do some more thinking about how explicit we need esp_idf to be here
// (will we have general purpose wifi things shared with stuff like RPI 0 W?,
// and will it sizeably be different from the outside - black box- ?)
namespace embr::esp_idf::wifi {

class service;

}

namespace embr::wifi {

// Low level calls, needs assistance
esp_err_t preinit();
esp_err_t ap_init(esp_netif_t** wifi_netif = nullptr);
esp_err_t sta_init(esp_netif_t** wifi_netif = nullptr);

// Assumes STA mode
esp_err_t simple_init(esp_netif_t** wifi_netif = nullptr);

}