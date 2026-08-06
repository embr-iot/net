#include "wifi-console.h"

#include <embr/esp-idf/wifi/fwd.h>

#include <esp_check.h>
#include <esp_console.h>
#include <esp_wifi.h>
#include <nvs_flash.h>

#include <string>

using string = const std::string_view;

// NOTE: If PGESP-74 ever gets resolved, we can put this out into an embr-net-console
// helper and share it

static embr::console::Args args;

static const char* nvs_ns = "embr:net:con";
static const char* nvs_prefs = "prefs";
static const char* TAG = "embr::net::console";

// TODO: Look into how much this overlaps with esp_wifi_restore
struct
{
    struct
    {
        bool ap : 1;
        bool sta : 1;

    }   autostart;

}   prefs;

static union
{
    struct
    {
        bool ap : 1;
        bool sta : 1;
        bool preinit : 1;
        bool started : 1;   // DEBT: I think we can query wifi stack directly about this

        //char ap_ssid[32];
        //char ap_pass[32];
        //char sta_ssid[32];
        //char sta_pass[32];
    }   armed;

    unsigned armed_raw;
};

esp_err_t arm_mode(wifi_mode_t mode = WIFI_MODE_NULL)
{
    if(armed.ap || mode == WIFI_MODE_AP)
    {
        if(armed.sta)
            return esp_wifi_set_mode(WIFI_MODE_APSTA);
        else
            return esp_wifi_set_mode(WIFI_MODE_AP);
    }
    if(armed.sta || mode == WIFI_MODE_STA)
    {
        return esp_wifi_set_mode(WIFI_MODE_STA);
    }

    return ESP_ERR_INVALID_ARG;
}

esp_err_t arm_stop()
{
    if(!armed.started)  return ESP_OK;

    armed.started = false;   
    return esp_wifi_stop();
}

static void print_prefs()
{
    printf("prefs\n");
    printf("  autostart.ap=%u\n", prefs.autostart.ap);
    printf("  autostart.sta=%u\n", prefs.autostart.sta);
}

static esp_err_t start()
{
    using namespace embr::wifi;

    ESP_RETURN_ON_ERROR(arm_mode(), TAG, "Nothing to config");

    if(armed.ap)
    {
        ESP_ERROR_CHECK(ap_init());
    }
    if(armed.sta)
    {
        ESP_ERROR_CHECK(sta_init());
    }

    ESP_ERROR_CHECK(esp_wifi_start());
    armed.started = true;

    return ESP_OK;
}

// DEBT: Clean up naming preinit is poorly named
esp_err_t lazy_preinit()
{
    if(armed.preinit)   return ESP_OK;

    ESP_RETURN_ON_ERROR(embr::wifi::preinit(), TAG, "preinit() call failed");

    armed.preinit = true;

    return ESP_OK;
}

