#include "position_calculator.h"

namespace tpxai {

namespace {

// x axis - vertical
// y axis - horizontal

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

[[maybe_unused]] Eigen::Matrix3f CalculateRotationMatrixForPoint(const cv::Point& point, const cv::Matx33d& K) {
  Eigen::Vector3f point_in_camera_coords = CalculatePointInScreenCoords(point, K);
  point_in_camera_coords.normalize();
  Eigen::Quaternionf quat = Eigen::Quaternionf::FromTwoVectors(
      Eigen::Vector3f{0, 0, 1}, point_in_camera_coords);
  quat.normalize();
  return quat.toRotationMatrix();
}

[[maybe_unused]] Eigen::Matrix3f CalculateRotationMatrixForPoint(const Eigen::Vector3f& point) {
  Eigen::Quaternionf quat = Eigen::Quaternionf::FromTwoVectors(
      Eigen::Vector3f{0, 0, 1}, point);
  quat.normalize();
  return quat.toRotationMatrix();
}

/*constexpr double VSHIFT = 6.08049 / 2; // CALCULATED BY MOTOR CALIBRATION
constexpr double VSHIFT_RADIANS = DegreesToRadians(VSHIFT);*/

[[maybe_unused]] Eigen::Vector3f NormalizeAngles(Eigen::Vector3f angles) {

  return angles;
  constexpr auto min_x_angle = DegreesToRadians(-15);
  constexpr auto max_x_angle = DegreesToRadians(90);

  {
    auto angles_in_degrees = RadiansToDegrees(angles);
    std::cout << "before normalization: " << angles_in_degrees.transpose() << std::endl;
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

Eigen::Vector3f ComputeAnglesFromPoint(const Eigen::Vector3f& point) {
  const Eigen::Vector2f projection_XZ {point[0], point[2]};
  const Eigen::Vector2f projection_YZ {point[1], projection_XZ.norm()};

  auto x_angle = std::atan2(projection_YZ[0], projection_YZ[1]);
  auto y_angle = -std::atan2(projection_XZ[0], projection_XZ[1]);

  y_angle = y_angle >= 0 ? y_angle : CV_2PI + y_angle;

  return {x_angle, y_angle, 0.f};
}


} // anonymous namespace 

Eigen::Vector3f CalculateAbsolutePosition(const cv::Point& point, const cv::Matx33d& K,
                                          const Eigen::Vector3f& current_euler_angles) {

  std::cout << "Point: " << point.x << "x" << point.y << std::endl;

  auto current_euler_angles_in_radians = DegreesToRadians(-current_euler_angles);
  //current_euler_angles_in_radians[0] -= VSHIFT_RADIANS;

  /*const Eigen::Matrix3f global_to_current_rotation =
      EulerAnglesToRotationMatrix(current_euler_angles_in_radians).transpose();
  const Eigen::Matrix3f current_to_new_rotation = CalculateRotationMatrixForPoint(point, K);
  const Eigen::Matrix3f global_to_new_rotation = current_to_new_rotation *
  global_to_current_rotation;*/

  const Eigen::Matrix3f current_to_global_rotation =
      EulerAnglesToRotationMatrix(current_euler_angles_in_radians);

  const Eigen::Vector3f point_in_camera_coords = CalculatePointInScreenCoords(point, K);
  auto point_in_global = current_to_global_rotation * point_in_camera_coords;

  return RadiansToDegrees(ComputeAnglesFromPoint(point_in_global));
//  const Eigen::Matrix3f global_to_new_rotation = CalculateRotationMatrixForPoint(point_in_global);

  /*std::cout << "global_to_current_rotation: " << global_to_current_rotation << std::endl;
  std::cout << "current_to_new_rotation: " << current_to_new_rotation << std::endl;
  std::cout << "global_to_new_rotation: " << global_to_new_rotation << std::endl;
  std::cout << "new_to_global_rotation: " << global_to_new_rotation.transpose() << std::endl;*/

  //Eigen::Vector3f angles =
  //    global_to_new_rotation.transpose().eulerAngles(1, 0, 2);
  // the eulerAngles doc says: The returned angles are in the ranges [0:pi]x[-pi:pi]x[-pi:pi].
  //angles[1] += VSHIFT_RADIANS;

//  Eigen::Vector3f angles = global_to_new_rotation.eulerAngles(0, 1, 2);
//  return RadiansToDegrees(NormalizeAngles(Eigen::Vector3f{angles[0], angles[1], angles[2]}));
}

} // namespace tpxai
