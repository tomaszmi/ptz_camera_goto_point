#include <glog/logging.h>

#include <Eigen/Geometry>
#include <opencv2/core/eigen.hpp>
#include <opencv2/opencv.hpp>

#include "dahua_ptz_camera.h"

namespace {

// x axis - vertical
// y axis - horizontal

Eigen::Vector3f DegreesToRadians(Eigen::Vector3f degrees) noexcept { return degrees * CV_PI / 180; }

constexpr float DegreesToRadians(float degrees) noexcept { return degrees * CV_PI / 180; }

Eigen::Vector3f RadiansToDegrees(Eigen::Vector3f radians) noexcept { return radians * 180 / CV_PI; }

Eigen::Matrix3f EulerAnglesToRotationMatrix(const Eigen::Vector3f& theta) {
  const Eigen::AngleAxisf x_angle_rot(theta[0], Eigen::Vector3f::UnitX());
  const Eigen::AngleAxisf y_angle_rot(theta[1], Eigen::Vector3f::UnitY());

  Eigen::Matrix3f m;
  m = y_angle_rot * x_angle_rot;
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
  Eigen::Vector3f point_in_camera_coords = CalculatePointInScreenCoords(point, K);
  point_in_camera_coords.normalize();
  Eigen::Quaternionf quat = Eigen::Quaternionf::FromTwoVectors(
      Eigen::Vector3f{0, 0, 1}, point_in_camera_coords);
  quat.normalize();
  return quat.toRotationMatrix();
}

constexpr double VSHIFT = 6.08049 / 2; // CALCULATED BY MOTOR CALIBRATION
constexpr double VSHIFT_RADIANS = DegreesToRadians(VSHIFT);

Eigen::Vector3f NormalizeAngles(Eigen::Vector3f angles) {
  constexpr auto min_x_angle = DegreesToRadians(-15);
  constexpr auto max_x_angle = DegreesToRadians(90);

  {
    auto angles_in_degrees = RadiansToDegrees(angles);
    std::cout << "before normalization: " << angles_in_degrees << std::endl;
  }

  auto& x_angle = angles[0];
  auto& y_angle = angles[1];

  // normalization as x_angle must be from the range <-15, 90> degrees
  if(x_angle > CV_PI/2 and y_angle > CV_PI/2)
  {
    // for instance: x_angle: 172.403, y_angle: 170.045, z_angle: 179.337
    y_angle += CV_PI;
    x_angle = CV_PI - x_angle;
  }
  else if(x_angle < -CV_PI/2 and y_angle > CV_PI/2)
  {
    // for instance: x_angle: -173.011, y_angle: 170.902, z_angle: -179.443
    y_angle = CV_PI + y_angle;
    x_angle = -(CV_PI + x_angle);
  }
  
  if(x_angle < min_x_angle) {
    x_angle = min_x_angle;
  }
  else if(x_angle >= max_x_angle) {
    x_angle = max_x_angle;
  }
  return angles;
}

[[maybe_unused]] Eigen::Vector3f
CalculateAbsolutePosition(const cv::Point &point, const cv::Matx33d &K,
                          const Eigen::Vector3f& current_euler_angles) {

  auto current_euler_angles_in_radians = DegreesToRadians(current_euler_angles);
  current_euler_angles_in_radians[0] -= VSHIFT_RADIANS;

  const Eigen::Matrix3f global_to_current_rotation =
      EulerAnglesToRotationMatrix(current_euler_angles_in_radians).transpose();
  const Eigen::Matrix3f current_to_new_rotation = CalculateRotationMatrixForPoint(point, K);
  const Eigen::Matrix3f global_to_new_rotation = current_to_new_rotation * global_to_current_rotation;

  /*std::cout << "global_to_current_rotation: " << global_to_current_rotation << std::endl;
  std::cout << "current_to_new_rotation: " << current_to_new_rotation << std::endl;
  std::cout << "global_to_new_rotation: " << global_to_new_rotation << std::endl;
  std::cout << "new_to_global_rotation: " << global_to_new_rotation.transpose() << std::endl;*/

  Eigen::Vector3f angles =
      global_to_new_rotation.transpose().eulerAngles(1, 0, 2);
  // the eulerAngles doc says: The returned angles are in the ranges [0:pi]x[-pi:pi]x[-pi:pi].
  angles[1] += VSHIFT_RADIANS;
  return RadiansToDegrees(NormalizeAngles(Eigen::Vector3f{angles[1], angles[0], angles[2]}));
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
  std::cout << "PTZ move: (" << ctx->current_position[0] << ", "
            << ctx->current_position[1] << ") -> (" << new_abs_position[0]
            << ", " << new_abs_position[1] << ")" << std::endl;

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
