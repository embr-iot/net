#pragma once

#include <array>
#include <cstdint>

namespace embr::ethernet {

using mac = std::array<uint8_t, 6>;

namespace addr {

constexpr mac broadcast { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

}

}
