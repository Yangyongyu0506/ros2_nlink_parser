#ifndef INITSERIAL_H
#define INITSERIAL_H
#include <serial/serial.h>

#include <string>

void initSerial(serial::Serial *serial, const std::string &port_name,
                uint32_t baud_rate);

#endif // INITSERIAL_H
