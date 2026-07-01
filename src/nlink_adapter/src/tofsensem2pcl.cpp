#include "rclcpp/rclcpp.hpp"
#include "nlink_parser_interfaces/msg/tofsense_m_frame0.hpp"
#include "sensor_msgs/msg/point_cloud.hpp"
#include "geometry_msgs/msg/point32.hpp"
#include <cmath>
#include <memory>
#include <string>
#include <vector>

class TofSenseM2PCL : public rclcpp::Node {
  private:
    float fov_h_rad;
    float fov_v_rad;
    std::string tof_frame_id;
    std::string tof_topic_name;
    std::string pcl_topic_name;
    int pxl_points_h;
    int pxl_points_v;
    std::vector<float> angles_h;
    std::vector<float> angles_v;

    rclcpp::Subscription<nlink_parser_interfaces::msg::TofsenseMFrame0>::SharedPtr tof_sub;
    rclcpp::Publisher<sensor_msgs::msg::PointCloud>::SharedPtr pcl_pub;
  public:
    TofSenseM2PCL(): Node("tof_sense_m2pcl") {
      this->declare_parameter("fov_h_rad", 45 * M_PI / 180.0);
      this->declare_parameter("fov_v_rad", 45 * M_PI / 180.0);
      this->declare_parameter("tof_frame_id", std::string("tof_frame"));
      this->declare_parameter("tof_topic_name", std::string("nlink_tofsensem_frame0"));
      this->declare_parameter("pcl_topic_name", std::string("point_cloud"));
      this->declare_parameter("pxl_points_h", 8);
      this->declare_parameter("pxl_points_v", 8);

      fov_h_rad = static_cast<float>(this->get_parameter("fov_h_rad").as_double());
      fov_v_rad = static_cast<float>(this->get_parameter("fov_v_rad").as_double());
      tof_frame_id = this->get_parameter("tof_frame_id").as_string();
      tof_topic_name = this->get_parameter("tof_topic_name").as_string();
      pcl_topic_name = this->get_parameter("pcl_topic_name").as_string();
      pxl_points_h = this->get_parameter("pxl_points_h").as_int();
      pxl_points_v = this->get_parameter("pxl_points_v").as_int();

      for (int i = 0; i < pxl_points_h; i++) {
        angles_h.push_back(-fov_h_rad / 2.0 + fov_h_rad * i / (pxl_points_h - 1));
      }
      for (int i = 0; i < pxl_points_v; i++) {
        angles_v.push_back(-fov_v_rad / 2.0 + fov_v_rad * i / (pxl_points_v - 1));
      }

      tof_sub = this->create_subscription<nlink_parser_interfaces::msg::TofsenseMFrame0>(
        tof_topic_name, 10, std::bind(&TofSenseM2PCL::tof_callback, this, std::placeholders::_1));
      pcl_pub = this->create_publisher<sensor_msgs::msg::PointCloud>(pcl_topic_name, 10);
      RCLCPP_INFO(this->get_logger(), "TofSenseM2PCL node initialized.");
    }

    void tof_callback(const nlink_parser_interfaces::msg::TofsenseMFrame0::SharedPtr msg) {
      RCLCPP_INFO(this->get_logger(), "Received TofSenseMFrame0 message.");
      auto pcl_msg = std::make_shared<sensor_msgs::msg::PointCloud>();
      pcl_msg->header.stamp = this->now();
      pcl_msg->header.frame_id = tof_frame_id;

      for (int i = 0; i < pxl_points_v; i++) {
        for (int j = 0; j < pxl_points_h; j++) {
          float distance = 1e-3 * msg->pixels[i * pxl_points_h + j].dis;
          if (msg->pixels[i * pxl_points_h + j].dis_status == 0) {
            geometry_msgs::msg::Point32 point;
            point.x = distance;
            point.y = distance * std::tan(angles_h[j]);
            point.z = distance * std::tan(angles_v[i]);
            pcl_msg->points.push_back(point);
          }
        }
      }
      pcl_pub->publish(*pcl_msg);
    }
};


int main(int argc, char ** argv) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<TofSenseM2PCL>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
