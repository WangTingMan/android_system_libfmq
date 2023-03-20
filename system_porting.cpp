#include <fmq\system_porting.h>

#include <windows.h>

#ifndef HAVE_MMAP   /* not true for MSVC, but may be true for msys */
#define MAP_FAILED  0
#define MREMAP_FIXED  2  /* the value in linux, though it doesn't really matter */
/* These, when combined with the mmap invariants below, yield the proper action */
#define PROT_READ      PAGE_READWRITE
#define PROT_WRITE     PAGE_READWRITE
#define MAP_ANONYMOUS  MEM_RESERVE
#define MAP_PRIVATE    MEM_COMMIT
#define MAP_SHARED     MEM_RESERVE   /* value of this #define is 100% arbitrary */
#endif

namespace system_porting
{

int system_porting_munmap( void* addr, size_t length )
{
    return VirtualFree( addr, 0, MEM_RELEASE ) ? 0 : -1;
}

/* VirtualAlloc only replaces for mmap when certain invariants are kept. */
void* system_porting_mmap( void* addr, size_t length, int prot, int flags,
    int fd, long offset )
{
    if( addr == NULL && fd == -1 && offset == 0 &&
        prot == ( PROT_READ | PROT_WRITE ) && flags == ( MAP_PRIVATE | MAP_ANONYMOUS ) )
    {
        return VirtualAlloc( 0, length, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE );
    }
    else
    {
        return NULL;
    }
}

}
