"""
ROS2 PORT: run_in_sim.launch → ego_planner.launch.py
Bugs fixed:
  Bug2:  odom_topic unified to /Odometry_gazebo
  Bug3:  cloud_topic switched to /cloud_registered
  Bug7:  obstacles_inflation 0.1→0.35m
  Bug28: TF non-blocking (in code)
  Bug30: map 50x50x50 → 20x20x4m (saves 1.88GB RAM)
  Bug_Ground: ground_height -0.1→-0.5m
"""
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare

def generate_launch_description():
    pkg = FindPackageShare('ego_planner')

    use_sim_time  = DeclareLaunchArgument('use_sim_time', default_value='true')
    odom_topic    = DeclareLaunchArgument('odom_topic',   default_value='/Odometry_gazebo')   # FIX Bug2
    cloud_topic   = DeclareLaunchArgument('cloud_topic',  default_value='/cloud_registered')  # FIX Bug3
    max_vel       = DeclareLaunchArgument('max_vel',      default_value='0.6')   # safe GO2 speed
    max_acc       = DeclareLaunchArgument('max_acc',      default_value='0.8')   # FIX Bug37
    flight_type   = DeclareLaunchArgument('flight_type',  default_value='3')     # REFENCE_PATH mode

    ego_planner = Node(
        package='ego_planner',
        executable='ego_planner_node',
        name='ego_planner_node',
        output='screen',
        parameters=[{
            'use_sim_time': LaunchConfiguration('use_sim_time'),

            # Odometry (FIX Bug2)
            'fsm/odometry_topic': LaunchConfiguration('odom_topic'),
            'grid_map/odom_topic': LaunchConfiguration('odom_topic'),

            # Cloud (FIX Bug3)
            'grid_map/cloud_topic': LaunchConfiguration('cloud_topic'),
            'grid_map/lidar_frame_id': 'camera_init',

            # Map size (FIX Bug30: was 50x50x50=1.88GB)
            'grid_map/map_size_x': 20.0,
            'grid_map/map_size_y': 20.0,
            'grid_map/map_size_z':  4.0,
            'grid_map/resolution': 0.1,

            # Inflation (FIX Bug7 + Bug25)
            'grid_map/obstacles_inflation': 0.35,  # GO2 body width ~0.4m

            # Ground/ceiling (FIX Bug_Ground)
            'grid_map/ground_height': -0.5,        # capture MID360 downward scans
            'grid_map/virtual_ceil_height': 3.0,   # indoor ceiling

            # Planner dynamics (FIX Bug37: was drone 2.0/3.0)
            'optimization/max_vel': LaunchConfiguration('max_vel'),
            'optimization/max_acc': LaunchConfiguration('max_acc'),

            # EGO FSM
            'fsm/flight_type': LaunchConfiguration('flight_type'),
            'fsm/planning_horizon': 7.5,
            'fsm/emergency_time': 1.0,
            'fsm/no_replan_thresh': 0.5,
            'fsm/replan_thresh': 1.5,

            # Bspline
            'optimization/bspline_degree': 3,
            'optimization/dist0': 0.8,
            'optimization/lambda_z_penalty': 1.2,
            'optimization/obstacles_inflation': 0.35,  # FIX Bug7

            # PID (FIX Bug12d: traj_server params now correctly namespaced)
            'traj_server/odometry_topic': LaunchConfiguration('odom_topic'),
            'traj_server/kp_yaw': 1.5,
            'traj_server/ki_yaw': 0.0,
            'traj_server/kd_yaw': 0.3,
        }]
    )

    traj_server = Node(
        package='ego_planner',
        executable='traj_server',
        name='traj_server',
        output='screen',
        parameters=[{
            'use_sim_time': LaunchConfiguration('use_sim_time'),
            'odometry_topic': LaunchConfiguration('odom_topic'),  # FIX Bug12d
            'kp_yaw': 1.5,
            'ki_yaw': 0.0,
            'kd_yaw': 0.3,
        }]
    )

    return LaunchDescription([
        use_sim_time, odom_topic, cloud_topic, max_vel, max_acc, flight_type,
        ego_planner,
        traj_server,
    ])
