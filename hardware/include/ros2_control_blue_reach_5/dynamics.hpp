#ifndef ROS2_CONTROL_BLUE_REACH_5__DYNAMICS_HPP_
#define ROS2_CONTROL_BLUE_REACH_5__DYNAMICS_HPP_

#include <string>
#include <algorithm>
#include <casadi/casadi.hpp>
#include <Eigen/Geometry>  // Include Eigen library for geometry

using namespace casadi;

namespace casadi_reach_alpha_5
{
    class Dynamics
    {

    public:
        uint8_t dynamics_id;       // Unique identifier for the dynamics
        Function forward_dynamics; // forward dynamics of the robotic arm
        Function forward_kinematics; // forward kinematics of the robotic arm
        Function inverse_dynamics; // inverse dynamics of the robotic arm
        Function inertia_matrix; // inertia/mass matrix of the robotic arm
        Function kalman_filter; // kalman filter for robotic arm sensor
        Function vehicle_dynamics; // forward dynamics of the vehicle
        Function torque2currentMap; // forward motor dynamics
        Function current2torqueMap; // inverse motor dynamics
        Function uvms_dynamics;
        Function pd_controller;

        Dynamics() = default;
        // Constructor with member initializer list
        Dynamics(uint8_t dyn_id)
            : dynamics_id(dyn_id) {}

        void usage_cplusplus_checks(const std::string &name, const std::string &bin_name, const std::string &node_name);
        /**
         * @brief checks casadi function loader works well
         *
         * @param name Name as in the label assigned to a CasADi Function object
         * @param bin_name File name of the shared library
         */
        Function load_casadi_fun(const std::string &name, const std::string &bin_name);
        /**
         * @brief checks casadi function loader works well
         *
         * @param name Name as in the label assigned to a CasADi Function object
         * @param bin_name File name of the shared library
         */
    };
} // namespace casadi_reach_alpha_5
#endif // ROS2_CONTROL_BLUE_REACH_5__JOINT_HPP_