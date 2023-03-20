#pragma once

#if defined(WIN32) || defined(_MSC_VER)

#if defined(LIBFMQ_IMPLEMENTATION)
#define LIBFMQ_EXPORT __declspec(dllexport)
#else
#define LIBFMQ_EXPORT __declspec(dllimport)
#endif  // defined(LIBFMQ_IMPLEMENTATION)

#else  // defined(WIN32)
#if defined(LIBFMQ_IMPLEMENTATION)
#define LIBFMQ_EXPORT __attribute__((visibility("default")))
#else
#define LIBFMQ_EXPORT
#endif  // defined(LIBFMQ_IMPLEMENTATION)
#endif


