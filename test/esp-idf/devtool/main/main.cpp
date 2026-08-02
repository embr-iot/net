#include <embr/esp-idf/net/fwd.h>

#include <console_simple_init.h>

#include <argtable3/argtable3.h>
#include <esp_console.h>

#include <string>

using namespace embr;

using string = const std::string_view;

struct Args
{
    struct arg_str* command;
    struct arg_str* arg1;
    struct arg_str* arg2;
    struct arg_end* end;
};

static Args args;

static int sys(int argc, char *argv[])
{
    const int nerrors = arg_parse(argc, argv, (void**)&args);

    if(nerrors) return -1;

    string command = args.command->sval[0];

    if(command == "hi") {}

    return -1;
}

esp_err_t sys_console_init(void)
{
    const esp_console_cmd_t cmd
    {
        .command = "sys",
        .help = "System control",
        .hint = NULL,
        .func = &sys,
        .argtable = &args,
        .func_w_context = nullptr,
        .context = nullptr
    };

    // TODO: Put in restart, meminfo and maybe sleep
    args.command = arg_str1(nullptr, nullptr,
        "<test>",
        "");
    args.arg1 = arg_strn(nullptr, nullptr,
        "<arg1>",
        0, 1,
        "TBD");
    args.arg2 = arg_strn(nullptr, nullptr,
        "<arg2>",
        0, 1,
        "TBD");
    args.end = arg_end(2);
    
    return esp_console_cmd_register(&cmd);
}

extern "C" void app_main(void)
{
    ESP_ERROR_CHECK(simple_flash_init());

    ESP_ERROR_CHECK(console_cmd_init());

    ESP_ERROR_CHECK(sys_console_init());

    ESP_ERROR_CHECK(console_cmd_start());
}
