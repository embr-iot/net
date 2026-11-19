#ifndef FEATURE_EMBR_NET_AGGRESSIVE_INLINE
#define FEATURE_EMBR_NET_AGGRESSIVE_INLINE 1
#endif

#if FEATURE_EMBR_NET_AGGRESSIVE_INLINE
#define EMBR_NET_INLINE_BASE __attribute__((always_inline, optimize("tree-sra,dse")))
#define EMBR_NET_INLINE inline EMBR_NET_INLINE_BASE
#else
#define EMBR_NET_INLINE
#endif

