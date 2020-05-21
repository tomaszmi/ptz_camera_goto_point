#include <gtest/gtest.h>

#include "position_calculator.h"

const tpxai::CameraIntrinsics dahua_intrinsics{
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

TEST(PositionCalculator, initial) {
  Eigen::Vector3f result = tpxai::CalculateAbsolutePosition({2592/2, 1520/2}, dahua_intrinsics.K, {0, 0, 0});
  //std::cout << result << std::endl;
  (void)result;
}
