#ifndef MOTOR_CONTROL_LINK_HPP
#define MOTOR_CONTROL_LINK_HPP
#include <string>
#include <array>
#include <chrono>
#include <memory>

#include "common.hpp"

namespace bw {
namespace motor_control {

using timeout_clock = std::chrono::high_resolution_clock;

enum class read_result {
    ok,
    no_data,
    error
};

class link_interface
{
public:

    virtual ~link_interface();

    virtual bool bind() = 0;
    virtual read_result read(packet& packet_out) = 0;
    virtual read_result read_blocking(packet& packet_out, timeout_clock::time_point timeout) = 0;
};

std::unique_ptr<link_interface> create_link(uint16_t port = 8001);

}  // namespace motor_control
}  // namespace bw

#endif // MOTOR_CONTROL_LINK_HPP
