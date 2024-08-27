# Copyright 2021 Stogl Robotics Consulting UG (haftungsbeschränkt)
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, RegisterEventHandler, ExecuteProcess, SetLaunchConfiguration
from launch.conditions import IfCondition, UnlessCondition
from launch.event_handlers import OnProcessExit
from launch.substitutions import Command, FindExecutable, LaunchConfiguration, PathJoinSubstitution, TextSubstitution

from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare


def generate_launch_description():
    # Declare arguments
    declared_arguments = []
    declared_arguments.append(
        DeclareLaunchArgument(
            "prefix",
            default_value='alpha',
            description="Prefix of the joint names, useful for \
        multi-robot setup. If changed than also joint names in the controllers' configuration \
        have to be updated.",
        )
    )
    declared_arguments.append(
        DeclareLaunchArgument(
            "serial_port",
            default_value="/dev/ttyUSB0",
            description="Start robot with device port to hardware.",
        )
    )
    declared_arguments.append(
        DeclareLaunchArgument(
            "state_update_frequency",
            default_value="200",
            description="The frequency (Hz) at which the driver updates the state of the robot."
            " Note that this should not be less than the read frequency defined by"
            " the controller configuration.",
        )
    )
    declared_arguments.append(
        DeclareLaunchArgument(
            "use_mock_hardware",
            default_value="true",
            description="Start robot with mock hardware mirroring command to its states.",
        )
    )
    declared_arguments.append(
        DeclareLaunchArgument(
            "endeffector_control",
            default_value="false",
            description="use cartesian control",
        )
    )
    declared_arguments.append(
        DeclareLaunchArgument(
            "robot_controller",
            default_value="forward_current_controller",
            description="Robot controller to start.",
        )
    )
    declared_arguments.append(
        DeclareLaunchArgument(
            "gui",
            default_value="true",
            description="Start RViz2 automatically with this launch file.",
        )
    )

    # Initialize Arguments
    prefix = LaunchConfiguration("prefix")
    use_mock_hardware = LaunchConfiguration("use_mock_hardware")
    serial_port = LaunchConfiguration("serial_port")
    state_update_frequency = LaunchConfiguration("state_update_frequency")
    robot_controller = LaunchConfiguration("robot_controller")
    gui = LaunchConfiguration("gui")


    # Define the robot description command with a conditional substitution
    robot_description_content = Command(
        [
            PathJoinSubstitution([FindExecutable(name="xacro")]),
            " ",
            PathJoinSubstitution(
                [FindPackageShare("ros2_control_blue_reach_5"),
                 "xacro", "robot_system_multi_interface.urdf.xacro"]
            ),
            " ",
            "prefix:=", prefix, " ",
            "serial_port:=", serial_port, " ",
            "state_update_frequency:=", state_update_frequency, " ",
            "use_mock_hardware:=", use_mock_hardware, " "
        ]
    )

    robot_controllers = PathJoinSubstitution(
        [
            FindPackageShare("ros2_control_blue_reach_5"),
            "config",
            "robot_multi_interface_forward_controllers.yaml",
        ]
    )
    rviz_config_file = PathJoinSubstitution(
        [FindPackageShare("ros2_control_blue_reach_5"), "rviz", "rviz.rviz"]
    )

    robot_description = {"robot_description": robot_description_content}
    robot_state_pub_node = Node(
        package="robot_state_publisher",
        executable="robot_state_publisher",
        output="both",
        parameters=[robot_description],
    )
    rviz_node = Node(
        package="rviz2",
        executable="rviz2",
        name="rviz2",
        output="log",
        arguments=["-d", rviz_config_file],
        condition=IfCondition(gui),
    )

    control_node = Node(
        package="controller_manager",
        executable="ros2_control_node",
        parameters=[robot_controllers, robot_description],
        # remappings=[("~/robot_description", "/robot_description")],
        output="both",
    )

    joint_state_broadcaster_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=["joint_state_broadcaster",
                   "--controller-manager", "/controller_manager"]
    )

    uvms_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=["uvms_controller",
                   "--controller-manager", "/controller_manager"]
    )

    run_plotjuggler = ExecuteProcess(
        cmd=['ros2', 'run', 'plotjuggler', 'plotjuggler > /dev/null 2>&1'],
        output='screen',
        shell=True
    )

    ema_filter_entity = Node(
        package='namor',
        executable='ema_node'
    )

    namor_entity = Node(
        package='namor',
        executable='namor_node'
    )

    mouse_control_current = Node(
        package='namor',
        executable='mouse_node_current',
    )

    mouse_control_velocity = Node(
        package='namor',
        executable='mouse_node_velocity'
    )

    # Delay rviz start after `joint_state_broadcaster`
    delay_rviz_after_joint_state_broadcaster_spawner = RegisterEventHandler(
        event_handler=OnProcessExit(
            target_action=joint_state_broadcaster_spawner,
            on_exit=[rviz_node],
        )
    )


    nodes = [
        run_plotjuggler,
        mouse_control_current,
        # mouse_control_velocity,
        # ema_filter_entity,
        # namor_entity,
        control_node,
        robot_state_pub_node,
        joint_state_broadcaster_spawner,
        uvms_spawner,
        delay_rviz_after_joint_state_broadcaster_spawner
    ]

    return LaunchDescription(declared_arguments + nodes)
