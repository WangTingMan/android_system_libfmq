#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <android/binder_interface_utils.h>
#include <android/binder_parcelable_utils.h>
#include <android/binder_to_string.h>
#include <aidl/android/hardware/common/NativeHandle.h>
#include <aidl/android/hardware/common/fmq/GrantorDescriptor.h>
#ifdef BINDER_STABILITY_SUPPORT
#include <android/binder_stability.h>
#endif  // BINDER_STABILITY_SUPPORT

namespace aidl::android::hardware::common {
class NativeHandle;
}  // namespace aidl::android::hardware::common
namespace aidl::android::hardware::common::fmq {
class GrantorDescriptor;
}  // namespace aidl::android::hardware::common::fmq
namespace aidl {
namespace android {
namespace hardware {
namespace common {
namespace fmq {
template <typename T, typename Flavor>
class MQDescriptor {
public:
  typedef std::false_type fixed_size;
  static const char* descriptor;

  std::vector<::aidl::android::hardware::common::fmq::GrantorDescriptor> grantors;
  ::aidl::android::hardware::common::NativeHandle handle;
  int32_t quantum = 0;
  int32_t flags = 0;

#ifdef _MSC_VER
  std::string name;
  std::string json_decriptor;
  bool from_json_descriptor(std::string a_json_decriptor);
  std::string to_json_string();
#endif

  binder_status_t readFromParcel(const AParcel* parcel);
  binder_status_t writeToParcel(AParcel* parcel) const;

  inline bool operator!=(const MQDescriptor& rhs) const {
    return std::tie(grantors, handle, quantum, flags) != std::tie(rhs.grantors, rhs.handle, rhs.quantum, rhs.flags);
  }
  inline bool operator<(const MQDescriptor& rhs) const {
    return std::tie(grantors, handle, quantum, flags) < std::tie(rhs.grantors, rhs.handle, rhs.quantum, rhs.flags);
  }
  inline bool operator<=(const MQDescriptor& rhs) const {
    return std::tie(grantors, handle, quantum, flags) <= std::tie(rhs.grantors, rhs.handle, rhs.quantum, rhs.flags);
  }
  inline bool operator==(const MQDescriptor& rhs) const {
    return std::tie(grantors, handle, quantum, flags) == std::tie(rhs.grantors, rhs.handle, rhs.quantum, rhs.flags);
  }
  inline bool operator>(const MQDescriptor& rhs) const {
    return std::tie(grantors, handle, quantum, flags) > std::tie(rhs.grantors, rhs.handle, rhs.quantum, rhs.flags);
  }
  inline bool operator>=(const MQDescriptor& rhs) const {
    return std::tie(grantors, handle, quantum, flags) >= std::tie(rhs.grantors, rhs.handle, rhs.quantum, rhs.flags);
  }

  static const ::ndk::parcelable_stability_t _aidl_stability = ::ndk::STABILITY_VINTF;
  inline std::string toString() const {
    std::ostringstream os;
    os << "MQDescriptor{";
    os << "grantors: " << ::android::internal::ToString(grantors);
    os << ", handle: " << ::android::internal::ToString(handle);
    os << ", quantum: " << ::android::internal::ToString(quantum);
    os << ", flags: " << ::android::internal::ToString(flags);
    os << "}";
    return os.str();
  }
};
}  // namespace fmq
}  // namespace common
}  // namespace hardware
}  // namespace android
}  // namespace aidl
#include "aidl/android/hardware/common/fmq/MQDescriptor.h"

#include <android/binder_parcel_utils.h>

