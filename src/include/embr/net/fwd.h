#pragma once

#include <array>
#include <cstdint>

#ifndef FEATURE_EMBR_NET_AGGRESSIVE_INLINE
#define FEATURE_EMBR_NET_AGGRESSIVE_INLINE 1
#endif

#if FEATURE_EMBR_NET_AGGRESSIVE_INLINE
#define EMBR_NET_INLINE_BASE __attribute__((always_inline, optimize("tree-sra,dse")))
#define EMBR_NET_INLINE inline EMBR_NET_INLINE_BASE
#else
#define EMBR_NET_INLINE
#endif

namespace embr::ethernet {

using mac = std::array<uint8_t, 6>;

namespace addr {

constexpr mac broadcast { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

}

}

namespace embr::wifi {

namespace addr {

constexpr ethernet::mac broadcast = ethernet::addr::broadcast;

}

}