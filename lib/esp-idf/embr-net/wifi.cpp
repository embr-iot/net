#include "embr/esp-idf/wifi/fwd.h"
#include "embr/esp-idf/wifi/service.h"

#include <esp_log.h>
#include <esp_wifi.h>

#include <cstring>

static const char* TAG = "embr::wifi";

// Safe, GCC behavior value-initializes missing ones
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"

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

esp_idf::wifi::service service;

static void event_handler(void* arg, esp_event_base_t event_base,
    int32_t event_id, void* event_data)
{
    assert(event_base == WIFI_EVENT);

    [[maybe_unused]]
    auto s = (esp_idf::wifi::service*) arg;

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

[[maybe_unused]]
static void ip_event_handler(void* arg, esp_event_base_t event_base,
    int32_t event_id, void* event_data)
{
    assert(event_base == IP_EVENT);

}

void softap_set_dns_addr(esp_netif_t * esp_netif_ap, esp_netif_t* esp_netif_sta)
{
    esp_netif_dns_info_t dns;
    esp_netif_get_dns_info(esp_netif_sta,ESP_NETIF_DNS_MAIN,&dns);
    //uint8_t dhcps_offer_option = DHCPS_OFFER_DNS;
    uint8_t dhcps_offer_option = 0;
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_netif_dhcps_stop(esp_netif_ap));
    ESP_ERROR_CHECK(esp_netif_dhcps_option(esp_netif_ap, ESP_NETIF_OP_SET, ESP_NETIF_DOMAIN_NAME_SERVER, &dhcps_offer_option, sizeof(dhcps_offer_option)));
    ESP_ERROR_CHECK(esp_netif_set_dns_info(esp_netif_ap, ESP_NETIF_DNS_MAIN, &dns));
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_netif_dhcps_start(esp_netif_ap));
}

#define EXAMPLE_ESP_WIFI_CHANNEL 1
#define EXAMPLE_MAX_STA_CONN 4

// Shamelessly adapted from
// https://github.com/espressif/esp-idf/blob/95e1386d5567123d092c8151e3c942e2ec9de6a1/examples/wifi/softap_sta/main/softap_sta.c
esp_err_t ap_init(esp_netif_t** wifi_netif)
{
    esp_netif_t* esp_netif_ap = esp_netif_create_default_wifi_ap();

    wifi_config_t wifi_ap_config
    {
        .ap
        {
            .ssid = CONFIG_EMBR_NET_WIFI_SSID,
            .password = CONFIG_EMBR_NET_WIFI_PASSWORD,
            .ssid_len = strlen(CONFIG_EMBR_NET_WIFI_SSID),
            .channel = EXAMPLE_ESP_WIFI_CHANNEL,
            .authmode = WIFI_AUTH_WPA2_PSK,
            .max_connection = EXAMPLE_MAX_STA_CONN,
            .pmf_cfg
            {
                .required = false,
            },
        },
    };

    if (strlen(CONFIG_EMBR_NET_WIFI_PASSWORD) == 0)
    {
        wifi_ap_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_ap_config));

    ESP_LOGD(TAG, "wifi_init_softap finished. SSID:%s password:%s channel:%d",
        CONFIG_EMBR_NET_WIFI_SSID,
        CONFIG_EMBR_NET_WIFI_PASSWORD,
        EXAMPLE_ESP_WIFI_CHANNEL);

    if(wifi_netif)  *wifi_netif = esp_netif_ap;

    return ESP_OK;
}

esp_err_t sta_init(esp_netif_t** wifi_netif)
{
    esp_netif_t* esp_netif = esp_netif_create_default_wifi_sta();

    if(wifi_netif)  *wifi_netif = esp_netif;

    wifi_config_t wifi_config{};

    strcpy((char*)wifi_config.sta.ssid, CONFIG_EMBR_NET_WIFI_SSID);
    strcpy((char*)wifi_config.sta.password, CONFIG_EMBR_NET_WIFI_PASSWORD);

    ESP_ERROR_CHECK(esp_event_handler_register(
        WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, &service));

    return esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
}

esp_err_t preinit()
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    const wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    return esp_wifi_init(&cfg);
}

esp_err_t simple_init(esp_netif_t** wifi_netif)
{
    ESP_ERROR_CHECK(preinit());

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    ESP_ERROR_CHECK(sta_init(wifi_netif));

    return esp_wifi_start();
}

}
