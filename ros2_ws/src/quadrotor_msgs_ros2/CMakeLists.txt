cmake_minimum_required(VERSION 3.8)
project(quadrotor_msgs_ros2)

find_package(ament_cmake REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(std_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)

# Only the messages actually used by EGO Planner + traj_server
rosidl_generate_interfaces(${PROJECT_NAME}
  "msg/PositionCommand.msg"
  "msg/PolynomialTrajectory.msg"
  "msg/Bspline.msg"
  "msg/DataDisp.msg"
  DEPENDENCIES std_msgs geometry_msgs
)

ament_export_dependencies(rosidl_default_runtime)
ament_package()
