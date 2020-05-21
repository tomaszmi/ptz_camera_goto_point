#include <glog/logging.h>

#include "dahua_ptz_camera.h"
#include "position_calculator.h"

namespace {

struct MouseClickCallbackContext {
  tpxai::dahua::DahuaPTZCamera* ptz_camera = nullptr;
  Eigen::Vector3f current_position; // Euler angles in degrees
};

void OnMouseClickCallback(int event, int x, int y, int/* flags*/, void* userdata) {
  if(event != cv::EVENT_LBUTTONDBLCLK) {
    return;
  }
  auto ctx = static_cast<MouseClickCallbackContext*>(userdata);

  Eigen::Vector3f new_abs_position =
      tpxai::CalculateAbsolutePosition(cv::Point(x, y), ctx->ptz_camera->GetIntrinsics().K, ctx->current_position);
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
  tpxai::dahua::DahuaPTZCamera ptz_camera("admin", "DUPAdupa..", "192.168.1.102", 80);
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
