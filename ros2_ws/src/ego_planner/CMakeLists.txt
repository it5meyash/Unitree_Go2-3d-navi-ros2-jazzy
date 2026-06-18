cmake_minimum_required(VERSION 3.8)
project(ego_planner)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)
add_compile_options(-O3 -Wall)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_components REQUIRED)
find_package(std_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(visualization_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(quadrotor_msgs_ros2 REQUIRED)
find_package(bspline_opt REQUIRED)
find_package(path_searching REQUIRED)
find_package(plan_env REQUIRED)
find_package(traj_utils REQUIRED)
find_package(Eigen3 REQUIRED)

include_directories(include ${EIGEN3_INCLUDE_DIR})

# Planner manager library
add_library(planner_manager SHARED src/planner_manager.cpp)
ament_target_dependencies(planner_manager rclcpp bspline_opt path_searching plan_env traj_utils Eigen3)
target_include_directories(planner_manager PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)

# EGO Planner node (replan FSM)
add_executable(ego_planner_node
  src/ego_planner_node.cpp
  src/ego_replan_fsm.cpp
)
ament_target_dependencies(ego_planner_node
  rclcpp std_msgs geometry_msgs nav_msgs sensor_msgs visualization_msgs
  tf2 tf2_ros tf2_geometry_msgs quadrotor_msgs_ros2
  bspline_opt path_searching plan_env traj_utils)
target_include_directories(ego_planner_node PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_link_libraries(ego_planner_node planner_manager)

# Trajectory server node
add_executable(traj_server src/traj_server.cpp)
ament_target_dependencies(traj_server
  rclcpp std_msgs geometry_msgs nav_msgs
  tf2 tf2_ros tf2_geometry_msgs quadrotor_msgs_ros2)
target_include_directories(traj_server PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_link_libraries(traj_server planner_manager)

install(TARGETS planner_manager EXPORT export_planner_manager
  ARCHIVE DESTINATION lib LIBRARY DESTINATION lib RUNTIME DESTINATION bin)
install(TARGETS ego_planner_node traj_server DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY include/ DESTINATION include)
install(DIRECTORY launch/ DESTINATION share/${PROJECT_NAME}/launch)
install(DIRECTORY config/ DESTINATION share/${PROJECT_NAME}/config)

ament_export_targets(export_planner_manager HAS_LIBRARY_TARGET)
ament_export_dependencies(rclcpp bspline_opt path_searching plan_env traj_utils)
ament_package()
