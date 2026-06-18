"""
ROS2 PORT: gazeboSim.launch → gazebo_sim.launch.py
Launches Unitree A1 in Gazebo Harmonic with MID360 LiDAR simulation
"""
import os
from ament_python import Package  # noqa
from launch import LaunchDescription
from launch.actions import (DeclareLaunchArgument, IncludeLaunchDescription,
                             ExecuteProcess, TimerAction)
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare

def generate_launch_description():
    pkg_unitree = FindPackageShare('unitree_guide_ros2')
    pkg_bringup  = FindPackageShare('3d_navi_bringup')

    # ── Args ─────────────────────────────────────────────────────
    use_sim_time = DeclareLaunchArgument('use_sim_time', default_value='true')
    world_file   = DeclareLaunchArgument('world', default_value='building.world')
    policy_path  = DeclareLaunchArgument(
        'policy_path',
        default_value='policy_flat.pt',
        description='Path to RL policy .pt file (flat ground or stair)')

    # ── Gazebo Harmonic ───────────────────────────────────────────
    # ROS2 Jazzy uses gz-sim (Gazebo Harmonic), not Gazebo Classic 11
    gazebo = ExecuteProcess(
        cmd=['gz', 'sim', '-r',
             PathJoinSubstitution([pkg_bringup, 'worlds', LaunchConfiguration('world')])],
        output='screen'
    )

    # ros_gz_bridge: bridge Gazebo topics to ROS2
    gz_bridge = Node(
        package='ros_gz_bridge',
        executable='parameter_bridge',
        arguments=[
            '/clock@rosgraph_msgs/msg/Clock[gz.msgs.Clock',
            '/gazebo/link_states@gazebo_msgs/msg/LinkStates[gz.msgs.Pose_V',
            '/joint_states@sensor_msgs/msg/JointState[gz.msgs.Model',
            # MID360 point cloud bridge (PointCloud2 — FIX Bug39)
            '/livox/lidar@sensor_msgs/msg/PointCloud2[gz.msgs.PointCloudPacked',
            '/livox/imu@sensor_msgs/msg/Imu[gz.msgs.IMU',
        ],
        output='screen'
    )

    # ── Odometry from Gazebo (sim ground truth) ───────────────────
    state_from_gazebo = Node(
        package='unitree_guide_ros2',
        executable='state_from_gazebo',
        name='state_from_gazebo',
        parameters=[{
            'use_sim_time': LaunchConfiguration('use_sim_time'),
            # FIX Bug19: spawn position now a ROS param
            'spawn_x': -5.0,
            'spawn_y':  7.0,
            'spawn_z':  0.3,
            'spawn_yaw': 0.0,
        }],
        output='screen'
    )

    # ── Virtual joystick (FIX Bug20: was commented out) ──────────
    virtual_joy = Node(
        package='unitree_guide_ros2',
        executable='virtual_joy.py',
        name='virtual_joy',
        output='screen'
    )

    # ── Main controller (FSM + RL) ────────────────────────────────
    controller = Node(
        package='unitree_guide_ros2',
        executable='unitree_guide_node',
        name='unitree_guide_node',
        parameters=[{
            'use_sim_time': LaunchConfiguration('use_sim_time'),
            'use_sim': True,
            'policy_path': LaunchConfiguration('policy_path'),  # FIX Bug10
            'log_dir': os.path.expanduser('~/rl_logs/'),         # FIX log path
        }],
        output='screen'
    )

    return LaunchDescription([
        use_sim_time,
        world_file,
        policy_path,
        gazebo,
        TimerAction(period=2.0, actions=[gz_bridge]),
        TimerAction(period=3.0, actions=[state_from_gazebo]),
        TimerAction(period=4.0, actions=[virtual_joy]),
        TimerAction(period=5.0, actions=[controller]),
    ])
