#pragma once

#include <system_error>

#include <curl/curl.h>

namespace tpxai::dahua {

using CURLErrorCode = decltype(CURLE_OK);

std::error_code make_error_code(const CURLErrorCode& e);

struct CURLErrorCategory : std::error_category {
  const char* name() const noexcept override;
  std::string message(int condition) const override;
};

} // namespace tpxai::dahua

namespace std {

template <>
struct is_error_code_enum<decltype(CURLE_OK)> : true_type {};

}  // namespace std
