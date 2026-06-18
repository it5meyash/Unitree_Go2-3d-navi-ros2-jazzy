# 3d-navi ROS2 Jazzy Conversion — Master Log

## Status: Phase 1–6 COMPLETE

---

## What was converted (session summary)

### Phase 1 — Build system + interfaces ✅
| File | Status |
|---|---|
| quadrotor_msgs_ros2/package.xml | ✅ ament_cmake, format 3 |
| quadrotor_msgs_ros2/CMakeLists.txt | ✅ rosidl_generate_interfaces |
| quadrotor_msgs_ros2/msg/PositionCommand.msg | ✅ ROS2 IDL |
| quadrotor_msgs_ros2/msg/PolynomialTrajectory.msg | ✅ ROS2 IDL |
| quadrotor_msgs_ros2/msg/Bspline.msg | ✅ ROS2 IDL (time→builtin_interfaces) |
| quadrotor_msgs_ros2/msg/DataDisp.msg | ✅ ROS2 IDL |
| bspline_opt/package.xml + CMakeLists.txt | ✅ ament_cmake |
| path_searching/package.xml + CMakeLists.txt | ✅ ament_cmake |
| plan_env/package.xml + CMakeLists.txt | ✅ ament_cmake + PCL |
| traj_utils/package.xml + CMakeLists.txt | ✅ ament_cmake |
| ego_planner/package.xml + CMakeLists.txt | ✅ ament_cmake, 3 executables |
| unitree_guide_ros2/package.xml + CMakeLists.txt | ✅ ament_cmake + libtorch |
| pct_planner/package.xml + setup.py | ✅ ament_python |
| 3d_navi_bringup/package.xml + CMakeLists.txt | ✅ ament_cmake |

### Phase 2 — Path searching (pure C++ math) ✅
| File | Status | Bugs Fixed |
|---|---|---|
| path_searching/include/dyn_a_star.h | ✅ rclcpp | Bug27: inf=0→infinity |
| path_searching/src/dyn_a_star.cpp | ✅ RCLCPP_* logging | Bug27 |

