cmake_minimum_required(VERSION 3.8)
project(unitree_guide_ros2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)
add_compile_options(-O3 -Wall)

# ── ROS2 dependencies ─────────────────────────────────────────────
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(Eigen3 REQUIRED)

# ── libtorch (RL policy inference) ───────────────────────────────
# Set TORCH_PATH in environment or pass -DTORCH_PATH=/path/to/libtorch
set(TORCH_PATH "$ENV{HOME}/third_party_libs/libtorch" CACHE PATH "Path to libtorch")
find_package(Torch REQUIRED PATHS ${TORCH_PATH})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

# ── unitree_ros2 SDK (replaces unitree_legged_sdk 3.8.0) ─────────
# Must be built and sourced separately from https://github.com/unitreerobotics/unitree_ros2
find_package(unitree_go QUIET)
find_package(unitree_api QUIET)

include_directories(
  include
  ${EIGEN3_INCLUDE_DIR}
  ${TORCH_INCLUDE_DIRS}
)

# ── Controller library ────────────────────────────────────────────
set(CONTROLLER_SOURCES
  src/FSM/FSM.cpp
  src/FSM/State_RL_test.cpp
  src/FSM/State_Trotting.cpp
  src/FSM/State_move_base.cpp
  src/FSM/FSMState.cpp
  src/Gait/GaitGenerator.cpp
  src/Gait/WaveGenerator.cpp
  src/control/BalanceCtrl.cpp
  src/control/Estimator.cpp
  src/common/LowPassFilter.cpp
  src/common/utilities.cpp
)

add_library(unitree_controller SHARED ${CONTROLLER_SOURCES})
ament_target_dependencies(unitree_controller
  rclcpp std_msgs geometry_msgs nav_msgs sensor_msgs tf2 tf2_ros tf2_geometry_msgs)
target_link_libraries(unitree_controller ${TORCH_LIBRARIES})
target_include_directories(unitree_controller PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)

# ── Main node executable ──────────────────────────────────────────
add_executable(unitree_guide_node src/main.cpp)
ament_target_dependencies(unitree_guide_node rclcpp)
target_link_libraries(unitree_guide_node unitree_controller ${TORCH_LIBRARIES})

# ── Simulation odometry publisher ────────────────────────────────
add_executable(state_from_gazebo src/state_from_gazebo.cpp)
ament_target_dependencies(state_from_gazebo rclcpp nav_msgs tf2_ros tf2_geometry_msgs)

install(TARGETS unitree_controller EXPORT export_unitree_controller
  ARCHIVE DESTINATION lib LIBRARY DESTINATION lib RUNTIME DESTINATION bin)
install(TARGETS unitree_guide_node state_from_gazebo
  DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY include/ DESTINATION include)
install(DIRECTORY launch/ DESTINATION share/${PROJECT_NAME}/launch)
install(DIRECTORY config/ DESTINATION share/${PROJECT_NAME}/config)

ament_export_targets(export_unitree_controller HAS_LIBRARY_TARGET)
ament_export_dependencies(rclcpp Eigen3)
ament_package()
