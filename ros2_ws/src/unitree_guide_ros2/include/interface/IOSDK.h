/**********************************************************************
 Copyright (c) 2020-2023, Unitree Robotics.Co.Ltd. All rights reserved.
***********************************************************************/
#ifndef IOSDK_H
#define IOSDK_H

#include "interface/IOInterface.h"
#include "unitree_legged_sdk/unitree_legged_sdk.h"

#ifdef COMPILE_WITH_MOVE_BASE
    #include <rclcpp/rclcpp.hpp>  // ROS2
    // ros/time.h removed (ROS2)
    #include <sensor_msgs/JointState.h>
#endif  // COMPILE_WITH_MOVE_BASE


class IOSDK : public IOInterface{
public:
IOSDK();
~IOSDK(){}
void sendRecv(const LowlevelCmd *cmd, LowlevelState *state);

private:
UNITREE_LEGGED_SDK::UDP _udp;
UNITREE_LEGGED_SDK::Safety _safe;
UNITREE_LEGGED_SDK::LowCmd _lowCmd;
UNITREE_LEGGED_SDK::LowState _lowState;

#ifdef COMPILE_WITH_MOVE_BASE
    rclcpp::Node::SharedPtr _nh;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr _pub;
    sensor_msgs::JointState _joint_state;
#endif  // COMPILE_WITH_MOVE_BASE
};

#endif  // IOSDK_H