#include "wifi-console.h"

#include <embr/esp-idf/wifi/fwd.h>

#include <esp_console.h>
#include <esp_wifi.h>
#include <nvs_flash.h>

#include <string>

using string = const std::string_view;

// NOTE: If PGESP-74 ever gets resolved, we can put this out into an embr-net-console
// helper and share it

static embr::console::Args args;

static const char* nvs_ns = "embr:net:con";

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
        bool started : 1;   // DEBT: I think we can query wifi stack directly about this
    }   armed;

    unsigned armed_raw;
};

esp_err_t arm_start(wifi_mode_t mode = WIFI_MODE_NULL)
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

static int wifi(int argc, char *argv[])
{
    using namespace embr::wifi;

    const int nerrors = arg_parse(argc, argv, (void**)&args);

    if(nerrors) return -1;

    string command = args.command->sval[0];
    string arg1 = args.arg1->sval[0];

    nvs_handle_t nvs;
    esp_err_t ret = nvs_open(nvs_ns, NVS_READWRITE, &nvs);

    ESP_ERROR_CHECK_WITHOUT_ABORT(ret);

    if(ret == ESP_OK)   nvs_close(nvs);

    if(command == "ap")
    {
        if(arg1.empty() || arg1 == "arm")
        {
            if(!armed_raw)  ESP_ERROR_CHECK(preinit());

            armed.ap = true;
        }
        else if(arg1 == "disarm")
        {
            ESP_ERROR_CHECK(arm_stop());

            armed.ap = false;
        }
        else if(arg1 == "start")
        {
            if(!armed_raw)  ESP_ERROR_CHECK(preinit());

            ESP_ERROR_CHECK(arm_start(WIFI_MODE_AP));
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
        // Toggle autostart of either sta or ap depending on what's running now
    }
    else if(command == "sta")
    {
        if(arg1.empty() || arg1 == "arm")
        {
            if(!armed_raw)  ESP_ERROR_CHECK(preinit());
            armed.sta = true;
            return 0;
        }
        else if(arg1 == "start")
        {
            if(!armed_raw)  ESP_ERROR_CHECK(preinit());

            ESP_ERROR_CHECK(arm_start(WIFI_MODE_STA));
            ESP_ERROR_CHECK(sta_init());
            ESP_ERROR_CHECK(esp_wifi_start());
            armed.started = true;
            return 0;
        }
    }
    else if(command == "start")
    {
        // shorthand for "sta start"
        embr::wifi::simple_init();
        return 0;
    }
    else if(command == "stop")
    {
        ESP_ERROR_CHECK(esp_wifi_stop());
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
        ret = nvs_get_blob(nvs, "prefs", &prefs, &len);
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
        "<cred|sta|ap|start|stop>",
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
