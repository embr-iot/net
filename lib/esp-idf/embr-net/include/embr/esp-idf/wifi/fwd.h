#pragma once

#include "embr/net/ethernet.h"

#include <esp_wifi.h>

const char* to_string(wifi_event_t event_id);
const char* to_string(wifi_err_reason_t reason);

// DEBT: Do some more thinking about how explicit we need esp_idf to be here
// (will we have general purpose wifi things shared with stuff like RPI 0 W?,
// and will it sizeably be different from the outside - black box- ?)
namespace embr::esp_idf::wifi {

class service;

inline ethernet::mac get_mac(wifi_interface_t interface = WIFI_IF_STA)
{
    ethernet::mac mac;

    ESP_ERROR_CHECK(esp_wifi_get_mac(interface, mac.data()));

    return mac; // RVO we're relying on you buddy
}

}

namespace embr::esp_idf::esp_now {

esp_err_t add_broadcast_peer(wifi_interface_t = WIFI_IF_STA);
esp_err_t init(int channel, wifi_mode_t = WIFI_MODE_STA);

}

// EXPERIMENTAL
namespace embr::inline idf {
using namespace embr::esp_idf;
}

namespace embr::wifi {

using namespace embr::ethernet;

// Low level calls, needs assistance
esp_err_t preinit(bool strict = false);
esp_err_t ap_init(esp_netif_t** wifi_netif = nullptr);
esp_err_t sta_init(esp_netif_t** wifi_netif = nullptr);

// Assumes STA mode
esp_err_t simple_init(esp_netif_t** wifi_netif = nullptr);

}
