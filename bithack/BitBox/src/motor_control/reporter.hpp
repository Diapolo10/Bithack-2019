#ifndef BW_MOTOR_CONTROL_REPORTER_HPP
#define BW_MOTOR_CONTROL_REPORTER_HPP
#include <string>
#include <array>
#include <memory>

#include "common.hpp"

namespace bw {
namespace motor_control {

class reporter_interface
{
public:
    virtual ~reporter_interface();

    virtual bool connect() = 0;
    virtual bool send(const packet& packet) = 0;
};

std::unique_ptr<reporter_interface> create_reporter(
        const std::string& ip_addr = "127.0.0.1",
        uint16_t port = 8002);

}  // namespace motor_control
}  // namespace bw

#endif  // BW_MOTOR_CONTROL_REPORTER_HPP