static int wifi(int argc, char *argv[])
{
    using namespace embr::wifi;

    const int nerrors = arg_parse(argc, argv, (void**)&args);

    if(nerrors) return -1;

    string command = args.command->sval[0];
    string arg1 = args.arg1->sval[0];
    string arg2 = args.arg2->sval[0];

    if(command == "auth")
    {
        if(arg1 == "ssid")
        {
            // TBD
        }
        else if(arg1 == "pass")
        {
            // TBD
        }
        else
            return -1;

        return 0;
    }
    else if(command == "ap")
    {
        if(arg1.empty() || arg1 == "arm")
        {
            ESP_ERROR_CHECK(lazy_preinit());

            armed.ap = true;
        }
        else if(arg1 == "disarm")
        {
            ESP_ERROR_CHECK(arm_stop());

            armed.ap = false;
        }
        else if(arg1 == "start")
        {
            ESP_ERROR_CHECK(lazy_preinit());
            ESP_ERROR_CHECK(arm_mode(WIFI_MODE_AP));
            ESP_ERROR_CHECK(ap_init());
            ESP_ERROR_CHECK(esp_wifi_start());
            armed.started = true;
        }
        else
            return -1;

        return 0;
    }
    else if(command == "autostart")
    {
        // Set autostart of either sta or ap depending on what's running now
        if(armed.started)
        {
            prefs.autostart.sta = armed.sta;
            prefs.autostart.ap = armed.ap;
        }
        else
        {
            // If nothing running, that's our way of clearing out autostart
            prefs.autostart.sta = false;
            prefs.autostart.ap = false;
        }

        nvs_handle_t nvs;
        esp_err_t ret = nvs_open(nvs_ns, NVS_READWRITE, &nvs);

        ESP_RETURN_ON_FALSE(ret == ESP_OK, -1, TAG, "Couldn't open NVS");
        ESP_RETURN_ON_FALSE(
            nvs_set_blob(nvs, nvs_prefs, &prefs, sizeof(prefs)) == ESP_OK, -1, TAG,
            "Couldn't write prefs");

        nvs_close(nvs);
        print_prefs();
        return 0;
    }
    else if(command == "sta")
    {
        if(arg1.empty() || arg1 == "arm")
        {
            ESP_ERROR_CHECK(lazy_preinit());
            armed.sta = true;
            return 0;
        }
        else if(arg1 == "start")
        {
            ESP_ERROR_CHECK(lazy_preinit());

            ESP_ERROR_CHECK(arm_mode(WIFI_MODE_STA));
            ESP_ERROR_CHECK(sta_init());
            ESP_ERROR_CHECK(esp_wifi_start());
            armed.started = true;
            return 0;
        }
    }
    else if(command == "scan")
    {
        // TBD
    }
    else if(command == "start")
    {
        if(armed_raw)
        {
            ESP_ERROR_CHECK(lazy_preinit());
            start();
        }
        else
            // shorthand for "sta start" - if nothing armed, that's our default
            embr::wifi::simple_init();
        return 0;
    }
    else if(command == "status")
    {
        wifi_mode_t mode;
        int rssi;

        puts("status");

        if(armed.started)
        {
            ESP_ERROR_CHECK(esp_wifi_get_mode(&mode));

            printf("  mode: %u\n", mode);
            if(armed.sta)
            {
                ESP_ERROR_CHECK(esp_wifi_sta_get_rssi(&rssi));
                printf("  rssi: %d\n", rssi);
            }
        }
        else
        {
            printf("  not running\n");
        }

        print_prefs();

        // TODO: Look into esp_wifi_statis_dump

        return 0;
    }
    else if(command == "stop")
    {
        ESP_ERROR_CHECK(esp_wifi_stop());
        armed.started = false;
        return 0;
    }

    return -1;
}

esp_err_t wifi_console_init()
{
    nvs_handle_t nvs;
    esp_err_t ret = nvs_open(nvs_ns, NVS_READONLY, &nvs);

    ESP_ERROR_CHECK_WITHOUT_ABORT(ret);

    if(ret == ESP_OK)
    {
        size_t len = sizeof(prefs);
        ret = nvs_get_blob(nvs, nvs_prefs, &prefs, &len);

        if(ret == ESP_OK)
        {
            armed.ap = prefs.autostart.ap;
            armed.sta = prefs.autostart.sta;

            if(armed_raw)
            {
                //arm_start();
            }
        }

        nvs_close(nvs);
    }

    const esp_console_cmd_t cmd
    {
        .command = "wifi",
        .help = "WiFi control",
        .hint = NULL,
        .func = &wifi,
        .argtable = &args,
        .func_w_context = nullptr,
        .context = nullptr
    };

    args.command = arg_str1(nullptr, nullptr,
        "<auth|autostart|sta|ap|start|status|stop>",
        "");
    args.arg1 = arg_strn(nullptr, nullptr,
        "<ssid|pass|start|stop>",
        0, 1,
        "TBD");
    args.arg2 = arg_strn(nullptr, nullptr,
        "<ssid|pass>",
        0, 1,
        "TBD");
    args.end = arg_end(2);
    
    return esp_console_cmd_register(&cmd);
}
