#include "init_serial.h"

#include <iostream>
#include <string>

void initSerial(serial::Serial *serial, const std::string &port_name,
                uint32_t baud_rate)
{
  try
  {
    serial->setPort(port_name);
    serial->setBaudrate(baud_rate);

    std::cout << "try to open serial port with " << port_name << " , "
              << baud_rate << "\n";
    auto timeout = serial::Timeout::simpleTimeout(10);
    serial->setTimeout(timeout);
    serial->open();

    if (serial->isOpen())
    {
      std::cout << "Serial port opened successfully, waiting for data." << "\n";
    }
    else
    {
      std::cout << "Failed to open serial port, please check and retry." << "\n";
      exit(EXIT_FAILURE);
    }
  }
  catch (const std::exception &e)
  {
    std::cout << "Unhandled Exception: " << e.what() << "\n";
    exit(EXIT_FAILURE);
  }
}
