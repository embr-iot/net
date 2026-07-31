#include "embr/esp-idf/wifi/fwd.h"

#include <esp_log.h>
#include <esp_wifi.h>

#include <cstring>

static const char* TAG = "embr::wifi";

#define CASE(x) case x: return #x;

const char* to_string(wifi_event_t event_id)
{
    switch(event_id)
    {
        CASE(WIFI_EVENT_WIFI_READY)
        CASE(WIFI_EVENT_SCAN_DONE)
        CASE(WIFI_EVENT_STA_START)
        CASE(WIFI_EVENT_STA_STOP)
        CASE(WIFI_EVENT_STA_CONNECTED)
        CASE(WIFI_EVENT_STA_DISCONNECTED)
        CASE(WIFI_EVENT_STA_WPS_ER_SUCCESS)
        CASE(WIFI_EVENT_AP_START)
        CASE(WIFI_EVENT_AP_STOP)
        CASE(WIFI_EVENT_AP_STACONNECTED)
        CASE(WIFI_EVENT_AP_WPS_RG_SUCCESS)
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(6, 0, 0)
        CASE(WIFI_EVENT_NAN_SYNC_STARTED)
#else
        CASE(WIFI_EVENT_NAN_STARTED)
#endif
        CASE(WIFI_EVENT_NDP_INDICATION)
        CASE(WIFI_EVENT_HOME_CHANNEL_CHANGE)
        CASE(WIFI_EVENT_AP_WRONG_PASSWORD)
        default: return "N/A";
    }
}

const char* to_string(wifi_err_reason_t reason)
{
    switch(reason)
    {
        CASE(WIFI_REASON_UNSPECIFIED)
        CASE(WIFI_REASON_AUTH_EXPIRE)
        CASE(WIFI_REASON_AUTH_LEAVE)
        CASE(WIFI_REASON_DISASSOC_DUE_TO_INACTIVITY)
        CASE(WIFI_REASON_CLASS2_FRAME_FROM_NONAUTH_STA)
        CASE(WIFI_REASON_CLASS3_FRAME_FROM_NONASSOC_STA)
        CASE(WIFI_REASON_TIMEOUT)
        CASE(WIFI_REASON_PEER_INITIATED)
        CASE(WIFI_REASON_AP_INITIATED)
        CASE(WIFI_REASON_NO_AP_FOUND)
        CASE(WIFI_REASON_AUTH_FAIL)
        CASE(WIFI_REASON_ASSOC_FAIL)
        CASE(WIFI_REASON_HANDSHAKE_TIMEOUT)
        default: return "N/A";
    }
}

namespace embr::wifi {

static void event_handler(void* arg, esp_event_base_t event_base,
    int32_t event_id, void* event_data)
{
    assert(event_base == WIFI_EVENT);

    const char* event_str = to_string((wifi_event_t)event_id);
    ESP_LOGV(TAG, "event_id=%s (%ld)", event_str, event_id);

    switch(event_id)
    {
        case WIFI_EVENT_STA_START:
            esp_wifi_connect();
            break;

        case WIFI_EVENT_STA_DISCONNECTED:
        {
            auto data = (const wifi_event_sta_disconnected_t*)event_data;
            auto reason = static_cast<wifi_err_reason_t>(data->reason);
            ESP_LOGD(TAG, "WIFI_EVENT_STA_DISCONNECTED: ssid=%.*s reason=%s rssi=%d",
                data->ssid_len,
                data->ssid,
                to_string(reason),
                data->rssi);
            break;
        }

        default:
            break;
    }
}

esp_err_t simple_init(esp_netif_t** wifi_netif)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_t* esp_netif = esp_netif_create_default_wifi_sta();

    if(wifi_netif)  *wifi_netif = esp_netif;

    const wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    wifi_config_t wifi_config{};

    strcpy((char*)wifi_config.sta.ssid, CONFIG_EMBR_NET_WIFI_SSID);
    strcpy((char*)wifi_config.sta.password, CONFIG_EMBR_NET_WIFI_PASSWORD);

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));

    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    return esp_wifi_start();
}

}
