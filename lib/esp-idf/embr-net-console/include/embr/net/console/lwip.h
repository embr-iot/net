#pragma once

#include "args.h"

#include <esp_err.h>

namespace embr::inline net {

esp_err_t lwip_udp_console_init();

}
