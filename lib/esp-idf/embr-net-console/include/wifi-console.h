#pragma once

#include <argtable3/argtable3.h>
#include <esp_err.h>

struct Args
{
    struct arg_str* command;
    struct arg_str* arg1;
    struct arg_str* arg2;
    struct arg_end* end;
};

esp_err_t wifi_console_init();
