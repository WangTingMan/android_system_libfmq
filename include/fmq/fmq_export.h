#pragma once

#if defined(WIN32) || defined(_MSC_VER)

#if defined(FMQ_IMPLEMENTATION)
#define FMQ_EXPORT __declspec(dllexport)
#else
#define FMQ_EXPORT __declspec(dllimport)
#endif  // defined(FMQ_IMPLEMENTATION)

#else  // defined(WIN32)
#if defined(FMQ_IMPLEMENTATION)
#define FMQ_EXPORT __attribute__((visibility("default")))
#else
#define FMQ_EXPORT
#endif  // defined(FMQ_IMPLEMENTATION)
#endif


