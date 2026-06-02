#include "nutils.h"
#include <iostream>

void TopicAdvertisedTip(const char *topic)
{
  std::cout << topic << " has been advertised, use "
            << "ros2 topic echo /" << topic << " to view the data" << "\n";
}
