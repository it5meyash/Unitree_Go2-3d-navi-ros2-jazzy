cmake_minimum_required(VERSION 3.8)
project(traj_utils)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)
add_compile_options(-O3 -Wall)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(visualization_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(bspline_opt REQUIRED)

include_directories(include ${EIGEN3_INCLUDE_DIR})

add_library(${PROJECT_NAME} SHARED
  src/planning_visualization.cpp
  src/polynomial_traj.cpp
)
ament_target_dependencies(${PROJECT_NAME} rclcpp visualization_msgs geometry_msgs nav_msgs bspline_opt)
target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)

install(TARGETS ${PROJECT_NAME} EXPORT export_${PROJECT_NAME}
  ARCHIVE DESTINATION lib LIBRARY DESTINATION lib RUNTIME DESTINATION bin)
install(DIRECTORY include/ DESTINATION include)
ament_export_targets(export_${PROJECT_NAME} HAS_LIBRARY_TARGET)
ament_export_dependencies(rclcpp visualization_msgs bspline_opt Eigen3)
ament_package()
