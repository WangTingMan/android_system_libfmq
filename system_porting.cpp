#include <fmq/system_porting.h>
#include <fmq/MessageQueueBase.h>

#include <direct.h>
#include <windows.h>
#include <log/log.h>

#include <map>

std::map<int, HANDLE> s_handle_map;

#define ZCE_OS_WINDOWS 1

/* Changes are private */
#define MAP_PRIVATE     0x02
/* Interpret addr exactly */
#define MAP_FIXED       0x04
/* page can not be accessed */
#define PROT_NONE       0x0
/* page can be read */
#ifndef PROT_READ
#define PROT_READ     0x1
#endif
/* page can be written */
#ifndef PROT_WRITE
#define PROT_WRITE    0x2
#endif
/* page can be executed */
#ifndef PROT_EXEC
#define PROT_EXEC     0x4
#endif

#if !defined (ZCE_INVALID_HANDLE)
# define ZCE_INVALID_HANDLE  INVALID_HANDLE_VALUE
#endif

#if !defined (MAP_FAILED)
#define MAP_FAILED ((void *) -1)
#endif

/* don't use a file */
#define MAP_ANONYMOUS   0x10

#define ZCE_UNUSED_ARG(a)    UNREFERENCED_PARAMETER(a)

/*
**����˵��һ��ΪʲôҪ��WINDOWS�ļ��ݴ���ʹ��HANDLE, ������int��
**��ΪWINDOWs��API�󲿷ֶ���ʹ��HANDLE�����ÿ�ζ�Ҫת����������е�Υ���Ҷ����ܵİ��á�
*/
typedef HANDLE   ZCE_HANDLE;

typedef unsigned int          mode_t;

#define ZCE_POSIX_MMAP_DIRECTORY   "C:\\dev.shm\\"

#ifndef PATH_MAX
#define PATH_MAX 1024
#endif

