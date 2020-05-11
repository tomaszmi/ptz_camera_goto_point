# Content.

A simple application centering the Dahua PTZ camera on a double-clicked point in the camera preview window.

The main logic is inside the **main.cpp** file. The remaining files contain functionality controlling the
camera, i.e. expose simple interface (**dahua_ptz_camera.h**) and hide unnecessary details.

# How to use the application.

After running the application a window with the camera preview is opened. Double click at any place
inside that window centers the PTZ camera at that point. At the application start the camera is moved to the *point zero* which
corresponds to horizontal and vertical angles equal `0`. Pressing `q` while the window is focused quits the application.

# How to build the application.

## Requirements:

1. Linux Ubuntu (checked on 19.10 and 20.04),
2. *cmake*, build system generator (sudo apt install cmake),
3. *GNU Makefile* (sudo apt install make),
4. *OpenCV4*, mostly for data structures (sudo apt install libopencv-dev),
5. *Boost* C++ libraries, for string processing (sudo apt install libboost-all-dev),
6. *libcurl*, for performing HTTP communication (sudo apt install libcurl4),
7. *Eigen3*, for algebra (sudo apt install libeigen3-dev),
8. *Glog*, logging library (sudo apt install libgoogle-glog-dev),

## Building.

```bash
  git clone
  mkdir build
  cd build
  cmake ..
  make
```

## Running.

```bash
./goto_point
```
