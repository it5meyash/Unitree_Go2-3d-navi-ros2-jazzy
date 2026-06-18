#ifndef _REBO_REPLAN_FSM_H_
#define _REBO_REPLAN_FSM_H_

// ROS2 PORT: ros/ros.h → rclcpp, nav_msgs::Path → nav_msgs::msg::Path, etc.
#include <rclcpp/rclcpp.hpp>
#include <Eigen/Eigen>
#include <algorithm>
#include <iostream>
#include <vector>

#include <nav_msgs/msg/path.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <sensor_msgs/msg/imu.hpp>
#include <std_msgs/msg/empty.hpp>
#include <std_msgs/msg/int16.hpp>
#include <visualization_msgs/msg/marker.hpp>

// ROS2 PORT: tf → tf2_ros
#include <tf2_ros/buffer.h>
#include <tf2_ros/transform_listener.h>

#include <bspline_opt/bspline_optimizer.h>
#include <plan_env/grid_map.h>
#include <quadrotor_msgs_ros2/msg/bspline.hpp>
#include <quadrotor_msgs_ros2/msg/data_disp.hpp>
#include <plan_manage/planner_manager.h>
#include <traj_utils/planning_visualization.h>

using std::vector;

namespace ego_planner
{
  class EGOReplanFSM : public rclcpp::Node  // ROS2 PORT: inherit from rclcpp::Node
  {
  private:
    int MAX_EMERGENCY_STOP{5};
    enum FSM_EXEC_STATE {
      INIT, WAIT_TARGET, GEN_NEW_TRAJ, REPLAN_TRAJ, EXEC_TRAJ, EMERGENCY_STOP
    };
    enum TARGET_TYPE {
      MANUAL_TARGET = 1, PRESET_TARGET = 2, REFENCE_PATH = 3
    };

    EGOPlannerManager::Ptr planner_manager_;
    PlanningVisualization::Ptr visualization_;
    quadrotor_msgs_ros2::msg::DataDisp data_disp_;

    int target_type_;
    double no_replan_thresh_, replan_thresh_;
    double waypoints_[50][3];
    int waypoint_num_;
    double planning_horizen_, planning_horizen_time_;
    double emergency_time_;

    bool trigger_, have_target_, have_odom_, have_new_target_;
    std::string odom_topic_;

    std_msgs::msg::Int16 goFlag_;

    FSM_EXEC_STATE exec_state_;
    int continously_called_times_{0};

    // ROS2 PORT: tf2_ros buffer+listener
    std::shared_ptr<tf2_ros::Buffer> tf_buffer_;
    std::shared_ptr<tf2_ros::TransformListener> tf_listener_;

    Eigen::Vector3d odom_pos_, odom_vel_, odom_acc_;
    Eigen::Quaterniond odom_orient_;

    Eigen::Vector3d init_pt_, start_pt_, start_vel_, start_acc_, start_yaw_;
    Eigen::Vector3d end_pt_, end_vel_;
    Eigen::Vector3d local_target_pt_, local_target_vel_;
    int current_wp_;

    bool flag_escape_emergency_;

    // ROS2 PORT: rclcpp::TimerBase replaces ros::Timer
    rclcpp::TimerBase::SharedPtr exec_timer_, safety_timer_, go_timer_;

    // ROS2 PORT: typed Subscription + Publisher
    rclcpp::Subscription<nav_msgs::msg::Path>::SharedPtr waypoint_sub_, path_sub_;
    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr odom_sub_;
    rclcpp::Publisher<std_msgs::msg::Empty>::SharedPtr replan_pub_, new_pub_;
    rclcpp::Publisher<quadrotor_msgs_ros2::msg::Bspline>::SharedPtr bspline_pub_;
    rclcpp::Publisher<quadrotor_msgs_ros2::msg::DataDisp>::SharedPtr data_disp_pub_;
    rclcpp::Publisher<std_msgs::msg::Int16>::SharedPtr go_flag_pub_;

    bool callReboundReplan(bool flag_use_poly_init, bool flag_randomPolyTraj);
    bool callEmergencyStop(Eigen::Vector3d stop_pos);
    bool planFromCurrentTraj();

    void changeFSMExecState(FSM_EXEC_STATE new_state, std::string pos_call);
    std::pair<int, EGOReplanFSM::FSM_EXEC_STATE> timesOfConsecutiveStateCalls();
    void printFSMExecState();
    void planGlobalTrajbyGivenWps();
    void getLocalTarget();

    // ROS2 PORT: callbacks no longer take TimerEvent — use plain void()
    void execFSMCallback();
    void checkCollisionCallback();
    void waypointCallback(const nav_msgs::msg::Path::SharedPtr msg);
    void odometryCallback(const nav_msgs::msg::Odometry::SharedPtr msg);
    void pathCallback(const nav_msgs::msg::Path::SharedPtr msg);

  public:
    // ROS2 PORT: constructor takes NodeOptions for composability
    explicit EGOReplanFSM(const rclcpp::NodeOptions & options = rclcpp::NodeOptions());
    ~EGOReplanFSM() {}

    void init();

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
  };

} // namespace ego_planner

#endif
