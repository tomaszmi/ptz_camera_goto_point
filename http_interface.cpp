#include "http_interface.h"

#include <charconv>
#include <chrono>
#include <iomanip>
#include <limits>
#include <sstream>
#include <string_view>
#include <thread>

#include <boost/algorithm/string/find_iterator.hpp>
#include <boost/algorithm/string/finder.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <glog/logging.h>

#include "curl_error_category.h"
#include "dahua_error_category.h"
#include "dahua_ptz_camera.h"

namespace tpxai::dahua {

HTTPInterface::HTTPInterface(std::string user, std::string password, std::string host, unsigned short port)
    : user_password_{user + ":" + password}, host_{std::move(host)}, port_{port}, curl_{curl_easy_init(),
                                                                                        &curl_easy_cleanup} {
  CHECK(curl_);
}

std::string HTTPInterface::GetStreamingURL() const {
  std::ostringstream ss;
  ss << "rtsp://" << user_password_ << "@" << host_ << ":" << port_ << "/cam/realmonitor?channel=1&subtype=0";
  return ss.str();
}

std::error_code HTTPInterface::GoToABSPosition(const PTZCameraPosition& position, std::uint16_t zoom_multiple) {
  const auto [error, response] = HTTPGetRequest(CreateGoToABSPositionURL(position, zoom_multiple));
  if (not error) {
    return make_error_code(response == "OK" ? DahuaErrorCode::ok : DahuaErrorCode::error);
  }
  return error;
}

std::pair<std::error_code, cv::Size> HTTPInterface::GetResolution() {
  std::pair<std::error_code, cv::Size> result;
  const auto [error, response] = HTTPGetRequest(CreateGetVideoEncodeConfigURL());
  if (error) {
    result.first = error;
    return result;
  }
  {
    const auto [error, option_value] =
        ExtractNumericOptionValueFromMultiline(response, "table.Encode[0].MainFormat[0].Video.Width=");
    if (error) {
      result.first = error;
      return result;
    }
    result.second.width = option_value;
  }
  {
    const auto [error, option_value] =
        ExtractNumericOptionValueFromMultiline(response, "table.Encode[0].MainFormat[0].Video.Height=");
    if (error) {
      result.first = error;
      return result;
    }
    result.second.height = option_value;
  }
  return result;
}

std::pair<std::error_code, std::uint16_t> HTTPInterface::GetFrameRate() {
  std::pair<std::error_code, std::uint16_t> result;
  const auto [error, response] = HTTPGetRequest(CreateGetVideoEncodeConfigURL());
  if (error) {
    result.first = error;
  } else {
    const auto [error, option_value] =
        ExtractNumericOptionValueFromMultiline(response, "table.Encode[0].MainFormat[0].Video.FPS=");
    if (error) {
      result.first = error;
    } else {
      DCHECK(option_value > 0 and option_value <= std::numeric_limits<std::uint16_t>::max());
      result.second = static_cast<std::uint16_t>(option_value);
    }
  }
  return result;
}

std::pair<std::error_code, std::string> HTTPInterface::GetDeviceType() {
  std::pair<std::error_code, std::string> result;
  const auto [error, response] = HTTPGetRequest(CreateGetDeviceTypeURL());
  if (error) {
    result.first = error;
  } else {
    const auto [error, option_value] = ExtractStringOptionValueFromMultiline(response, "type=");
    if (error) {
      result.first = error;
    } else {
      result.second = option_value;
    }
  }
  return result;
}

std::error_code HTTPInterface::StartThenStopCommand(const std::string& start_cmd, std::chrono::milliseconds nap_time,
                                                    const std::string& stop_cmd) {
  {
    auto [error, response] = HTTPGetRequest(start_cmd);
    if (error) {
      return error;
    }
    if (response != "OK") {
      return make_error_code(DahuaErrorCode::error);
    }
  }
  std::this_thread::sleep_for(nap_time);
  auto [error, response] = HTTPGetRequest(stop_cmd);
  if (error) {
    return error;
  }
  return make_error_code(response == "OK" ? DahuaErrorCode::ok : DahuaErrorCode::error);
}

std::error_code HTTPInterface::SetFocusNear(std::uint16_t multiple) {
  return StartThenStopCommand(CreateSetFocusNear(multiple, Action::start), std::chrono::milliseconds{100},
                              CreateSetFocusNear(multiple, Action::stop));
}

std::error_code HTTPInterface::SetFocusFar(std::uint16_t multiple) {
  return StartThenStopCommand(CreateSetFocusFar(multiple, Action::start), std::chrono::milliseconds{100},
                              CreateSetFocusFar(multiple, Action::stop));
}

std::string HTTPInterface::CreateGoToABSPositionURL(const PTZCameraPosition& position,
                                                    std::uint16_t zoom_multiple) const {
  std::ostringstream ss;
  ss << std::setprecision(1) << std::fixed << "http://" << host_
     << "/cgi-bin/ptz.cgi?action=start&channel=1&code=PositionABS&arg1=" << position.horizontal_angle
     << "&arg2=" << position.vertical_angle << "&arg3=" << zoom_multiple;
  return ss.str();
}

std::string HTTPInterface::CreateGetVideoEncodeConfigURL() const {
  std::ostringstream ss;
  ss << "http://" << host_ << "/cgi-bin/configManager.cgi?action=getConfig&name=Encode";
  return ss.str();
}

std::string HTTPInterface::CreateGetDeviceTypeURL() const {
  std::ostringstream ss;
  ss << "http://" << host_ << "/cgi-bin/magicBox.cgi?action=getDeviceType";
  return ss.str();
}

std::string HTTPInterface::CreateSetFocusNear(std::uint16_t multiple, Action action) {
  std::ostringstream ss;
  ss << "http://" << host_ << "/cgi-bin/ptz.cgi?action=" << (action == Action::start ? "start" : "stop")
     << "&channel=1&code=FocusNear&arg1=0&arg2=" << multiple << "&arg3=0";
  return ss.str();
}

std::string HTTPInterface::CreateSetFocusFar(std::uint16_t multiple, Action action) {
  std::ostringstream ss;
  ss << "http://" << host_ << "/cgi-bin/ptz.cgi?action=" << (action == Action::start ? "start" : "stop")
     << "&channel=1&code=FocusFar&arg1=0&arg2=" << multiple << "&arg3=0";
  return ss.str();
}

namespace {

std::size_t CURLWriteCallback(void* chunk, std::size_t size, std::size_t nmemb, void* context) {
  DCHECK(size == 1);
  auto buffer = static_cast<std::string*>(context);
  buffer->append(static_cast<char*>(chunk), nmemb);
  return nmemb;
}

} // anonymous namespace

std::pair<std::error_code, std::string> HTTPInterface::HTTPGetRequest(const std::string& url) {
  std::string response_buffer;
  // curl_easy_reset(curl_.get());
  // curl_easy_setopt(curl_.get(), CURLOPT_VERBOSE, 1L);
  curl_easy_setopt(curl_.get(), CURLOPT_ERRORBUFFER, error_buffer_.data());
  curl_easy_setopt(curl_.get(), CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl_.get(), CURLOPT_PORT, port_);
  curl_easy_setopt(curl_.get(), CURLOPT_HTTPGET, 1L);
  curl_easy_setopt(curl_.get(), CURLOPT_HTTPAUTH, CURLAUTH_DIGEST);
  curl_easy_setopt(curl_.get(), CURLOPT_USERPWD, user_password_.c_str());
  curl_easy_setopt(curl_.get(), CURLOPT_WRITEFUNCTION, CURLWriteCallback);
  curl_easy_setopt(curl_.get(), CURLOPT_WRITEDATA, &response_buffer);
  curl_easy_setopt(curl_.get(), CURLOPT_TIMEOUT, std::chrono::seconds{5}.count());

  error_buffer_[0] = '\0';
  auto res = curl_easy_perform(curl_.get());
  if (res != CURLE_OK) {
    const auto len = std::strlen(error_buffer_.data());
    if (len) {
      LOG(ERROR) << error_buffer_.data();
    } else {
      LOG(ERROR) << curl_easy_strerror(res);
    }
    return {make_error_code(res), {}};
  }
  boost::algorithm::trim(response_buffer);
  return {{}, std::move(response_buffer)};
}

namespace {

std::string_view FindLineStartingWithPrefix(std::string_view multi_line_response, const char* prefix) {
  for (auto split_it = boost::make_split_iterator(
           multi_line_response, boost::token_finder(boost::is_any_of("\n\r\t"), boost::token_compress_on));
       split_it != decltype(split_it)(); ++split_it) {
    if (not split_it->empty()) {
      std::string_view line(&*split_it->begin(), split_it->size());
      if (line.find(prefix) == 0) {
        return line;
      }
    }
  }
  return {};
}

std::pair<std::error_code, int> ToInteger(std::string_view text) {
  std::pair<std::error_code, int> result;
  if (auto [ptr, ec] = std::from_chars(text.data(), text.data() + text.size(), result.second); ec != std::errc()) {
    result.first = std::make_error_code(ec);
  }
  return result;
}

} // anonymous namespace

std::pair<std::error_code, int> ExtractNumericOptionValueFromMultiline(std::string_view multiline, const char* option) {
  auto line = FindLineStartingWithPrefix(multiline, option);
  std::pair<std::error_code, int> result{{}, 0};
  if (line.empty()) {
    result.first = std::make_error_code(std::errc::invalid_argument);
  } else {
    result = ToInteger(line.substr(std::strlen(option)));
  }
  return result;
}

std::pair<std::error_code, std::string> ExtractStringOptionValueFromMultiline(std::string_view multiline,
                                                                              const char* option) {
  auto line = FindLineStartingWithPrefix(multiline, option);
  std::pair<std::error_code, std::string> result;
  if (line.empty()) {
    result.first = std::make_error_code(std::errc::invalid_argument);
  } else {
    result.second = line.substr(std::strlen(option));
  }
  return result;
}

} // namespace tpxai::dahua
