#include "link.hpp"
#include <cassert>
#include <iostream>
#include <asio/ts/io_context.hpp>
#include <asio/ts/net.hpp>
#include <asio/ts/internet.hpp>

using asio::ip::udp;

namespace bw {
namespace motor_control {

class link : public link_interface
{
public:
    explicit link(uint16_t port = 8001);
    ~link() override;

    bool bind() override;
    read_result read(packet& packet_out) override;
    read_result read_blocking(packet& packet_out, timeout_clock::time_point timeout) override;

private:
    asio::io_context context_;
    udp::socket socket_;
    uint16_t port_;
};

link::link(uint16_t port) :
    socket_(context_),
    port_(port)
{
}

link::~link()
{
}

bool link::bind()
{
    asio::error_code ec;
    socket_.open(udp::v4());
    socket_.non_blocking(true);
    socket_.bind(udp::endpoint(udp::v4(), port_), ec);

    if (ec) {
        std::cerr << "Binding motor_control::link failed: " << ec.message() << std::endl;
        return false;
    }
    return true;
}

read_result link::read(packet& packet_out)
{
    asio::error_code ec;
    packet buf;
    udp::endpoint sender;
    size_t received_bytes = socket_.receive_from(asio::buffer(buf), sender, 0, ec);

    if (ec.value() == asio::error::would_block)
    {
        return read_result::no_data;
    }
    if (ec) {
        std::cerr << "link::read: Error during receive_from:" << ec.message() << std::endl;
        return read_result::error;
    }
    else if (received_bytes != packet_size) {
        std::cerr << "link::read: Received unexpected packet size: "
                  << received_bytes << std::endl;
        return read_result::error;
    }

    packet_out = buf;
    return read_result::ok;
}

read_result link::read_blocking(bw::motor_control::packet& packet_out, timeout_clock::time_point timeout)
{
    read_result r = read_result::no_data;
    udp::endpoint sender;
    socket_.async_receive_from(
                asio::buffer(packet_out),
                sender,
                [&r](const asio::error_code& error, std::size_t bytes_transferred) {
        if (error) {
            std::cerr << "link::read_blocking: Error during async_receive_from:" << error.message() << std::endl;
            r = read_result::error;
        }
        else if (bytes_transferred != packet_size){
            std::cerr << "link::read_blocking: Received unexpected packet size: "
                      << bytes_transferred << std::endl;
            r = read_result::error;
        }
        else {
            r = read_result::ok;
        }
    });

    size_t executed_handlers = context_.run_one_until(timeout);
    context_.restart();
    if (executed_handlers == 0) {
        // Timeout occured before async_receive_from completed so we have to cancel it
        socket_.cancel();
    }

    return r;
}

link_interface::~link_interface() {}

std::unique_ptr<link_interface> create_link(uint16_t port)
{
    return std::unique_ptr<link_interface>(new link(port));
}



}  // namespace motor_control
}  // namespace bw

