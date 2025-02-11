/*
 * Copyright (C) 2020 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
<<<<<<< HEAD

#ifndef _CRT_NONSTDC_NO_WARNINGS
#define _CRT_NONSTDC_NO_WARNINGS
#endif

#include <aidl/android/hardware/common/fmq/MQDescriptor.h>
=======
#pragma once
>>>>>>> 208ef36
#include <cutils/native_handle.h>
#include <limits>
#include <type_traits>

<<<<<<< HEAD
#ifdef _MSC_VER
#include <corecrt_io.h>
#endif
=======
#include <aidl/android/hardware/common/fmq/MQDescriptor.h>
#include <fmq/MQDescriptorBase.h>
#include "AidlMQDescriptorShimBase.h"
>>>>>>> 208ef36

namespace android {
namespace details {
using aidl::android::hardware::common::fmq::MQDescriptor;
using aidl::android::hardware::common::fmq::SynchronizedReadWrite;
using aidl::android::hardware::common::fmq::UnsynchronizedWrite;
using android::hardware::MQFlavor;

struct BackendTypesStore {
    template <typename T, typename flavor>
    using MQDescriptorType = aidl::android::hardware::common::fmq::MQDescriptor<T, flavor>;
    using SynchronizedReadWriteType = aidl::android::hardware::common::fmq::SynchronizedReadWrite;
    using UnsynchronizedWriteType = aidl::android::hardware::common::fmq::UnsynchronizedWrite;
};

template <typename T, MQFlavor flavor>
struct AidlMQDescriptorShim : public AidlMQDescriptorShimBase<T, flavor, BackendTypesStore> {
    // Takes ownership of handle
    AidlMQDescriptorShim(const std::vector<android::hardware::GrantorDescriptor>& grantors,
                         native_handle_t* nHandle, size_t size);

    // Takes ownership of handle
    AidlMQDescriptorShim(
            const MQDescriptor<
                    T, typename std::conditional<flavor == hardware::kSynchronizedReadWrite,
                                                 SynchronizedReadWrite, UnsynchronizedWrite>::type>&
                    desc);

    // Takes ownership of handle
    AidlMQDescriptorShim(size_t bufferSize, native_handle_t* nHandle, size_t messageSize,
                         bool configureEventFlag = false);

    explicit AidlMQDescriptorShim(const AidlMQDescriptorShim& other)
        : AidlMQDescriptorShim(0, nullptr, 0) {
        *this = other;
    }
<<<<<<< HEAD
    AidlMQDescriptorShim& operator=(const AidlMQDescriptorShim& other);

    ~AidlMQDescriptorShim();

#ifdef _MSC_VER
    std::string toString()const
    {
        std::string str;
        str = ::system_porting::generate_string( mGrantors, mHandle, mQuantum, mFlags, mName );
        return str;
    }

    void fromString( std::string const& a_decriptor_str )
    {
        ::system_porting::from_string( a_decriptor_str, mGrantors, mHandle, mQuantum, mFlags, mName );
    }

    void setName( std::string const& a_name )
    {
        mName = a_name;
    }

    std::string const& getName()const noexcept
    {
        return mName;
    }
#endif

    size_t getSize() const;

    size_t getQuantum() const;

    uint32_t getFlags() const;

    bool isHandleValid() const { return mHandle != nullptr; }
    size_t countGrantors() const { return mGrantors.size(); }

    inline const std::vector<android::hardware::GrantorDescriptor>& grantors() const {
        return mGrantors;
    }

    inline const ::native_handle_t* handle() const { return mHandle; }

    inline ::native_handle_t* handle() { return mHandle; }

    static const size_t kOffsetOfGrantors;
    static const size_t kOffsetOfHandle;

  private:
    std::vector<android::hardware::GrantorDescriptor> mGrantors;
    native_handle_t* mHandle = nullptr;
    uint32_t mQuantum = 0;
    uint32_t mFlags = 0;
#ifdef _MSC_VER
    std::string mName;
#endif
=======
>>>>>>> 208ef36
};

template <typename T, MQFlavor flavor>
AidlMQDescriptorShim<T, flavor>::AidlMQDescriptorShim(
        const MQDescriptor<T, typename std::conditional<flavor == hardware::kSynchronizedReadWrite,
                                                        SynchronizedReadWrite,
                                                        UnsynchronizedWrite>::type>& desc)
<<<<<<< HEAD
    : mQuantum(desc.quantum), mFlags(desc.flags) {
#ifdef _MSC_VER
    fromString( desc.json_decriptor );
#else
    if (desc.quantum < 0 || desc.flags < 0) {
        // MQDescriptor uses signed integers, but the values must be positive.
        hardware::details::logError("Invalid MQDescriptor. Values must be positive. quantum: " +
                                    std::to_string(desc.quantum) +
                                    ". flags: " + std::to_string(desc.flags));
        return;
    }

    mGrantors.resize(desc.grantors.size());
    for (size_t i = 0; i < desc.grantors.size(); ++i) {
        if (desc.grantors[i].offset < 0 || desc.grantors[i].extent < 0 ||
            desc.grantors[i].fdIndex < 0) {
            // GrantorDescriptor uses signed integers, but the values must be positive.
            // Return before setting up the native_handle to make this invalid.
            hardware::details::logError(
                    "Invalid MQDescriptor grantors. Values must be positive. Grantor index: " +
                    std::to_string(i) + ". offset: " + std::to_string(desc.grantors[i].offset) +
                    ". extent: " + std::to_string(desc.grantors[i].extent));
            return;
        }
        mGrantors[i].flags = 0;
        mGrantors[i].fdIndex = desc.grantors[i].fdIndex;
        mGrantors[i].offset = desc.grantors[i].offset;
        mGrantors[i].extent = desc.grantors[i].extent;
    }

    mHandle = native_handle_create(desc.handle.fds.size() /* num fds */,
                                   desc.handle.ints.size() /* num ints */);
    if (mHandle == nullptr) {
        hardware::details::logError("Null native_handle_t");
        return;
    }
    int data_index = 0;
    for (const auto& fd : desc.handle.fds) {
        mHandle->data[data_index] = dup(fd.get());
        data_index++;
    }
    for (const auto& data_int : desc.handle.ints) {
        mHandle->data[data_index] = data_int;
        data_index++;
    }
