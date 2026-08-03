#include "wifi-console.h"

#include <embr/esp-idf/wifi/fwd.h>

#include <esp_console.h>
#include <esp_wifi.h>

#include <string>

using string = const std::string_view;

// NOTE: If PGESP-74 ever gets resolved, we can put this out into an embr-net-console
// helper and share it

static embr::console::Args args;

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

esp_err_t arm_start()
{
    if(armed.ap)
    {
        if(armed.sta)
            return esp_wifi_set_mode(WIFI_MODE_APSTA);
        else
            return esp_wifi_set_mode(WIFI_MODE_AP);
    }
    if(armed.sta)
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
            ESP_ERROR_CHECK(arm_start());
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
            ESP_ERROR_CHECK(arm_start());
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
