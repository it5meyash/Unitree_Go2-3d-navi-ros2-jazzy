"""
3d_navi_sim.launch.py — One-command launch for full simulation stack
ROS2 Jazzy | Gazebo Harmonic | Unitree A1/Go2 | MID360 LiDAR

Launch order (with delays to allow each component to initialise):
  t=0s  : Gazebo Harmonic + robot model
  t=3s  : ros_gz_bridge (Gazebo↔ROS2 topic bridge)
  t=4s  : state_from_gazebo (publishes /Odometry_gazebo)
  t=5s  : FAST-LIO2 (LiDAR-inertial odometry → /cloud_registered)
  t=8s  : EGO Planner + traj_server
  t=10s : unitree_guide controller (FSM + RL)
  t=12s : PCT planner (Python, needs FAST-LIO map)
  t=14s : RViz2

Usage:
  ros2 launch 3d_navi_bringup 3d_navi_sim.launch.py
  ros2 launch 3d_navi_bringup 3d_navi_sim.launch.py policy_path:=/path/to/flat.pt
"""
import os
from launch import LaunchDescription
from launch.actions import (DeclareLaunchArgument, IncludeLaunchDescription,
                             TimerAction, ExecuteProcess)
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare

def generate_launch_description():
    pkg_bringup  = FindPackageShare('3d_navi_bringup')
    pkg_unitree  = FindPackageShare('unitree_guide_ros2')
    pkg_planner  = FindPackageShare('ego_planner')

    # ── Args ─────────────────────────────────────────────────────
    policy_path = DeclareLaunchArgument(
        'policy_path',
        default_value=os.path.expanduser(
            '~/3d-navi/src/unitree_guide/logs/policy_act_inference_flat.pt'),
        description='RL policy .pt file path (FIX Bug10: was hardcoded stair policy)')
    use_sim_time = DeclareLaunchArgument('use_sim_time', default_value='true')
    odom_topic   = DeclareLaunchArgument('odom_topic',  default_value='/Odometry_gazebo')
    max_vel      = DeclareLaunchArgument('max_vel',     default_value='0.6')
    max_acc      = DeclareLaunchArgument('max_acc',     default_value='0.8')

    # ── t=0: Gazebo Harmonic ─────────────────────────────────────
    gazebo_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            PathJoinSubstitution([pkg_unitree, 'launch', 'gazebo_sim.launch.py'])),
        launch_arguments={
            'use_sim_time': LaunchConfiguration('use_sim_time'),
            'policy_path':  LaunchConfiguration('policy_path'),
        }.items()
    )

    # ── t=5: FAST-LIO2 (external package, must be built separately) ──
    # Install: https://github.com/hku-mars/FAST_LIO/tree/ROS2
    fast_lio = TimerAction(period=5.0, actions=[
        Node(
            package='fast_lio',
            executable='fastlio_mapping',
            name='fast_lio',
            parameters=[
                PathJoinSubstitution([pkg_bringup, 'config', 'mid360.yaml'])
            ],
            remappings=[
                ('/cloud_registered', '/cloud_registered'),
                ('/Odometry',         '/Odometry_gazebo'),   # FIX Bug2: unified topic
            ],
            output='screen'
        )
    ])

    # ── t=8: EGO Planner + traj_server ───────────────────────────
    ego_planner = TimerAction(period=8.0, actions=[
        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(
                PathJoinSubstitution([pkg_planner, 'launch', 'ego_planner.launch.py'])),
            launch_arguments={
                'use_sim_time': LaunchConfiguration('use_sim_time'),
                'odom_topic':   LaunchConfiguration('odom_topic'),
                'max_vel':      LaunchConfiguration('max_vel'),
                'max_acc':      LaunchConfiguration('max_acc'),
            }.items()
        )
    ])

    # ── t=12: PCT Planner (Python rclpy node) ────────────────────
    pct = TimerAction(period=12.0, actions=[
        Node(
            package='pct_planner',
            executable='plan_node.py',
            name='pct_planner',
            parameters=[{
                'use_sim_time': LaunchConfiguration('use_sim_time'),
                'odom_topic':   LaunchConfiguration('odom_topic'),
                'scene': 'building',
            }],
            output='screen'
        )
    ])

    # ── t=14: RViz2 ──────────────────────────────────────────────
    rviz = TimerAction(period=14.0, actions=[
        Node(
            package='rviz2',
            executable='rviz2',
            name='rviz2',
            arguments=['-d', PathJoinSubstitution([pkg_bringup, 'rviz', '3d_navi.rviz'])],
            parameters=[{'use_sim_time': LaunchConfiguration('use_sim_time')}],
            output='screen'
        )
    ])

    return LaunchDescription([
        policy_path, use_sim_time, odom_topic, max_vel, max_acc,
        gazebo_launch,
        fast_lio,
        ego_planner,
        pct,
        rviz,
    ])
