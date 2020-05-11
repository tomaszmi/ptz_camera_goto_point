#pragma once

#include <system_error>

namespace tpxai::dahua {

enum class DahuaErrorCode {
  ok,
  error
};

std::error_code make_error_code(const DahuaErrorCode& e);

struct DahuaErrorCategory : std::error_category {
  const char* name() const noexcept override;
  std::string message(int condition) const override;
};

} // namespace tpxai::dahua

namespace std {

template <>
struct is_error_code_enum<tpxai::dahua::DahuaErrorCode> : true_type {};

}  // namespace std
