#pragma once

#include <opencv2/core/matx.hpp>
#include <opencv2/core/types.hpp>

#include <cmath>

namespace tpxai {

struct CameraIntrinsics {
  cv::Matx33d K;
  std::vector<double> distortion_coeffs;

  cv::Point2d center() const { return {K(0, 2), K(1, 2)}; }

  double focal_length() const { return (K(0, 0) + K(1, 1)) / 2.0F; }

  double fov_x(int width) const { return atan2(static_cast<double>(width), 2 * K(0, 0)); }

  double fov_y(int height) const { return atan2(static_cast<double>(height), 2 * K(1, 1)); }
};

} // namespace tpxai
