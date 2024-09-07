// Copyright 2024, Edward Morgan
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef ROS2_CONTROL_BLUE_REACH_5__SIM_VEHICLE_SYSTEM_MULTI_INTERFACE_HPP_
#define ROS2_CONTROL_BLUE_REACH_5__SIM_VEHICLE_SYSTEM_MULTI_INTERFACE_HPP_

#include <chrono>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "hardware_interface/handle.hpp"
#include "hardware_interface/hardware_info.hpp"
#include "hardware_interface/system_interface.hpp"
#include "hardware_interface/types/hardware_interface_return_values.hpp"
#include "rclcpp/rclcpp.hpp"
#include "rclcpp/clock.hpp"
#include "rclcpp/duration.hpp"
#include "rclcpp/macros.hpp"
#include "rclcpp/time.hpp"
#include "rclcpp_lifecycle/node_interfaces/lifecycle_node_interface.hpp"
#include "rclcpp_lifecycle/state.hpp"
#include "ros2_control_blue_reach_5/visibility_control.h"

#include "ros2_control_blue_reach_5/state.hpp"
#include "ros2_control_blue_reach_5/custom_hardware_interface_type_values.hpp"
#include "ros2_control_blue_reach_5/dynamics.hpp"

#include "realtime_tools/realtime_buffer.h"
#include "realtime_tools/realtime_publisher.h"
#include "std_msgs/msg/float64_multi_array.hpp"
#include "tf2_msgs/msg/tf_message.hpp"

#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>
#include <sensor_msgs/msg/imu.hpp>
#include <geometry_msgs/msg/twist.hpp>


#include <casadi/casadi.hpp>

namespace ros2_control_blue_reach_5
{

    class SimVehicleSystemMultiInterfaceHardware : public hardware_interface::SystemInterface
    {

    public:
        RCLCPP_SHARED_PTR_DEFINITIONS(SimVehicleSystemMultiInterfaceHardware);

        ROS2_CONTROL_BLUE_REACH_5_PUBLIC
        hardware_interface::CallbackReturn on_init(
            const hardware_interface::HardwareInfo &info) override;

        ROS2_CONTROL_BLUE_REACH_5_PUBLIC
        hardware_interface::CallbackReturn on_configure(
            const rclcpp_lifecycle::State &previous_state) override;

        // ROS2_CONTROL_BLUE_REACH_5_PUBLIC
        // hardware_interface::CallbackReturn on_cleanup(
        //     const rclcpp_lifecycle::State &previous_state) override;

        ROS2_CONTROL_BLUE_REACH_5_PUBLIC
        hardware_interface::return_type prepare_command_mode_switch(
            const std::vector<std::string> &start_interfaces,
            const std::vector<std::string> &stop_interfaces) override;

        // ROS2_CONTROL_BLUE_REACH_5_PUBLIC
        // hardware_interface::return_type perform_command_mode_switch(
        //     const std::vector<std::string> &start_interfaces,
        //     const std::vector<std::string> &stop_interfaces) override;

        ROS2_CONTROL_BLUE_REACH_5_PUBLIC
        std::vector<hardware_interface::StateInterface> export_state_interfaces() override;

        ROS2_CONTROL_BLUE_REACH_5_PUBLIC
        std::vector<hardware_interface::CommandInterface> export_command_interfaces() override;

        ROS2_CONTROL_BLUE_REACH_5_PUBLIC
        hardware_interface::CallbackReturn on_activate(
            const rclcpp_lifecycle::State &previous_state) override;

        ROS2_CONTROL_BLUE_REACH_5_PUBLIC
        hardware_interface::CallbackReturn on_deactivate(
            const rclcpp_lifecycle::State &previous_state) override;

        ROS2_CONTROL_BLUE_REACH_5_PUBLIC
        hardware_interface::return_type read(
            const rclcpp::Time &time, const rclcpp::Duration &period) override;

        ROS2_CONTROL_BLUE_REACH_5_PUBLIC
        hardware_interface::return_type write(
            const rclcpp::Time &time, const rclcpp::Duration &period) override;

    private:
        // Enum defining at which control level we are
        // maintaining the command_interface type per thruster.
        enum mode_level_t : std::uint8_t
        {
            MODE_STANDBY = 0x00,
            MODE_DISABLE = 0x01,
            MODE_POSITION = 0x02,
            MODE_VELOCITY = 0x03,
            MODE_CURRENT = 0x04,
            MODE_EFFORT_GENERALIZED = 0x05,
            MODE_EFFORT = 0x09,
        };

        std::vector<mode_level_t> control_level_;

        // Store the dynamics function for the robot joints
        casadi_reach_alpha_5::Dynamics dynamics_service;

        // Store the state & commands for the robot vehicle
        blue::dynamics::Vehicle hw_vehicle_struct_;

        // stores the dynamic response from the forward dynamics simulator
        std::vector<double> forward_dynamics_res;

        std::shared_ptr<rclcpp::Publisher<tf2_msgs::msg::TFMessage>> odometry_transform_publisher_;
        std::shared_ptr<realtime_tools::RealtimePublisher<tf2_msgs::msg::TFMessage>>
            realtime_odometry_transform_publisher_;

        std::shared_ptr<rclcpp::Publisher<sensor_msgs::msg::Imu>> uv_imu_publisher_;
        std::shared_ptr<realtime_tools::RealtimePublisher<sensor_msgs::msg::Imu>>
            realtime_uv_imu_publisher_;

        std::shared_ptr<rclcpp::Publisher<geometry_msgs::msg::Twist>> uv_dvl_publisher_;
        std::shared_ptr<realtime_tools::RealtimePublisher<geometry_msgs::msg::Twist>>
            realtime_uv_dvl_publisher_;
    };

} // namespace ros2_control_blue
#endif // ROS2_CONTROL_BLUE_REACH_5__SIM_VEHICLE_SYSTEM_MULTI_INTERFACE_HPP_
