#include <fmq/system_porting.h>
#include <fmq/MessageQueueBase.h>

#include <base/rand_util.h>
#include <base/strings/sys_string_conversions.h>

#include <direct.h>
#include <windows.h>
#include <log/log.h>
#include <json/json.h>

#include <map>

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
**还是说明一下为什么要在WINDOWS的兼容代码使用HANDLE, 而不是int，
**因为WINDOWs的API大部分都是使用HANDLE，如果每次都要转换，这个，有点违背我对性能的爱好。
*/
typedef HANDLE   ZCE_HANDLE;

typedef unsigned int          mode_t;

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

void* system_porting_open_shmem( std::string a_name, bool read_only )
{
    std::wstring name = base::SysNativeMBToWide( a_name );
    DWORD access = FILE_MAP_READ | SECTION_QUERY;
    if( !read_only )
        access |= FILE_MAP_WRITE;
    HANDLE handle = OpenFileMapping( access, false, name.empty() ? nullptr : name.c_str() );
    DWORD error = GetLastError();
    return handle;
}

/*!
* @brief      将共享内存和文件进行映射
* @return     void* 映射返回的地址，
* @param      addr  准备映射的地址，一般传递nullptr，让OS自己决定
* @param      len   准备映射的长度
* @param      prot  映射的内存区域的操作权限（保护属性）包括PROT_READ、PROT_WRITE、PROT_READ|PROT_WRITE
* @param      flags 标志位参数，包括：MAP_SHARED、MAP_PRIVATE与MAP_ANONYMOUS
* @param      handle用来建立映射区的文件描述符，带入参数是INVALID_HANDLE_VALUE 时，可以使用不用文件映射的共享内存
* @param      off   映射文件的偏移，必须按页面大小对齐，一般是4096
* @note
* code from https://github.com/sailzeng/zcelib.git
*/
void* system_porting_mmap( void* addr,
    size_t len,
    int prot,
    int flags,
    void* a_file_handle,
    long off )
{
    ZCE_HANDLE file_handle = a_file_handle;

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
        ALOGE( "Error, flags = %d. Should be 1?", flags );
        errno = ENOTSUP;
        return MAP_FAILED;
    }

    /*匿名使用，必须文件句柄是无效值*/
    if( ( flags & MAP_ANONYMOUS ) && ZCE_INVALID_HANDLE != file_handle || file_handle == nullptr )
    {
        errno = ENOTSUP;
        return MAP_FAILED;
    }

    /*这段在干嘛，就是将mmap的prot参数转换成微软的的参数，TNND*/
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

    /*如果是私有的，相当于所有人都是副本*/
    if( flags & MAP_PRIVATE )
    {
        //PAGE_WRITECOPY 等价 PAGE_READONLY
        nt_flag_protect |= PAGE_WRITECOPY;
        nt_flags = FILE_MAP_COPY;
    }

    LARGE_INTEGER longlong_value;
    longlong_value.QuadPart = len;

    //file_handle == ZCE_INVALID_HANDLE后，创建的共享内存不在文件里面，而在系统映射文件中 system paging file
    ZCE_HANDLE file_mapping = INVALID_HANDLE_VALUE;
    if( file_handle == INVALID_HANDLE_VALUE )
    {
        file_mapping = ::CreateFileMappingA( file_handle,
                              nullptr,
                              nt_flag_protect,
                              ( file_handle == ZCE_INVALID_HANDLE ) ? longlong_value.HighPart : 0,
                              ( file_handle == ZCE_INVALID_HANDLE ) ? longlong_value.LowPart : 0,
                              nullptr );
    }
    else
    {
        HANDLE duped_handle;
        HANDLE process = GetCurrentProcess();
        BOOL success = ::DuplicateHandle( process, file_handle, process, &duped_handle, 0,
                                          FALSE, DUPLICATE_SAME_ACCESS );

        file_mapping = duped_handle;
    }

    if( file_mapping == 0 )
    {
        return MAP_FAILED;
    }

    longlong_value.QuadPart = off;
    void* addr_mapping = ::MapViewOfFileEx( file_mapping,
        nt_flags,
        longlong_value.HighPart,
        longlong_value.LowPart,
        len,
        addr );

    if( addr_mapping == 0 )
    {
        auto error = GetLastError();
        ALOGE( "Mapping failed with error code: ", error );
        if( error == ERROR_MAPPED_ALIGNMENT )
        {
            SYSTEM_INFO sinf;
            GetSystemInfo( &sinf );
            DWORD dwBytesInBlock = sinf.dwAllocationGranularity;
            /**
             * off的值必须是dwAllocationGranularity的整数倍
             */
            ALOGE(" off = %d, not aligned to %d.", off, dwBytesInBlock );
        }
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

/* 建立某个目录，单层, WINDOWS下，后面那个参数无效 */
int system_porting_shm_mkdir( const char* pathname, mode_t mode )
{
#if defined (ZCE_OS_WINDOWS)
    ZCE_UNUSED_ARG( mode );
    return ::_mkdir( pathname );
#elif defined (ZCE_OS_LINUX)
    return ::mkdir( pathname, mode );
#endif
}

#define GrantorDescriptor_element_flags_key "GrantorDescriptor_flags_key"
#define GrantorDescriptor_element_fdIndex_key "GrantorDescriptor_fdIndex_key"
#define GrantorDescriptor_element_offset_key "GrantorDescriptor_offset_key"
#define GrantorDescriptor_element_extent_key "GrantorDescriptor_extent_key"
#define GrantorDescriptor_size_key "GrantorDescriptor_size_key"
#define GrantorDescriptor_key "GrantorDescriptor_key"

#define quantum_key "quantum_key"
#define flags_key "flags_key"
#define name_key "name_key"

#define native_handle_t_numFds_key "numFds_key"
#define native_handle_t_numInts_key "numInts_key"

std::string generate_string
    (
    ::android::hardware::hidl_vec<::android::hardware::GrantorDescriptor> const& a_grantors,
    ::android::hardware::details::hidl_pointer<native_handle_t> const& a_handles,
    uint32_t a_quantum,
    uint32_t a_flags,
    std::string a_name
    )
{
    std::string json_str;
    try
    {
        Json::Value value;
        Json::StreamWriterBuilder builder;
        builder["indentation"] = " ";

        Json::Value array_value;
        for( int i = 0; i < a_grantors.size(); ++i )
        {
            value.clear();
            value[GrantorDescriptor_element_flags_key] = a_grantors[i].flags;
            value[GrantorDescriptor_element_fdIndex_key] = a_grantors[i].fdIndex;
            value[GrantorDescriptor_element_offset_key] = a_grantors[i].offset;
            value[GrantorDescriptor_element_extent_key] = a_grantors[i].extent;
            array_value[i] = value;
        }

        value.clear();
        value[GrantorDescriptor_size_key] = a_grantors.size();
        value[GrantorDescriptor_key] = array_value;
        value[quantum_key] = Json::Value( uint32_t( a_quantum ) );
        value[flags_key] = Json::Value( uint32_t( a_flags ) );

        if( a_handles )
        {
            value[native_handle_t_numFds_key] = Json::Value( uint32_t( a_handles->numFds ) );
            value[native_handle_t_numInts_key] = Json::Value( uint32_t( a_handles->numInts ) );
        }
        else
        {
            value[native_handle_t_numFds_key] = Json::Value( uint32_t( 0 ) );
            value[native_handle_t_numInts_key] = Json::Value( uint32_t( 0 ) );
        }

        value[name_key] = a_name;
        json_str = Json::writeString( builder, value );
    }
    catch( const std::exception& e)
    {
        ALOGE( "exception caught: %s", e.what() );
    }
    return json_str;
}

void from_string
    (
    std::string const& a_string,
    std::vector<::android::hardware::GrantorDescriptor>& a_grantors,
    native_handle_t*& a_handles,
    uint32_t& a_quantum,
    uint32_t& a_flags,
    std::string& a_name
    )
{
    ::android::hardware::hidl_vec<::android::hardware::GrantorDescriptor> grantors;
    ::android::hardware::details::hidl_pointer<native_handle_t> handle_;
    from_string( a_string, grantors, handle_, a_quantum, a_flags, a_name );
    a_grantors = static_cast<std::vector<::android::hardware::GrantorDescriptor>>( grantors );
    a_handles = static_cast<native_handle_t*>( handle_ );
}

void from_string
    (
    std::string const& a_string,
    ::android::hardware::hidl_vec<::android::hardware::GrantorDescriptor>& a_grantors,
    ::android::hardware::details::hidl_pointer<native_handle_t>& a_handles,
    uint32_t& a_quantum,
    uint32_t& a_flags,
    std::string& a_name
    )
{
    bool ret = false;
    Json::Reader reader;
    Json::Value raw_value;
    int numFds = 0;
    int numInts = 0;
    try
    {
        ret = reader.parse( a_string, raw_value, false );
        if( !ret )
        {
            ALOGE( "Cannot parse from string: %s", a_string.c_str() );
            return;
        }

        Json::Value& value = raw_value[GrantorDescriptor_key];
        if( !value.isArray() && !value.isNull() )
        {
            ALOGE( "Wrong string: %s", a_string.c_str() );
            return;
        }

        uint32_t size = value.size();
        a_grantors.resize( size );
        for( int i = 0; i < size; ++i )
        {
            Json::Value& array_element = value[i];
            a_grantors[i].extent = array_element[GrantorDescriptor_element_extent_key].asUInt64();
            a_grantors[i].fdIndex = array_element[GrantorDescriptor_element_fdIndex_key].asUInt();
            a_grantors[i].offset = array_element[GrantorDescriptor_element_offset_key].asUInt();
            a_grantors[i].flags = array_element[GrantorDescriptor_element_flags_key].asUInt();
        }

        value = raw_value[quantum_key];
        a_quantum = value.asUInt();
        value = raw_value[flags_key];
        a_flags = value.asUInt();

        value = raw_value[native_handle_t_numFds_key];
        numFds = value.asUInt();
        value = raw_value[native_handle_t_numInts_key];
        numInts = value.asUInt();

        value = raw_value[name_key];
        a_name.clear();
        if( !value.isNull() )
        {
            a_name.assign( value.asString() );
        }
    }
    catch( const std::exception& e )
    {
        ALOGE( "exception caught when parse json string: %s", e.what() );
    }

    native_handle_t* handle = native_handle_create( numFds, numInts );
    a_handles = handle;
}

std::string generate_random_name()
{
    std::string name{"random_message_queue_name_"};
    int rand = base::RandInt(0, 10000);
    //name.append(std::to_string(rand));
    return name;
}

}
