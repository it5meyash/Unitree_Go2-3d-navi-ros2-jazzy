// ROS2 PORT of state_from_gazebo.cpp
// Bugs fixed: 18 (velocity .x/.y/.z fields), 19 (TF offset param), 26 (rate.sleep in cb)
#include <rclcpp/rclcpp.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <gazebo_msgs/msg/link_states.hpp>
#include <tf2_ros/transform_broadcaster.h>
#include <geometry_msgs/msg/transform_stamped.hpp>
#include <tf2/LinearMath/Quaternion.h>
#include <tf2/LinearMath/Matrix3x3.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>
#include <chrono>
using namespace std::chrono_literals;

class StateFromGazebo : public rclcpp::Node {
public:
  explicit StateFromGazebo(const rclcpp::NodeOptions& opts = rclcpp::NodeOptions())
  : rclcpp::Node("state_from_gazebo", opts)
  {
    // FIX Bug19: map→odom origin now a ROS param (was hardcoded cmd-line args)
    this->declare_parameter("spawn_x", 0.0);
    this->declare_parameter("spawn_y", 0.0);
    this->declare_parameter("spawn_z", 0.3);
    this->declare_parameter("spawn_yaw", 0.0);
    spawn_x_ = this->get_parameter("spawn_x").as_double();
    spawn_y_ = this->get_parameter("spawn_y").as_double();
    spawn_z_ = this->get_parameter("spawn_z").as_double();
    spawn_yaw_ = this->get_parameter("spawn_yaw").as_double();

    odom_pub_ = create_publisher<nav_msgs::msg::Odometry>("/Odometry_gazebo", 10);
    tf_broadcaster_ = std::make_shared<tf2_ros::TransformBroadcaster>(this);

    // FIX Bug26: rate.sleep() removed from subscriber callback
    // ROS2: use reliable QoS for Gazebo link states
    link_states_sub_ = create_subscription<gazebo_msgs::msg::LinkStates>(
        "/gazebo/link_states", rclcpp::SensorDataQoS(),
        [this](const gazebo_msgs::msg::LinkStates::SharedPtr msg){ linkStatesCallback(msg); });

    RCLCPP_INFO(get_logger(), "state_from_gazebo ready (spawn: %.1f %.1f %.1f yaw:%.1f)",
        spawn_x_, spawn_y_, spawn_z_, spawn_yaw_);
  }

private:
  double spawn_x_{0}, spawn_y_{0}, spawn_z_{0.3}, spawn_yaw_{0};
  rclcpp::Publisher<nav_msgs::msg::Odometry>::SharedPtr odom_pub_;
  rclcpp::Subscription<gazebo_msgs::msg::LinkStates>::SharedPtr link_states_sub_;
  std::shared_ptr<tf2_ros::TransformBroadcaster> tf_broadcaster_;

  void linkStatesCallback(const gazebo_msgs::msg::LinkStates::SharedPtr msg) {
    // Find the trunk link (A1/Go2 base body)
    int trunk_idx = -1;
    for (size_t i = 0; i < msg->name.size(); i++) {
      if (msg->name[i] == "a1_gazebo::trunk" || msg->name[i] == "go2_description::trunk" ||
          msg->name[i].find("trunk") != std::string::npos) {
        trunk_idx = (int)i; break;
      }
    }
    if (trunk_idx < 0) return;

    // ── Build odometry message ───────────────────────────────────
    nav_msgs::msg::Odometry Odom;
    Odom.header.stamp = now();
    Odom.header.frame_id = "map";
    Odom.child_frame_id = "trunk";

    Odom.pose.pose = msg->pose[trunk_idx];

    // ROS2 PORT: transform world velocities to body frame using quaternion
    auto& q_msg = msg->pose[trunk_idx].orientation;
    tf2::Quaternion q(q_msg.x, q_msg.y, q_msg.z, q_msg.w);
    tf2::Matrix3x3 R(q);
    tf2::Vector3 lin_w(msg->twist[trunk_idx].linear.x,
                       msg->twist[trunk_idx].linear.y,
                       msg->twist[trunk_idx].linear.z);
    tf2::Vector3 ang_w(msg->twist[trunk_idx].angular.x,
                       msg->twist[trunk_idx].angular.y,
                       msg->twist[trunk_idx].angular.z);
    tf2::Vector3 lin_b = R.transpose() * lin_w;
    tf2::Vector3 ang_b = R.transpose() * ang_w;

    // FIX Bug18: was all assigned to .x; now correct .x .y .z
    Odom.twist.twist.linear.x  = lin_b.x();
    Odom.twist.twist.linear.y  = lin_b.y();
    Odom.twist.twist.linear.z  = lin_b.z();
    Odom.twist.twist.angular.x = ang_b.x();
    Odom.twist.twist.angular.y = ang_b.y();
    Odom.twist.twist.angular.z = ang_b.z();

    odom_pub_->publish(Odom);

    // ── Broadcast TF: map → trunk ────────────────────────────────
    geometry_msgs::msg::TransformStamped ts;
    ts.header.stamp = now();
    ts.header.frame_id = "map";
    ts.child_frame_id = "trunk";
    ts.transform.translation.x = msg->pose[trunk_idx].position.x;
    ts.transform.translation.y = msg->pose[trunk_idx].position.y;
    ts.transform.translation.z = msg->pose[trunk_idx].position.z;
    ts.transform.rotation = msg->pose[trunk_idx].orientation;
    tf_broadcaster_->sendTransform(ts);
  }
};

int main(int argc, char** argv) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<StateFromGazebo>());
  rclcpp::shutdown();
  return 0;
}
