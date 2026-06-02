#ifndef TOFSENSEMINIT_H
#define TOFSENSEMINIT_H

#include "utils/protocol_extracter/nprotocol_extracter.h"
#include <nlink_parser_interfaces/msg/tofsense_m_frame0.hpp>
#include <rclcpp/rclcpp.hpp>
#include <unordered_map>

namespace tofsensem
{
  class Init
  {
  public:
    explicit Init(NProtocolExtracter *protocol_extraction, rclcpp::Node *nd);

  private:
    void InitFrame0(NProtocolExtracter *protocol_extraction);
    rclcpp::Publisher<nlink_parser_interfaces::msg::TofsenseMFrame0>::SharedPtr  publisher_TofsenseMFrame0_;
    rclcpp::Node *node;
  };

} // namespace tofsensem
#endif // TOFSENSEMINIT_H
