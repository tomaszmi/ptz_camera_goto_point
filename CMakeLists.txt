cmake_minimum_required(VERSION 3.10)
project("goto_point")

set(MAJOR_VERSION 0)
set(MINOR_VERSION 1)
set(PATCH_VERSION 0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -ggdb")
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wno-deprecated -Wno-deprecated-declarations")

include_directories(${CMAKE_SOURCE_DIR})

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}")

include(GTest)

find_package(OpenCV REQUIRED core highgui)
find_package(CURL REQUIRED)
find_package(Eigen3 3.3 REQUIRED NO_MODULE)
find_package(Glog REQUIRED)

add_library(inventory
  position_calculator.cpp
  curl_error_category.cpp
  dahua_error_category.cpp
  dahua_ptz_camera.cpp
  http_interface.cpp
)

target_include_directories(inventory SYSTEM
  PRIVATE ${OpenCV_INCLUDE_DIRS}
  PRIVATE ${CURL_INCLUDE_DIRS}
)

target_link_libraries(inventory
  ${OpenCV_LIBS}
  ${CURL_LIBRARIES}
  ${GLOG_LIBRARIES}
  Eigen3::Eigen
)

add_executable(goto_point
  main.cpp
)

target_link_libraries(goto_point
  inventory
)

cxx_test(inventory_test tests/position_calculator_test.cpp inventory)
