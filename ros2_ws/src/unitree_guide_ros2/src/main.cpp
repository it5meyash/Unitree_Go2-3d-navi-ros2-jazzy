// ROS2 PORT of main.cpp
// FIX Bug41: IOFREEDOGSDK guarded by COMPILE_WITH_REAL_ROBOT
#include <rclcpp/rclcpp.hpp>
#include "FSM/FSM.h"
#include "interface/IOROS2.h"      // ROS2 PORT: replaces IOROS.h
#include "common/CtrlComponents.h"

int main(int argc, char** argv)
{
  rclcpp::init(argc, argv);
  auto node = rclcpp::Node::make_shared("unitree_guide_node");

  // ROS2 PORT: IOInterface selects sim vs real at runtime via ROS param
  node->declare_parameter("use_sim", true);
  bool use_sim = node->get_parameter("use_sim").as_bool();

  IOInterface* ioInter = nullptr;
  if (use_sim) {
    // Simulation: reads joint states from Gazebo via /joint_states
    ioInter = new IOSIM(node);
    RCLCPP_INFO(node->get_logger(), "[main] Simulation mode");
  } else {
    // FIX Bug41: Real hardware only when explicitly requested
    // Requires unitree_ros2 to be built and sourced
    // ioInter = new IOREAL(node);  // uncomment after hardware validation
    RCLCPP_ERROR(node->get_logger(), "[main] Real robot mode not yet validated. Set use_sim:=true");
    rclcpp::shutdown(); return 1;
  }

  CtrlComponents* ctrlComp = new CtrlComponents(ioInter);
  ctrlComp->dt = 0.002;  // 500 Hz control loop

  // Start FSM in separate thread (500 Hz control loop)
  FSM fsm(ctrlComp, node);

  rclcpp::executors::MultiThreadedExecutor executor;
  executor.add_node(node);

  // FSM runs its own real-time thread; spin handles ROS callbacks
  std::thread fsm_thread([&fsm](){ fsm.run(); });
  executor.spin();
  fsm_thread.join();

  delete ctrlComp;
  delete ioInter;
  rclcpp::shutdown();
  return 0;
}
