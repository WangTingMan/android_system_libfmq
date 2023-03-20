#pragma once

#include <fmq\fmq_export.h>
#include <stdint.h>

namespace system_porting
{

FMQ_EXPORT int system_porting_munmap( void* addr, size_t length );

FMQ_EXPORT void* system_porting_mmap( void* addr, size_t length, int prot, int flags,
    int fd, long offset );

}

