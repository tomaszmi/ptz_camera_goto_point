#include "dahua_ptz_camera.h"

namespace tpxai::dahua {

DahuaPTZCamera::DahuaPTZCamera(std::string user, std::string password,
                               std::string host, unsigned short port)
    : http_iface_{std::move(user), std::move(password), std::move(host), port} {

  bool status = capture_.open(http_iface_.GetStreamingURL());
  if (not status) {
    throw std::runtime_error("unable to start camera capture");
  }
  capture_.set(cv::CAP_PROP_FPS, 30);
  capture_.set(cv::CAP_PROP_FRAME_WIDTH, 2592);
  capture_.set(cv::CAP_PROP_FRAME_HEIGHT, 1520);
}

void DahuaPTZCamera::SetZoom(std::uint16_t multiple) {
  auto error = http_iface_.GoToABSPosition(current_position_, multiple);
  if (not error) {
    current_zoom_multiple_ = multiple;
  } else {
    throw std::system_error(error);
  }
}

void DahuaPTZCamera::SetAbsolutePosition(const PTZCameraPosition &position) {
  auto error = http_iface_.GoToABSPosition(position, current_zoom_multiple_);
  if (not error) {
    current_position_ = position;
  } else {
    throw std::system_error(error);
  }
}

void DahuaPTZCamera::SetAbsolutePosition(const PTZCameraPosition &position,
                                         std::uint16_t zoom_multiple) {
  auto error = http_iface_.GoToABSPosition(position, zoom_multiple);
  if (not error) {
    current_position_ = position;
    current_zoom_multiple_ = zoom_multiple;
  } else {
    throw std::system_error(error);
  }
}

void DahuaPTZCamera::SetFocusNear(std::uint16_t multiple) {
  auto error = http_iface_.SetFocusNear(multiple);
  if (error) {
    throw std::system_error(error);
  }
}

void DahuaPTZCamera::SetFocusFar(std::uint16_t multiple) {
  auto error = http_iface_.SetFocusFar(multiple);
  if (error) {
    throw std::system_error(error);
  }
}

PTZCameraPosition DahuaPTZCamera::GetCurrentPosition() const {
  return current_position_;
}

std::uint16_t DahuaPTZCamera::GetCurrentZoom() const {
  return current_zoom_multiple_;
}

CameraIntrinsics DahuaPTZCamera::GetIntrinsics() const {
  return {
    cv::Matx33d{
        2338.9152623521627,  0.,                  1297.4678987212778,
        0.,                  2338.5344212108994,  743.3445529777781,
        0.,                  0.,                 1.
    },
    {
      0.03413359728013275,
      0.20648704610948337,
      -0.0006930691652865927,
      -0.0020291504344734992
    }
  };
}

cv::Mat DahuaPTZCamera::GetNextFrame() {
  cv::Mat frame;
  capture_ >> frame;
  if (frame.empty()) {
    throw std::runtime_error("unable to get next frame");
  }
  return frame;
}

} // namespace tpxai::dahua
