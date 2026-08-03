#include "embr/net/console/lwip.h"

#include <embr/lwip/shared_pbuf.h>

#include <lwip/udp.h>

#include <esp_console.h>

#include <string>

using string = const std::string_view;

// NOTE: If PGESP-74 ever gets resolved, we can put this out into an embr-net-console
// helper and share it

namespace embr::inline net {

static console::Args args;

udp_pcb* pcb;

static void udp_recv(void* arg, 
    udp_pcb* pcb, pbuf* p,
    const ip_addr_t* addr, u16_t port)
{

}

static int lwip_udp(int argc, char *argv[])
{
    const int nerrors = arg_parse(argc, argv, (void**)&args);

    if(nerrors) return -1;

    string command = args.command->sval[0];
    string arg1 = args.arg1->sval[0];

    if(command == "broadcast")
    {

    }

    return -1;
}

esp_err_t lwip_udp_console_init()
{
    const esp_console_cmd_t cmd
    {
        .command = "lwip-udp",
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

    udp_pcb* pcb = udp_new();
    assert(pcb);

    // https://datatracker.ietf.org/doc/html/rfc862
    err_t err = udp_bind(pcb, IP_ADDR_ANY, 7);

    assert(err == ERR_OK);

    udp_recv(pcb, udp_recv, nullptr);    
    
    return esp_console_cmd_register(&cmd);
}

}