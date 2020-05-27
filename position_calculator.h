#pragma once

#include <Eigen/Geometry>
#include <opencv2/opencv.hpp>

#include "dahua_ptz_camera.h"

namespace tpxai {

inline Eigen::Vector3f DegreesToRadians(Eigen::Vector3f degrees) noexcept { return degrees * CV_PI / 180; }

constexpr float DegreesToRadians(float degrees) noexcept { return degrees * CV_PI / 180; }

inline Eigen::Vector3f RadiansToDegrees(Eigen::Vector3f radians) noexcept { return radians * 180 / CV_PI; }

Eigen::Vector3f CalculateAbsolutePosition(const cv::Point& point, const CameraIntrinsics& intrinsics,
                                          const Eigen::Vector3f& current_euler_angles_in_degrees);

} // namespace tpxai
