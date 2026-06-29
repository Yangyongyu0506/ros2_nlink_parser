#include <chrono>
#include <functional>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"

#include "init.h"
#include "utils/init_serial.h"
#include "utils/protocol_extracter/nprotocol_extracter.h"

#include <iomanip>
#include <iostream>

#include <thread>

using namespace std::chrono_literals;

class TOFSenseM : public rclcpp::Node
{
public:
    TOFSenseM() : Node("tofsensem"), count_(0)
    {
        this->declare_parameter<std::string>("port_name", "/dev/ttyUSB0");
        this->declare_parameter<int>("baud_rate", 921600);

        auto port_name = this->get_parameter("port_name").as_string();
        auto baud_rate = static_cast<uint32_t>(
            this->get_parameter("baud_rate").as_int());

        serial::Serial serial;
        NProtocolExtracter extracter;

        initSerial(&serial, port_name, baud_rate);
        tofsensem::Init init(&extracter, this);

        while (rclcpp::ok())
        {
            auto available_bytes = serial.available();
            std::string str_received;
            if (available_bytes)
            {
                serial.read(str_received, available_bytes);
                extracter.AddNewData(str_received);
            }
            else
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
    }

private:
    size_t count_;
};
