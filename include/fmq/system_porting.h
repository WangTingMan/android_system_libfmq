#pragma once

#include <fmq\fmq_export.h>
#include <stdint.h>
#include <string>

#include <cutils/native_handle.h>
#include <fmq/MQDescriptorBase.h>
#include <hidl/HidlInternal.h>
#include <hidl/HidlSupport.h>

namespace system_porting
{

FMQ_EXPORT int system_porting_munmap( void* addr, size_t length );

FMQ_EXPORT void* system_porting_mmap( void* addr, size_t length, int prot, int flags,
                                      void* fd, long offset );

FMQ_EXPORT void* system_porting_open_shmem( std::string name, bool read_only );

FMQ_EXPORT std::string generate_string
    (
    ::android::hardware::hidl_vec<::android::hardware::GrantorDescriptor> const& a_grantors,
    ::android::hardware::details::hidl_pointer<native_handle_t> const& a_handles,
    uint32_t a_quantum,
    uint32_t a_flags,
    std::string a_name
    );

FMQ_EXPORT void from_string
    (
    std::string const& a_string,
    ::android::hardware::hidl_vec<::android::hardware::GrantorDescriptor>& a_grantors,
    ::android::hardware::details::hidl_pointer<native_handle_t>& a_handles,
    uint32_t& a_quantum,
    uint32_t& a_flags,
    std::string& a_name
    );

FMQ_EXPORT void from_string
    (
    std::string const& a_string,
    std::vector<::android::hardware::GrantorDescriptor>& a_grantors,
    native_handle_t*& a_handles,
    uint32_t& a_quantum,
    uint32_t& a_flags,
    std::string& a_name
    );

FMQ_EXPORT std::string generate_random_name();

}