#endif
}
=======
    : AidlMQDescriptorShimBase<T, flavor, BackendTypesStore>(desc) {}
>>>>>>> 208ef36

template <typename T, MQFlavor flavor>
AidlMQDescriptorShim<T, flavor>::AidlMQDescriptorShim(
        const std::vector<android::hardware::GrantorDescriptor>& grantors, native_handle_t* nhandle,
        size_t size)
<<<<<<< HEAD
    : mGrantors(grantors),
      mHandle(nhandle),
      mQuantum(static_cast<uint32_t>(size)),
      mFlags(flavor) {}

template <typename T, MQFlavor flavor>
AidlMQDescriptorShim<T, flavor>& AidlMQDescriptorShim<T, flavor>::operator=(
        const AidlMQDescriptorShim& other) {
    mGrantors = other.mGrantors;
    if (mHandle != nullptr) {
        native_handle_close(mHandle);
        native_handle_delete(mHandle);
        mHandle = nullptr;
    }
    mQuantum = other.mQuantum;
    mFlags = other.mFlags;

#ifdef _MSC_VER
    mName = other.getName();
#endif

    if (other.mHandle != nullptr) {
        mHandle = native_handle_create(other.mHandle->numFds, other.mHandle->numInts);

        for (int i = 0; i < other.mHandle->numFds; ++i) {
#ifdef _MSC_VER
            mHandle->data[i] = other.mHandle->data[i];
#else
            mHandle->data[i] = dup(other.mHandle->data[i]);
#endif
        }

        memcpy(&mHandle->data[other.mHandle->numFds], &other.mHandle->data[other.mHandle->numFds],
               static_cast<size_t>(other.mHandle->numInts) * sizeof(int));
    }

    return *this;
}
=======
    : AidlMQDescriptorShimBase<T, flavor, BackendTypesStore>(grantors, nhandle, size) {}
>>>>>>> 208ef36

template <typename T, MQFlavor flavor>
AidlMQDescriptorShim<T, flavor>::AidlMQDescriptorShim(size_t bufferSize, native_handle_t* nHandle,
                                                      size_t messageSize, bool configureEventFlag)
    : AidlMQDescriptorShimBase<T, flavor, BackendTypesStore>(bufferSize, nHandle, messageSize,
                                                             configureEventFlag) {}
}  // namespace details
}  // namespace android
