#pragma once
// ROS2 PORT: IOROS2.h replaces IOROS.h
// For simulation: uses /joint_states + /joint_commands (Gazebo ros2_control)
// For real robot: uses unitree_ros2 (Cyclone DDS, https://github.com/unitreerobotics/unitree_ros2)

#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/joint_state.hpp>
#include <std_msgs/msg/float64_multi_array.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include "interface/IOInterface.h"
#include "message/LowlevelState.h"
#include "message/LowlevelCmd.h"

// ── Simulation IO (Gazebo ros2_control bridge) ────────────────────
class IOSIM : public IOInterface {
public:
    explicit IOSIM(rclcpp::Node::SharedPtr node)
    : node_(node)
    {
        // Subscribe to Gazebo joint states
        joint_state_sub_ = node_->create_subscription<sensor_msgs::msg::JointState>(
            "/joint_states", rclcpp::SensorDataQoS(),
            [this](const sensor_msgs::msg::JointState::SharedPtr msg){
                jointStateCallback(msg);
            });

        // Subscribe to odometry from state_from_gazebo
        odom_sub_ = node_->create_subscription<nav_msgs::msg::Odometry>(
            "/Odometry_gazebo", 10,
            [this](const nav_msgs::msg::Odometry::SharedPtr msg){
                odomCallback(msg);
            });

        // Publish joint position commands to Gazebo
        joint_cmd_pub_ = node_->create_publisher<std_msgs::msg::Float64MultiArray>(
            "/a1_joint_group_controller/commands", 10);

        RCLCPP_INFO(node_->get_logger(), "[IOSIM] Gazebo simulation interface initialized");
    }

    void sendRecv(const LowlevelCmd* cmd, LowlevelState* state) override {
        // Send joint commands to Gazebo
        std_msgs::msg::Float64MultiArray cmd_msg;
        for (int i = 0; i < 12; ++i) {
            cmd_msg.data.push_back(cmd->motorCmd[i].q);
        }
        joint_cmd_pub_->publish(cmd_msg);

        // Return latest state (updated by callbacks)
        *state = latest_state_;
    }

    void setPassive() override {
        RCLCPP_WARN(node_->get_logger(), "[IOSIM] setPassive: zeroing joint commands");
        LowlevelCmd zero_cmd;
        std_msgs::msg::Float64MultiArray msg;
        for (int i = 0; i < 12; i++) msg.data.push_back(0.0);
        joint_cmd_pub_->publish(msg);
    }

private:
    rclcpp::Node::SharedPtr node_;
    LowlevelState latest_state_;

    rclcpp::Subscription<sensor_msgs::msg::JointState>::SharedPtr joint_state_sub_;
    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr odom_sub_;
    rclcpp::Publisher<std_msgs::msg::Float64MultiArray>::SharedPtr joint_cmd_pub_;

    // Joint name to index mapping for A1/Go2
    // Order: FL_hip, FL_thigh, FL_calf, FR_hip, ...  (12 joints)
    const std::vector<std::string> JOINT_ORDER = {
        "FL_hip_joint", "FL_thigh_joint", "FL_calf_joint",
        "FR_hip_joint", "FR_thigh_joint", "FR_calf_joint",
        "RL_hip_joint", "RL_thigh_joint", "RL_calf_joint",
        "RR_hip_joint", "RR_thigh_joint", "RR_calf_joint"
    };

    void jointStateCallback(const sensor_msgs::msg::JointState::SharedPtr msg) {
        for (size_t i = 0; i < msg->name.size(); ++i) {
            for (int j = 0; j < 12; ++j) {
                if (msg->name[i] == JOINT_ORDER[j]) {
                    latest_state_.motorState[j].q   = msg->position[i];
                    latest_state_.motorState[j].dq  = msg->velocity.size() > i ? msg->velocity[i] : 0.0;
                    latest_state_.motorState[j].tauEst = msg->effort.size() > i ? msg->effort[i] : 0.0;
                }
            }
        }
    }

    void odomCallback(const nav_msgs::msg::Odometry::SharedPtr msg) {
        // Fill IMU rotation matrix from odometry quaternion
        latest_state_.imu.quaternion[0] = msg->pose.pose.orientation.w;
        latest_state_.imu.quaternion[1] = msg->pose.pose.orientation.x;
        latest_state_.imu.quaternion[2] = msg->pose.pose.orientation.y;
        latest_state_.imu.quaternion[3] = msg->pose.pose.orientation.z;
        latest_state_.imu.gyroscope[0]  = msg->twist.twist.angular.x;
        latest_state_.imu.gyroscope[1]  = msg->twist.twist.angular.y;
        latest_state_.imu.gyroscope[2]  = msg->twist.twist.angular.z;
    }
};

// ── Real Robot IO (unitree_ros2 — Cyclone DDS) ────────────────────
// Uncomment and implement after unitree_ros2 is installed and validated
// https://github.com/unitreerobotics/unitree_ros2
/*
#include <unitree_go/msg/low_state.hpp>
#include <unitree_go/msg/low_cmd.hpp>
#include <unitree_api/msg/request.hpp>

class IOREAL : public IOInterface {
public:
    explicit IOREAL(rclcpp::Node::SharedPtr node) : node_(node) {
        low_state_sub_ = node_->create_subscription<unitree_go::msg::LowState>(
            "/lowstate", 10,
            [this](const unitree_go::msg::LowState::SharedPtr msg){ lowStateCallback(msg); });
        low_cmd_pub_ = node_->create_publisher<unitree_go::msg::LowCmd>("/lowcmd", 10);
        RCLCPP_INFO(node_->get_logger(), "[IOREAL] unitree_ros2 real robot interface initialized");
    }
    void sendRecv(const LowlevelCmd* cmd, LowlevelState* state) override { ... }
    void setPassive() override { ... }
private:
    rclcpp::Node::SharedPtr node_;
    ...
};
*/
