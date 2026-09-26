#pragma once

#include <esp_err.h>

namespace embr::esp_idf::inline net {

esp_err_t simple_flash_init(bool force_erase = false);

}
