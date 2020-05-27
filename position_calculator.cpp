#include "position_calculator.h"

namespace tpxai {

namespace {

// x axis - vertical
// y axis - horizontal

Eigen::Matrix3f EulerAnglesToRotationMatrix(const Eigen::Vector3f& theta) {
  const Eigen::AngleAxisf x_angle_rot(theta[0], Eigen::Vector3f::UnitX());
  const Eigen::AngleAxisf y_angle_rot(theta[1], Eigen::Vector3f::UnitY());

  Eigen::Matrix3f m;
  m = x_angle_rot * y_angle_rot;
  return m;
}

Eigen::Vector3f CalculatePointInScreenCoords(const cv::Point& point, const CameraIntrinsics& intrinsics) {
//  std::vector<cv::Point2d> undistorted_point_vec;
//  cv::undistortPoints(std::vector<cv::Point2d>{point}, undistorted_point_vec, intrinsics.K, intrinsics.distortion_coeffs);


/// for now without undistorting
  const auto undistorted_point = point; //undistorted_point_vec[0];

  const auto cx = intrinsics.K(0, 2);
  const auto cy = intrinsics.K(1, 2);
  const auto fx = intrinsics.K(0, 0);
  const auto fy = intrinsics.K(1, 1);
  float x = (undistorted_point.x - cx) / fx;
  float y = (undistorted_point.y - cy) / fy;
  return {x, y, 1};
}

[[maybe_unused]] Eigen::Matrix3f CalculateRotationMatrixForPoint(const cv::Point& point, const CameraIntrinsics& intrinsics) {
  Eigen::Vector3f point_in_camera_coords = CalculatePointInScreenCoords(point, intrinsics);
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

//constexpr double VSHIFT = 6.08049 / 2; // CALCULATED BY MOTOR CALIBRATION
//constexpr double VSHIFT_RADIANS = DegreesToRadians(VSHIFT);

Eigen::Vector3f NormalizeAngles(Eigen::Vector3f euler_angles) {
  if (euler_angles[0] > CV_PI / 2. + CV_PI / 180.f) {
    std::cout << "X decreasing "  << RadiansToDegrees(euler_angles).transpose() << std::endl;
    euler_angles[0] = -CV_PI  + euler_angles[0];
    euler_angles[1] = CV_PI - euler_angles[1];

  }

  std::cout << "X correction " << RadiansToDegrees(euler_angles).transpose() << std::endl;

  euler_angles[1] = euler_angles[1] >= 0 ? euler_angles[1] : CV_2PI + euler_angles[1];

  euler_angles[0] = -euler_angles[0];
  euler_angles[1] = CV_2PI - euler_angles[1];

  std::cout << "Before assert" << RadiansToDegrees(euler_angles).transpose() << std::endl;
  assert(euler_angles[0] >= -20.f * CV_PI / 180.f);
  assert(euler_angles[0] <= CV_PI / 2.f + CV_PI / 180.f);
  assert(euler_angles[1] >= 0.f);
  assert(euler_angles[1] <= CV_2PI);

  return euler_angles;
}


} // anonymous namespace 

Eigen::Vector3f CalculateAbsolutePosition(const cv::Point& point, const CameraIntrinsics& intrinsics,
                                          const Eigen::Vector3f& current_euler_angles) {

  std::cout << "Point: " << point.x << "x" << point.y << std::endl;

  auto current_euler_angles_in_radians = DegreesToRadians(-current_euler_angles);

  const Eigen::Matrix3f current_to_global_rotation =
      EulerAnglesToRotationMatrix(current_euler_angles_in_radians);

  const Eigen::Vector3f point_in_camera_coords = CalculatePointInScreenCoords(point, intrinsics);

  std::cout << "Point in camera: " << point_in_camera_coords.transpose() << std::endl;

  auto point_in_global = current_to_global_rotation * point_in_camera_coords;

  std::cout << "Point in global: " << point_in_global.transpose() << std::endl;
  Eigen::Vector3f z_axis(0., 0., 1.);


  auto global_to_current_location = Eigen::Quaternionf::FromTwoVectors(z_axis, point_in_global).toRotationMatrix();

  Eigen::Vector3f euler_angles = global_to_current_location.eulerAngles(0, 1, 2);
  std::cout << "Computed " << RadiansToDegrees(euler_angles).transpose() << std::endl;

  return RadiansToDegrees(NormalizeAngles({euler_angles[0], euler_angles[1], 0.f}));

}

} // namespace tpxai
