// ROS2 PORT of ego_planner_node.cpp
// Original: created ros::NodeHandle and called fsm.init(nh)
// ROS2: EGOReplanFSM is itself an rclcpp::Node, spin it directly

#include <rclcpp/rclcpp.hpp>
#include "plan_manage/ego_replan_fsm.h"

int main(int argc, char** argv)
{
  rclcpp::init(argc, argv);

  // ROS2 PORT: EGOReplanFSM inherits rclcpp::Node, init() called in constructor
  auto node = std::make_shared<ego_planner::EGOReplanFSM>();

  // Use MultiThreadedExecutor so timer callbacks and subscriber callbacks
  // run concurrently (replaces ros::AsyncSpinner in ROS1)
  rclcpp::executors::MultiThreadedExecutor executor;
  executor.add_node(node);
  executor.spin();

  rclcpp::shutdown();
  return 0;
}
