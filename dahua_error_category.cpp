#include "dahua_error_category.h"

namespace tpxai::dahua {

const char* DahuaErrorCategory::name() const noexcept { return "Dahua"; }

std::string DahuaErrorCategory::message(int condition) const {
  switch (static_cast<DahuaErrorCode>(condition)) {
    case DahuaErrorCode::ok:
      return "OK";
    case DahuaErrorCode::error:
      return "ERROR";
    default:
      return "UNKNOWN ERROR";
  }
}

std::error_code make_error_code(const DahuaErrorCode& e) {
  static const DahuaErrorCategory error_category;
  return {static_cast<int>(e), error_category};
}

} // namespace tpxai::dahua
