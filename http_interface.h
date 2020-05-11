#pragma once

#include <array>
#include <chrono>
#include <memory>
#include <string>
#include <system_error>
#include <utility>

#include <opencv2/core/types.hpp>

#include <curl/curl.h>

namespace tpxai {

struct PTZCameraPosition;

namespace dahua {

class HTTPInterface {
public:
  HTTPInterface(std::string user, std::string password, std::string host, unsigned short port);
  std::string GetStreamingURL() const;
  std::error_code GoToABSPosition(const PTZCameraPosition& position, std::uint16_t zoom_multiple);
  std::pair<std::error_code, cv::Size> GetResolution();
  std::pair<std::error_code, std::uint16_t> GetFrameRate();
  std::pair<std::error_code, std::string> GetDeviceType();
  std::error_code SetFocusNear(std::uint16_t multiple);
  std::error_code SetFocusFar(std::uint16_t multiple);

private:
  enum class Action { start, stop };

  std::string CreateGoToABSPositionURL(const PTZCameraPosition& position, std::uint16_t zoom_multiple) const;
  std::string CreateGetVideoEncodeConfigURL() const;
  std::string CreateGetDeviceTypeURL() const;
  std::string CreateSetFocusNear(std::uint16_t multiple, Action action);
  std::string CreateSetFocusFar(std::uint16_t multiple, Action action);

  std::error_code StartThenStopCommand(const std::string& start_cmd, std::chrono::milliseconds nap_time,
                                       const std::string& stop_cmd);

  std::pair<std::error_code, std::string> HTTPGetRequest(const std::string& url);

  std::string user_password_;
  std::string host_;
  unsigned short port_;
  std::unique_ptr<CURL, decltype(&curl_easy_cleanup)> curl_;
  std::array<char, CURL_ERROR_SIZE> error_buffer_ = {};
};

std::pair<std::error_code, int> ExtractNumericOptionValueFromMultiline(std::string_view multiline, const char* option);

std::pair<std::error_code, std::string> ExtractStringOptionValueFromMultiline(std::string_view multiline,
                                                                              const char* option);

} // namespace dahua
} // namespace tpxai
