#include <iostream>
#include "motor_control/link.hpp"
#include "motor_control/reporter.hpp"

namespace mc = bw::motor_control;

int main()
{
    auto link = bw::motor_control::create_link();
    if (!link->bind()) {
        std::cerr << "Failed to bind motor link udp socket.\n"
                  << "Multiple instances running?"
                  << std::endl;
        return 1;
    }

    auto reporter = bw::motor_control::create_reporter();
    if (!reporter->connect()) {
        std::cerr << "Failed to connect reporter to UI" << std::endl;
        return 1;
    }

    while (true) {
        mc::packet packet;
        mc::read_result result = link->read(packet);
        if (result == mc::read_result::ok) {
            // Do something nice with the packet

            // Print packet
            for (auto b : packet) {
                std::cout << std::hex << static_cast<unsigned>(b);
            }
            std::cout << std::endl;

            if (!reporter->send(packet)) {
                std::cerr << "Failed to send" << std::endl;
            }
        }
        else if (result == mc::read_result::error) {
            return 1;
        }
    }
}