namespace aidl {
namespace android {
namespace hardware {
namespace common {
namespace fmq {
template <typename T, typename Flavor>
const char* MQDescriptor<T, Flavor>::descriptor = "android.hardware.common.fmq.MQDescriptor";

template <typename T, typename Flavor>
binder_status_t MQDescriptor<T, Flavor>::readFromParcel(const AParcel* _aidl_parcel) {
  binder_status_t _aidl_ret_status = STATUS_OK;
  int32_t _aidl_start_pos = AParcel_getDataPosition(_aidl_parcel);
  int32_t _aidl_parcelable_size = 0;
  _aidl_ret_status = AParcel_readInt32(_aidl_parcel, &_aidl_parcelable_size);
  if (_aidl_ret_status != STATUS_OK) return _aidl_ret_status;

  if (_aidl_parcelable_size < 4) return STATUS_BAD_VALUE;
  if (_aidl_start_pos > INT32_MAX - _aidl_parcelable_size) return STATUS_BAD_VALUE;
  if (AParcel_getDataPosition(_aidl_parcel) - _aidl_start_pos >= _aidl_parcelable_size) {
    AParcel_setDataPosition(_aidl_parcel, _aidl_start_pos + _aidl_parcelable_size);
    return _aidl_ret_status;
  }
  _aidl_ret_status = ::ndk::AParcel_readData(_aidl_parcel, &grantors);
  if (_aidl_ret_status != STATUS_OK) return _aidl_ret_status;

  if (AParcel_getDataPosition(_aidl_parcel) - _aidl_start_pos >= _aidl_parcelable_size) {
    AParcel_setDataPosition(_aidl_parcel, _aidl_start_pos + _aidl_parcelable_size);
    return _aidl_ret_status;
  }
  _aidl_ret_status = ::ndk::AParcel_readData(_aidl_parcel, &handle);
  if (_aidl_ret_status != STATUS_OK) return _aidl_ret_status;

  if (AParcel_getDataPosition(_aidl_parcel) - _aidl_start_pos >= _aidl_parcelable_size) {
    AParcel_setDataPosition(_aidl_parcel, _aidl_start_pos + _aidl_parcelable_size);
    return _aidl_ret_status;
  }
  _aidl_ret_status = ::ndk::AParcel_readData(_aidl_parcel, &quantum);
  if (_aidl_ret_status != STATUS_OK) return _aidl_ret_status;

  if (AParcel_getDataPosition(_aidl_parcel) - _aidl_start_pos >= _aidl_parcelable_size) {
    AParcel_setDataPosition(_aidl_parcel, _aidl_start_pos + _aidl_parcelable_size);
    return _aidl_ret_status;
  }
  _aidl_ret_status = ::ndk::AParcel_readData(_aidl_parcel, &flags);
  if (_aidl_ret_status != STATUS_OK) return _aidl_ret_status;

#ifdef _MSC_VER
  _aidl_ret_status = ::ndk::AParcel_readString( _aidl_parcel, &json_decriptor );
#endif

  AParcel_setDataPosition(_aidl_parcel, _aidl_start_pos + _aidl_parcelable_size);

  return _aidl_ret_status;
}
template <typename T, typename Flavor>
binder_status_t MQDescriptor<T, Flavor>::writeToParcel(AParcel* _aidl_parcel) const {
  binder_status_t _aidl_ret_status;
  size_t _aidl_start_pos = AParcel_getDataPosition(_aidl_parcel);
  _aidl_ret_status = AParcel_writeInt32(_aidl_parcel, 0);
  if (_aidl_ret_status != STATUS_OK) return _aidl_ret_status;

  _aidl_ret_status = ::ndk::AParcel_writeData(_aidl_parcel, grantors);
  if (_aidl_ret_status != STATUS_OK) return _aidl_ret_status;

  _aidl_ret_status = ::ndk::AParcel_writeData(_aidl_parcel, handle);
  if (_aidl_ret_status != STATUS_OK) return _aidl_ret_status;

  _aidl_ret_status = ::ndk::AParcel_writeData(_aidl_parcel, quantum);
  if (_aidl_ret_status != STATUS_OK) return _aidl_ret_status;

  _aidl_ret_status = ::ndk::AParcel_writeData(_aidl_parcel, flags);
  if (_aidl_ret_status != STATUS_OK) return _aidl_ret_status;

#ifdef _MSC_VER
  ::ndk::AParcel_writeString( _aidl_parcel, json_decriptor );
#endif

  size_t _aidl_end_pos = AParcel_getDataPosition(_aidl_parcel);
  AParcel_setDataPosition(_aidl_parcel, _aidl_start_pos);
  AParcel_writeInt32(_aidl_parcel, _aidl_end_pos - _aidl_start_pos);
  AParcel_setDataPosition(_aidl_parcel, _aidl_end_pos);

  return _aidl_ret_status;
}

#ifdef _MSC_VER
template <typename T, typename Flavor>
bool MQDescriptor<T, Flavor>::from_json_descriptor(std::string a_json_decriptor)
{
    native_handle_t native_handle;
    native_handle_t* p_native_handle = &native_handle;
    std::vector<::android::hardware::GrantorDescriptor> grantor_desc;
    uint32_t quantum_value = 0;
    uint32_t flags_value = 0;
    ::system_porting::from_string( a_json_decriptor, grantor_desc, p_native_handle,
        quantum_value, flags_value, name);
    grantors.clear();
    for (auto& ele : grantor_desc)
    {
        ::aidl::android::hardware::common::fmq::GrantorDescriptor local_use_grantor;
        local_use_grantor.extent = ele.extent;
        local_use_grantor.fdIndex = ele.fdIndex;
        local_use_grantor.offset = ele.offset;
        grantors.push_back(local_use_grantor);
    }
    json_decriptor = a_json_decriptor;
    return true;
}

template <typename T, typename Flavor>
std::string MQDescriptor<T, Flavor>::to_json_string()
{
    if (!json_decriptor.empty())
    {
        return json_decriptor;
    }

    std::string str;
    //str = ::system_porting::generate_string(grantors, handle, quantum, flags, name);
    return str;
}
#endif

}  // namespace fmq
}  // namespace common
}  // namespace hardware
}  // namespace android
}  // namespace aidl
