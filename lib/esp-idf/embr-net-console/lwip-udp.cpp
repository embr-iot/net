#include "embr/net/console/lwip.h"

#include <embr/lwip/shared_pbuf.h>

#include <lwip/udp.h>

#include <esp_console.h>
#include <esp_check.h>
#include <esp_log.h>

#include <string>

using string = std::string_view;

// NOTE: If PGESP-74 ever gets resolved, we can put this out into an embr-net-console
// helper and share it

namespace embr::inline net {

static const char* TAG = "embr::net::console::lwip";

static console::Args args;

udp_pcb* pcb;
udp_pcb* send_pcb;

static void udp_echoback_recv(void* arg, 
    udp_pcb* pcb, pbuf* p,
    const ip_addr_t* addr, u16_t port)
{
    lwip::shared_pbuf buf(move(p));

    ESP_LOGI(TAG, "udp_echoback_recv: %.*s",
        buf.total_length(),
        (const char*)buf.payload());
}

static void udp_echo_recv(void* arg, 
    udp_pcb* pcb, pbuf* p,
    const ip_addr_t* addr, u16_t port)
{
    lwip::shared_pbuf buf(move(p));

    ESP_LOGI(TAG, "udp_echo_recv: %.*s port=%u",
        buf.total_length(),
        (const char*)buf.payload(),
        (unsigned)port);

    [[maybe_unused]]
    err_t err = udp_sendto(pcb, buf, addr, port);
}

static int lwip_udp(int argc, char *argv[])
{
    const int nerrors = arg_parse(argc, argv, (void**)&args);

    if(nerrors) return -1;

    string command = args.command->sval[0];
    string arg1 = args.arg1->sval[0];
    err_t err;

    if(command == "broadcast")
    {
        if(arg1.empty())    arg1 = "hello";

        auto buf = lwip::shared_pbuf::alloc(arg1.size());

        ESP_RETURN_ON_FALSE(buf.valid(), ESP_ERR_INVALID_ARG, TAG, "pbuf alloc failed");

        //ip_addr dest;
        //ip_addr_set_zero(&dest);

        //if(arg1.empty())
        err = buf.take(arg1.data(), arg1.size());

        ESP_RETURN_ON_FALSE(err == ERR_OK, ESP_ERR_INVALID_ARG, TAG, "take call failed");

        err = udp_sendto(send_pcb, buf, &ip_addr_broadcast, 7);

        return err == ERR_OK ? 0 : -1;
    }

    return -1;
}

esp_err_t lwip_udp_console_init()
{
    const esp_console_cmd_t cmd
    {
#if FULL_NAME
        .command = "lwip-udp",
#else
        .command = "udp",
#endif
        .help = "LwIP UDP tests",
        .hint = NULL,
        .func = &lwip_udp,
        .argtable = &args,
        .func_w_context = nullptr,
        .context = nullptr
    };

    args.command = arg_str1(nullptr, nullptr,
        "<broadcast>",
        "");
    args.arg1 = arg_strn(nullptr, nullptr,
        "<TBD>",
        0, 1,
        "TBD");
    args.arg2 = arg_strn(nullptr, nullptr,
        "<TBD>",
        0, 1,
        "TBD");
    args.end = arg_end(2);

    pcb = udp_new();
    send_pcb = udp_new();
    assert(pcb);

    // https://datatracker.ietf.org/doc/html/rfc862
    err_t err = udp_bind(pcb, IP_ADDR_ANY, 7);

    assert(err == ERR_OK);

    err = udp_bind(send_pcb, IP_ADDR_ANY, 20000);

    assert(err == ERR_OK);

    udp_recv(pcb, udp_echo_recv, nullptr);    
    udp_recv(send_pcb, udp_echoback_recv, nullptr);    
    
    return esp_console_cmd_register(&cmd);
}

}