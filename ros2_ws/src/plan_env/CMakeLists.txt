cmake_minimum_required(VERSION 3.8)
project(plan_env)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)
add_compile_options(-O3 -Wall)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(pcl_ros REQUIRED)
find_package(pcl_conversions REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(PCL REQUIRED)

include_directories(include ${EIGEN3_INCLUDE_DIR} ${PCL_INCLUDE_DIRS})

add_library(${PROJECT_NAME} SHARED
  src/grid_map.cpp
  src/raycast.cpp
)
ament_target_dependencies(${PROJECT_NAME}
  rclcpp sensor_msgs geometry_msgs nav_msgs
  tf2_ros tf2_geometry_msgs pcl_ros pcl_conversions)
target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_link_libraries(${PROJECT_NAME} ${PCL_LIBRARIES})

install(TARGETS ${PROJECT_NAME} EXPORT export_${PROJECT_NAME}
  ARCHIVE DESTINATION lib LIBRARY DESTINATION lib RUNTIME DESTINATION bin)
install(DIRECTORY include/ DESTINATION include)

ament_export_targets(export_${PROJECT_NAME} HAS_LIBRARY_TARGET)
ament_export_dependencies(rclcpp sensor_msgs geometry_msgs nav_msgs tf2_ros pcl_ros Eigen3)
ament_package()
