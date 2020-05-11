#pragma once

#include <string>

#include <opencv2/opencv.hpp>

#include "http_interface.h"
#include "camera_intrinsics.h"

namespace tpxai {

struct PTZCameraPosition {
  // TODO use cv::Vec2f for angles
  float horizontal_angle = 0;
  float vertical_angle = 0;
};

namespace dahua {

class DahuaPTZCamera {
public:
  DahuaPTZCamera(std::string user, std::string password, std::string host,
                 unsigned short port);

  void SetZoom(std::uint16_t multiple);

  void SetAbsolutePosition(const PTZCameraPosition& position);
  void SetAbsolutePosition(const PTZCameraPosition& position, std::uint16_t zoom_multiple);

  void SetFocusNear(std::uint16_t multiple);
  void SetFocusFar(std::uint16_t multiple);

  PTZCameraPosition GetCurrentPosition() const;
  std::uint16_t GetCurrentZoom() const;

  CameraIntrinsics GetIntrinsics() const;

  cv::Mat GetNextFrame();

private:
  cv::VideoCapture capture_;
  HTTPInterface http_iface_;
  PTZCameraPosition current_position_;
  std::uint16_t current_zoom_multiple_ = 0;
};

}} // namespace tpxai::dahua
