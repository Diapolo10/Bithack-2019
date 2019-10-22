#include "reporter.hpp"

#include <cassert>
#include <iostream>
#include <asio/ts/io_context.hpp>
#include <asio/ts/net.hpp>
#include <asio/ts/internet.hpp>

using asio::ip::udp;

namespace bw {
namespace motor_control {

class reporter : public reporter_interface
{
public:
    reporter(const std::string& address = "127.0.0.1", uint16_t port = 8002);
    ~reporter() override;

    bool connect() override;
    bool send(const packet& packet) override;

private:
    asio::io_context context_;
    udp::socket socket_;
    udp::endpoint send_endpoint_;
};

reporter::reporter(const std::string& address, uint16_t port) :
    socket_(context_),
    send_endpoint_(asio::ip::make_address_v4(address), port)
{
}

reporter::~reporter()
{

}

bool reporter::connect()
{
    socket_.open(udp::v4());
    return true;
}

bool reporter::send(const packet& packet)
{
    //Implement this
    return false;
}

std::unique_ptr<reporter_interface> create_reporter(const std::string& ip_addr, uint16_t port)
{
    return std::unique_ptr<reporter_interface>(new reporter(ip_addr, port));
}

reporter_interface::~reporter_interface() {}



}  // namespace motor_control
}  // namespace bw

