#pragma once

#include <argtable3/argtable3.h>

namespace embr::inline net::console {

struct Args
{
    struct arg_str* command;
    struct arg_str* arg1;
    struct arg_str* arg2;
    struct arg_end* end;
};

}
