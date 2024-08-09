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

// #pragma once
#ifndef ROS2_CONTROL_BLUE_REACH_5__VEHICLE_HPP_
#define ROS2_CONTROL_BLUE_REACH_5__VEHICLE_HPP_

#include <string>
#include <algorithm>
#include <cmath>
#include <vector>
#include <Eigen/Core>
#include <Eigen/Dense>
#include "ros2_control_blue_reach_5/thruster.hpp"
#include "ros2_control_blue_reach_5/eigen.hpp"

namespace blue::dynamics
{
  class Vehicle
  {

  public:
    std::string name;  // Name of the device or component
    uint8_t device_id; // Unique identifier for the device
    std::string world_frame; // origin frame
    std::string body_frame; // body frame


    struct Pose_vel {
      double position_x, position_y, position_z;
      double orientation_w, orientation_x, orientation_y, orientation_z;
      double u, v, w, p, q, r;
      double Fx, Fy, Fz, Tx, Ty, Tz;
    };

    Pose_vel default_state_, command_state_, current_state_, async_state_;
    std::vector<Thruster> hw_thrust_structs_;  // Store the state & commands for the uv thruster
    
    Vehicle() = default;
    Vehicle(std::string vehicle_name, Pose_vel default_state)
        : name(std::move(vehicle_name)), current_state_(default_state) {};

    void set_vehicle_name(const std::string &vehicle_name, const Pose_vel &default_state);

    void thrustSizeAllocation(const double &joint_size); // Attempt to preallocate enough memory for specified number of elements
  };
} // namespace blue::dynamics
#endif // ROS2_CONTROL_BLUE_REACH_5__JOINT_HPP_