namespace system_porting
{

/* code from https://github.com/sailzeng/zcelib.git */
int system_porting_munmap( void* addr, size_t len )
{
#if defined (ZCE_OS_WINDOWS)
    ZCE_UNUSED_ARG( len );

    BOOL ret_bool = ::UnmapViewOfFile( addr );

    if( ret_bool == FALSE )
    {
        return -1;
    }

    return 0;
#elif defined (ZCE_OS_LINUX)
    return ::munmap( addr, len );
#endif
}

/*!
* @brief      �������ڴ���ļ�����ӳ��
* @return     void* ӳ�䷵�صĵ�ַ��
* @param      addr  ׼��ӳ��ĵ�ַ��һ�㴫��nullptr����OS�Լ�����
* @param      len   ׼��ӳ��ĳ���
* @param      prot  ӳ����ڴ�����Ĳ���Ȩ�ޣ��������ԣ�����PROT_READ��PROT_WRITE��PROT_READ|PROT_WRITE
* @param      flags ��־λ������������MAP_SHARED��MAP_PRIVATE��MAP_ANONYMOUS
* @param      handle��������ӳ�������ļ������������������INVALID_HANDLE_VALUE ʱ������ʹ�ò����ļ�ӳ��Ĺ����ڴ�
* @param      off   ӳ���ļ���ƫ�ƣ����밴ҳ���С���룬һ����4096
* @note
* code from https://github.com/sailzeng/zcelib.git
*/
void* system_porting_mmap( void* addr,
    size_t len,
    int prot,
    int flags,
    int a_file_handle,
    long off )
{
    ZCE_HANDLE file_handle = INVALID_HANDLE_VALUE;
    if( a_file_handle != 0 && a_file_handle != -1 )
    {
        file_handle = s_handle_map[a_file_handle];
    }

#if defined (ZCE_OS_WINDOWS)

    if( flags & MAP_FIXED )
    {
        addr = 0;
    }

    DWORD nt_flag_protect = 0;
    DWORD  nt_flags = 0;

    // can not map to address 0
    if( ( flags & MAP_FIXED ) && 0 == addr )
    {
        errno = ENOTSUP;
        return MAP_FAILED;
    }

    /*����ʹ�ã������ļ��������Чֵ*/
    if( ( flags & MAP_ANONYMOUS ) && ZCE_INVALID_HANDLE != file_handle || file_handle == nullptr )
    {
        errno = ENOTSUP;
        return MAP_FAILED;
    }

    /*����ڸ�����ǽ�mmap��prot����ת����΢��ĵĲ�����TNND*/
    if( PROT_NONE == prot )
    {
        nt_flag_protect = PAGE_NOACCESS;
    }
    else if( PROT_READ == prot )
    {
        nt_flag_protect = PAGE_READWRITE;
        nt_flags = FILE_MAP_READ;
    }
    else if( ( prot & PROT_READ ) && ( prot & PROT_WRITE ) )
    {
        nt_flag_protect = PAGE_READWRITE;
        nt_flags = FILE_MAP_WRITE;
    }
    else if( PROT_EXEC == prot )
    {
        nt_flag_protect = PAGE_EXECUTE;
        nt_flags = FILE_MAP_EXECUTE;
    }
    else if( ( prot & PROT_EXEC ) && ( prot & PROT_READ ) )
    {
        nt_flag_protect = PAGE_EXECUTE_READ;
        nt_flags = FILE_MAP_EXECUTE | FILE_MAP_READ;
    }
    else if( ( prot & PROT_EXEC ) && ( prot & PROT_READ ) && ( prot & PROT_WRITE ) )
    {
        nt_flag_protect = PAGE_EXECUTE_READWRITE;
        nt_flags = FILE_MAP_EXECUTE | FILE_MAP_WRITE;
    }
    else
    {
        errno = EOPNOTSUPP;
        return MAP_FAILED;
    }

    /*�����˽�еģ��൱�������˶��Ǹ���*/
    if( flags & MAP_PRIVATE )
    {
        //PAGE_WRITECOPY �ȼ� PAGE_READONLY
        nt_flag_protect |= PAGE_WRITECOPY;
        nt_flags = FILE_MAP_COPY;
    }

    LARGE_INTEGER longlong_value;
    longlong_value.QuadPart = len;

    //file_handle == ZCE_INVALID_HANDLE�󣬴����Ĺ����ڴ治���ļ����棬����ϵͳӳ���ļ��� system paging file
    ZCE_HANDLE file_mapping = ::CreateFileMappingA( file_handle,
        nullptr,
        nt_flag_protect,
        ( file_handle == ZCE_INVALID_HANDLE ) ? longlong_value.HighPart : 0,
        ( file_handle == ZCE_INVALID_HANDLE ) ? longlong_value.LowPart : 0,
        nullptr );

    if( file_mapping == 0 )
    {
        return MAP_FAILED;
    }

    //
    longlong_value.QuadPart = off;
    void* addr_mapping = ::MapViewOfFileEx( file_mapping,
        nt_flags,
        longlong_value.HighPart,
        longlong_value.LowPart,
        len,
        addr );

    // Only close this down if we used the temporary.
    ::CloseHandle( file_mapping );

    if( addr_mapping == 0 )
    {
        return MAP_FAILED;
    }
    else
    {
        return addr_mapping;
    }

#elif defined (ZCE_OS_LINUX)
    //
    return ::mmap( addr,
        len,
        prot,
        flags,
        file_handle,
        off );

#endif
}

/* ����ĳ��Ŀ¼������, WINDOWS�£������Ǹ�������Ч */
int system_porting_shm_mkdir( const char* pathname, mode_t mode )
{
#if defined (ZCE_OS_WINDOWS)
    ZCE_UNUSED_ARG( mode );
    return ::_mkdir( pathname );
#elif defined (ZCE_OS_LINUX)
    return ::mkdir( pathname, mode );
#endif
}

}