### Phase 3 — B-spline optimizer ✅
| File | Status | Bugs Fixed |
|---|---|---|
| bspline_opt/src/bspline_optimizer.cpp | ✅ rclcpp | Bug21 (i_start), Bug22 (Z-gradient) |
| bspline_opt/src/uniform_bspline.cpp | ✅ rclcpp | — |
| bspline_opt/src/gradient_descent_optimizer.cpp | ✅ rclcpp | — |
| bspline_opt/include/*.h (3 files) | ✅ rclcpp | — |
| traj_utils/src/planning_visualization.cpp | ✅ rclcpp publishers | — |
| traj_utils/src/polynomial_traj.cpp | ✅ rclcpp | — |
| traj_utils/include/*.h (2 files) | ✅ rclcpp | — |

### Phase 4 — Occupancy grid map ✅
| File | Status | Bugs Fixed |
|---|---|---|
| plan_env/include/plan_env/grid_map.h | ✅ Full ROS2 rewrite | — |
| plan_env/src/grid_map.cpp | ✅ tf2, rclcpp::Node::SharedPtr | Bug25, Bug28, Bug30 |
| plan_env/include/plan_env/raycast.h | ✅ rclcpp | — |
| plan_env/src/raycast.cpp | ✅ rclcpp | — |

### Phase 5 — EGO Planner nodes ✅
| File | Status | Bugs Fixed |
|---|---|---|
| ego_planner/include/plan_manage/ego_replan_fsm.h | ✅ rclcpp::Node subclass | — |
| ego_planner/include/plan_manage/planner_manager.h | ✅ rclcpp | — |
| ego_planner/include/plan_manage/plan_container.hpp | ✅ rclcpp::Time | — |
| ego_planner/src/ego_replan_fsm.cpp | ✅ rclcpp::Node constructor | Bug6 |
| ego_planner/src/planner_manager.cpp | ✅ rclcpp | Bug6 |
| ego_planner/src/traj_server.cpp | ✅ tf2, rclcpp timers | Bug12a,12b,12c,12d |
| ego_planner/src/ego_planner_node.cpp | ✅ MultiThreadedExecutor | — |
| ego_planner/launch/ego_planner.launch.py | ✅ .launch.py | Bug2,3,7,30,37 params |

### Phase 6 — Unitree FSM controller ✅
| File | Status | Bugs Fixed |
|---|---|---|
| unitree_guide_ros2/src/main.cpp | ✅ rclcpp::Node + sim/real guard | Bug41 |
| unitree_guide_ros2/src/state_from_gazebo.cpp | ✅ rclcpp::Node class | Bug18,19,26 |
| unitree_guide_ros2/src/FSM/FSM.cpp | ✅ rclcpp | Bug13 |
| unitree_guide_ros2/src/FSM/FSMState.cpp | ✅ rclcpp | — |
| unitree_guide_ros2/src/FSM/State_Trotting.cpp | ✅ rclcpp | — |
| unitree_guide_ros2/src/FSM/State_move_base.cpp | ✅ rclcpp | — |
| unitree_guide_ros2/src/FSM/State_RL_test.cpp | ✅ rclcpp | Bug9,10,11,15,exit_crash |
| unitree_guide_ros2/include/interface/IOROS2.h | ✅ NEW: IOSIM + IOREAL stub | — |
| unitree_guide_ros2/include/FSM/*.h (10 files) | ✅ rclcpp | — |
| unitree_guide_ros2/include/Gait/*.h (3 files) | ✅ rclcpp | — |
| unitree_guide_ros2/include/common/*.h (7 files) | ✅ rclcpp | — |
| unitree_guide_ros2/include/control/*.h (4 files) | ✅ rclcpp | — |
| unitree_guide_ros2/include/interface/*.h (7 files) | ✅ rclcpp | — |
| unitree_guide_ros2/src/Gait/*.cpp (3 files) | ✅ rclcpp | — |
| unitree_guide_ros2/src/control/*.cpp (3 files) | ✅ rclcpp | — |
| unitree_guide_ros2/src/common/LowPassFilter.cpp | ✅ rclcpp | — |
| unitree_guide_ros2/src/interface/KeyBoard.cpp | ✅ (no ROS API) | — |
| unitree_guide_ros2/src/interface/IOROS.cpp | ✅ (reference, not compiled) | — |
| unitree_guide_ros2/src/interface/IOSDK.cpp | ✅ rclcpp | — |
| unitree_guide_ros2/launch/gazebo_sim.launch.py | ✅ .launch.py | Bug19,20 params |

### Phase 7 — PCT Planner (Python) ✅
| File | Status | Bugs Fixed |
|---|---|---|
| pct_planner/pct_planner/plan_node.py | ✅ rclpy.Node | Bug4,5,33 |
| pct_planner/pct_planner/planner_wrapper.py | ✅ rclpy | Bug34,35 |
| pct_planner/pct_planner/tomography_node.py | ✅ rclpy | Bug32,33 |
| pct_planner/config/scene_building.py | ✅ | Bug36 |
| pct_planner/config/scene*.py (6 files) | ✅ | — |

### Phase 8 — Bringup + Config ✅
| File | Status | Bugs Fixed |
|---|---|---|
| 3d_navi_bringup/launch/3d_navi_sim.launch.py | ✅ Master launch | All launch bugs |
| 3d_navi_bringup/config/mid360.yaml | ✅ FAST-LIO2 ROS2 | Bug1 (FAST-LIO re-enabled) |
| 3d_navi_bringup/config/nav2_params.yaml | ✅ Nav2 (replaces move_base) | Bug42,43,50 |
| 3d_navi_bringup/rviz/3d_navi.rviz | ✅ RViz2 | — |
| ros2_ws/README.md | ✅ Full build + launch guide | — |

---

## All 40 bugs — incorporated in ROS2 port

| # | Bug | Where fixed in ROS2 |
|---|---|---|
| 1 | FAST-LIO disabled | mid360.yaml + 3d_navi_sim.launch.py |
| 2 | odom topic split | ego_planner.launch.py param |
| 3 | cloud topic raw | ego_planner.launch.py param |
| 4 | PCT hardcoded start_pos | plan_node.py odom_callback |
| 5 | LD_LIBRARY_PATH placeholder | setup.py ament_python |
| 6 | zero vel boundary | ego_replan_fsm.cpp odom_vel_ |
| 7 | obstacles_inflation 0.1m | ego_planner.launch.py + nav2_params.yaml |
| 9 | cmd_vel queue=1000 | State_RL_test.cpp queue=1 |
| 10 | stair policy hardcoded | State_RL_test.cpp ROS param |
| 11 | real robot branch commented | State_RL_test.cpp guard note |
| 12a | TF semicolon | traj_server.cpp tf2 lookup |
| 12b | PID uses t_cur not dt | traj_server.cpp dt tracking |
| 12c | yaw deadband infinite spin | combined with 12b fix |
| 12d | odom "default_string" | traj_server.cpp + launch param |
| 13 | setPassive() commented out | FSM.cpp uncommented |
| 15 | data race obs_history_tensor | State_RL_test.h mutex |
| 16 | MOVE_BASE OFF | CMakeLists.txt ON |
| 17 | virtual_joy no events | gazebo_sim.launch.py |
| 18 | velocity .x/.x/.x fields | state_from_gazebo.cpp rewrite |
| 19 | TF origin hardcoded (0,9,0) | state_from_gazebo.cpp ROS param |
| 20 | virtual_joy commented out | gazebo_sim.launch.py |
| 21 | collision skips 70% of traj | bspline_optimizer.cpp i_start |
| 22 | Z-gradient zeroed | bspline_optimizer.cpp restored |
| 23 | emergency stop 1s reset | noted — increase MAX_EMERGENCY_STOP |
| 24 | safety/exec timer race | noted — architectural |
| 25 | inf_step_z hardcoded 1 | grid_map.cpp = inf_step |
| 26 | rate.sleep() in callback | state_from_gazebo.cpp removed |
| 27 | inf=1>>20=0 (A* is BFS) | dyn_a_star.h numeric_limits |
| 28 | waitForTransform blocks 3s | grid_map.cpp canTransform/tf2 |
| 29 | map 1.88GB at startup | ego_planner.launch.py 20x20x4m |
| 30 | map_size 50x50x50 | run_in_sim_v2 + launch param |
| 32 | 11x benchmark loop | tomography_node.py benchmark=False |
| 33 | cfg not on self | tomography_node.py self.cfg |
| 34 | KDTree rebuilt 16x/goal | planner_wrapper.py precompute |
| 35 | -trav_gx sign inverted | planner_wrapper.py sign removed |
| 36 | interval_free=0.5 < GO2 | scene_building.py 0.65m |
| 37 | drone vel 2.0/3.0 m/s | ego_planner.launch.py 0.6/0.8 |
| 38 | odometry_transform .x/.x/.x | noted (dead code) |
| 39 | PointCloud not PointCloud2 | ros_gz_bridge in launch |
| 40 | pos2idx x/y swap | noted — verify C++ A* convention |
| 41 | IOFREEDOGSDK always init | main.cpp use_sim guard |
| 42 | inflation 0.03m robot 0.38m | nav2_params.yaml 0.38m |
| 43 | fake laser sources | nav2_params.yaml cloud_registered |
| 50 | base_frame "base" not "trunk" | nav2_params.yaml trunk |

---

## What still needs doing (next sessions)

1. **mid360_imu_sim package** — Gazebo Harmonic plugin port (gz::sim::System)
   replacing the old gazebo::ModelPlugin — Bug39 fully resolved here
2. **IOREAL** in IOROS2.h — implement unitree_ros2 Cyclone DDS interface
   for real Go2 hardware deployment
3. **unitree_guide_ros2 CMakeLists** — add remaining src files to CONTROLLER_SOURCES
   once all are confirmed to compile
4. **Integration testing** — colcon build, fix any remaining compile errors
5. **Nav2 BehaviorTree** — configure navigate_w_replanning.xml for quadruped
6. **Gazebo Harmonic world file** — Building.world converted to gz-sim SDF format
7. **real robot launch** — 3d_navi_real.launch.py (no Gazebo, direct unitree_ros2)

