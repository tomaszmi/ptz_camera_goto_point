#include <glog/logging.h>

#include <Eigen/Geometry>
#include <opencv2/core/eigen.hpp>
#include <opencv2/opencv.hpp>

#include "dahua_ptz_camera.h"

namespace {

// x axis - vertical
// y axis - horizontal

Eigen::Vector3f DegreesToRadians(Eigen::Vector3f degrees) noexcept { return degrees * CV_PI / 180; }

Eigen::Vector3f RadiansToDegrees(Eigen::Vector3f radians) noexcept { return radians * 180 / CV_PI; }

Eigen::Matrix3f EulerAnglesToRotationMatrix(const Eigen::Vector3f& theta) {
  const Eigen::AngleAxisf x_angle_rot(theta[0], Eigen::Vector3f::UnitX());
  const Eigen::AngleAxisf y_angle_rot(theta[1], Eigen::Vector3f::UnitY());
  const Eigen::AngleAxisf z_angle_rot(theta[2], Eigen::Vector3f::UnitZ());

  Eigen::Matrix3f m;
  m = y_angle_rot * x_angle_rot * z_angle_rot;
  return m;
}

Eigen::Vector3f CalculatePointInScreenCoords(const cv::Point& point, const cv::Matx33d& K) {
  const auto cx = K(0, 2);
  const auto cy = K(1, 2);
  const auto fx = K(0, 0);
  const auto fy = K(1, 1);
  float x = (point.x - cx) / fx;
  float y = (point.y - cy) / fy;
  return {x, y, 1};
}

Eigen::Matrix3f CalculateRotationMatrixForPoint(const cv::Point& point, const cv::Matx33d& K) {
  Eigen::Quaternionf quat =
      Eigen::Quaternionf::FromTwoVectors(Eigen::Vector3f{0, 0, 1}, CalculatePointInScreenCoords(point, K));
  quat.normalize();
  return quat.toRotationMatrix();
}

Eigen::Vector3f
CalculateAbsolutePosition(const cv::Point &point, const cv::Matx33d &K,
                          const Eigen::Vector3f &current_euler_angles) {

  std::cout << "point.x: " << point.x << ", point.y: " << point.y << std::endl;

  const Eigen::Matrix3f global_to_current_rotation =
      EulerAnglesToRotationMatrix(DegreesToRadians({current_euler_angles[0], current_euler_angles[1], 0})).transpose();
  const Eigen::Matrix3f current_to_new_rotation = CalculateRotationMatrixForPoint(point, K);
  const Eigen::Matrix3f global_to_new_rotation = current_to_new_rotation * global_to_current_rotation;

  std::cout << "global_to_current_rotation: " << global_to_current_rotation << std::endl;
  std::cout << "current_to_new_rotation: " << current_to_new_rotation << std::endl;
  std::cout << "global_to_new_rotation: " << global_to_new_rotation << std::endl;
  std::cout << "new_to_global_rotation: " << global_to_new_rotation.transpose() << std::endl;

  const Eigen::Vector3f angles = global_to_new_rotation.transpose().eulerAngles(1, 0, 2);
  return RadiansToDegrees({angles[1], angles[0], angles[2]});
}

struct MouseClickCallbackContext {
  tpxai::dahua::DahuaPTZCamera* ptz_camera = nullptr;
  Eigen::Vector3f current_position; // Euler angles
};

void OnMouseClickCallback(int event, int x, int y, int/* flags*/, void* userdata) {
  if(event != cv::EVENT_LBUTTONDBLCLK) {
    return;
  }
  auto ctx = static_cast<MouseClickCallbackContext*>(userdata);

  Eigen::Vector3f new_abs_position = CalculateAbsolutePosition(
      cv::Point(x, y), ctx->ptz_camera->GetIntrinsics().K, ctx->current_position);
  std::cout << "goto: " << new_abs_position << std::endl;
  ctx->ptz_camera->SetAbsolutePosition(tpxai::PTZCameraPosition{new_abs_position[1], new_abs_position[0]});
  ctx->current_position = new_abs_position;
  std::cout << "===========================================" << std::endl;
}

void Run(tpxai::dahua::DahuaPTZCamera& ptz_camera) {
  auto current_pos = ptz_camera.GetCurrentPosition();
  MouseClickCallbackContext clbk_ctx{
      &ptz_camera,
      {current_pos.vertical_angle, current_pos.horizontal_angle, 0},
  };

  cv::namedWindow("dahua", cv::WINDOW_AUTOSIZE);
  cv::setMouseCallback("dahua", OnMouseClickCallback, &clbk_ctx);

  for(int key = 0; key != 'q'; key = cv::waitKey(1)) {
    auto next_frame = ptz_camera.GetNextFrame();
    cv::circle(next_frame, ptz_camera.GetIntrinsics().center(), 10,
               cv::viz::Color::red(), cv::FILLED);
    cv::imshow("dahua", next_frame);
  }
}

} // anonymous namespace

int main() try {
  tpxai::dahua::DahuaPTZCamera ptz_camera("admin", "DUPAdupa..", "192.168.1.105", 80);
  ptz_camera.SetAbsolutePosition(tpxai::PTZCameraPosition{0, 0});
  Run(ptz_camera);
  return 0;
} catch (std::exception& e) {
  std::cerr << e.what() << std::endl;
  return 1;
} catch (...) {
  std::cerr << "Unknown error" << std::endl;
  return 1;
}
