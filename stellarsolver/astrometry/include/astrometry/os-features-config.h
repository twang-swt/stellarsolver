#pragma once

#if defined(_MSC_VER) || defined(__ANDROID__)
    #define NEED_DECLARE_QSORT_R 1
    #define NEED_QSORT_R 1
    #define NEED_SWAP_QSORT_R 0
#else
    #define NEED_DECLARE_QSORT_R 0
    #define NEED_QSORT_R 1
    #define NEED_SWAP_QSORT_R 0
#endif

#define HAVE_NETPBM 0
