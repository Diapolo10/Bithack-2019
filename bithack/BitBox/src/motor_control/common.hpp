#ifndef MOTOR_CONTROL_COMMON_HPP
#define MOTOR_CONTROL_COMMON_HPP
#include <cstdint>
#include <array>

namespace bw {
namespace motor_control {

static constexpr size_t packet_size = 9;
using packet = std::array<uint8_t, packet_size>;

}  // namespace motor_control
}  // bw
#endif  // MOTOR_CONTROL_COMMON_HPP
