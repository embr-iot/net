#pragma once

#include "args.h"

#include <esp_err.h>

namespace embr::esp_idf {

esp_err_t lwip_udp_console_init();

}
