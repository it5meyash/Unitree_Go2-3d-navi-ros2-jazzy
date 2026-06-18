# 3d-navi ROS2 Jazzy Workspace

Full ROS2 Jazzy port of the 3d-navi autonomous navigation stack for Unitree A2/Go2.

## Package structure

| Package | Description |
|---|---|
| `quadrotor_msgs_ros2` | Custom message interfaces (Bspline, PositionCommand, DataDisp) |
| `path_searching` | A* path search (Bug27 fixed: inf=0→infinity) |
| `bspline_opt` | B-spline trajectory optimizer |
| `plan_env` | Occupancy grid map (Bug28 fixed: non-blocking TF) |
| `traj_utils` | Planning visualization utilities |
| `ego_planner` | EGO Planner node + traj_server |
| `unitree_guide_ros2` | FSM locomotion controller + RL policy |
| `pct_planner` | PCT Point Cloud Tomography planner (Python) |
| `3d_navi_bringup` | Master launch, Nav2 config, RViz |

## Build

```bash
cd ~/ros2_ws
source /opt/ros/jazzy/setup.bash

# Install deps
sudo apt install -y \
  ros-jazzy-tf2-ros ros-jazzy-tf2-geometry-msgs \
  ros-jazzy-pcl-ros ros-jazzy-pcl-conversions \
  ros-jazzy-nav2-bringup ros-jazzy-ros-gz-bridge \
  ros-jazzy-ros-gz-sim \
  libeigen3-dev libpcl-dev

colcon build --symlink-install --cmake-args -DCMAKE_BUILD_TYPE=Release
source install/setup.bash
```

## External packages (build separately first)

```bash
# 1. FAST-LIO2 ROS2 branch
mkdir -p ~/fastlio_ws/src && cd ~/fastlio_ws/src
git clone -b ROS2 https://github.com/hku-mars/FAST_LIO.git
cd ~/fastlio_ws && colcon build

# 2. livox_ros_driver2 ROS2
mkdir -p ~/livox_ws/src && cd ~/livox_ws/src
git clone https://github.com/Livox-SDK/livox_ros_driver2.git
cd ~/livox_ws && colcon build

# 3. unitree_ros2 (for real robot only)
# https://github.com/unitreerobotics/unitree_ros2
```

## Launch (simulation)

```bash
# Full stack — one command
ros2 launch 3d_navi_bringup 3d_navi_sim.launch.py

# With custom policy
ros2 launch 3d_navi_bringup 3d_navi_sim.launch.py \
  policy_path:=/path/to/policy_flat.pt
```

## Send navigation goal

```bash
ros2 topic pub /goal_pose geometry_msgs/msg/PoseStamped "
header:
  frame_id: map
pose:
  position: {x: 5.0, y: 3.0, z: 0.0}
  orientation: {w: 1.0}" --once
```

## All 40 bugs fixed

See `APPLY_PATCHES_FINAL.md` for complete bug list.
All patches are incorporated directly into this ROS2 port.
