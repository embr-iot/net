#include "embr/esp-idf/wifi/fwd.h"

#include <esp_check.h>
#include <esp_log.h>
#include <esp_now.h>
#include <esp_wifi.h>

#include <cstring>

static const char* TAG = "embr::esp-now";

namespace embr::esp_idf::esp_now {

// Guidance from
// https://github.com/espressif/esp-idf/blob/e4df0c12f70daf0a7958e586e223c519fa9a1576/examples/wifi/espnow/main/espnow_example_main.c
esp_err_t init(int channel, wifi_mode_t mode)
{
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_RETURN_ON_ERROR(esp_wifi_set_mode(mode),
        TAG, "WiFi mode set failed");

    // esp-now has this peculiar need to start wifi before setting the channel
    ESP_RETURN_ON_ERROR(esp_wifi_start(), TAG, "WiFi start failed");
    ESP_RETURN_ON_ERROR(esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE),
        TAG, "Set channel failed");

#if CONFIG_ESPNOW_ENABLE_LONG_RANGE
    ESP_ERROR_CHECK(
        esp_wifi_set_protocol(
            wifi_if,
            WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N|WIFI_PROTOCOL_LR));
#endif

    return esp_now_init();
}

esp_err_t add_broadcast_peer(wifi_interface_t wifi_if)
{
    esp_now_peer_info_t peer{};
    //peer->channel = CONFIG_ESPNOW_CHANNEL;
    peer.ifidx = wifi_if;
    memcpy(peer.peer_addr, ethernet::addr::broadcast.data(), ESP_NOW_ETH_ALEN);
    return esp_now_add_peer(&peer);
}

}
