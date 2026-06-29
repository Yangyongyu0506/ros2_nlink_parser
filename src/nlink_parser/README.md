# nlink_parser (ROS2)

解析 Nooploop NLink 系列设备数据的 ROS2 功能包。

## 依赖

- ROS2 Humble 或更高版本
- `serial`（本仓库自带 `src/serial/`）
- `nlink_parser_interfaces`（本仓库自带 `src/nlink_parser_interfaces/`）

## 编译

本包是 `ros2_nlink_parser` 工作空间的一部分，从工作空间根目录编译：

```bash
source /opt/ros/humble/setup.bash
cd ~/dev_ws
colcon build
source install/setup.bash
```

或仅编译本包：

```bash
colcon build --packages-select nlink_parser
```

修改 `.msg` 文件后，需先编译消息包：

```bash
colcon build --packages-select nlink_parser_interfaces
colcon build --packages-select nlink_parser
```

## 可执行文件

| 可执行文件 | 设备类型 |
|-----------|---------|
| `linktrack` | LinkTrack |
| `linktrack_aoa` | LinkTrack AoA |
| `iot` | IOT |
| `tofsense` | TOFSense |
| `tofsensem` | TOFSense-M |

## 运行

```bash
# 所有节点通过参数指定串口和波特率
ros2 run nlink_parser linktrack --ros-args -p port_name:="/dev/ttyUSB0" -p baud_rate:=921600
ros2 run nlink_parser linktrack_aoa --ros-args -p port_name:="/dev/ttyUSB0"
ros2 run nlink_parser iot --ros-args -p port_name:="/dev/ttyUSB0"
ros2 run nlink_parser tofsense --ros-args -p port_name:="/dev/ttyUSB0"
ros2 run nlink_parser tofsensem --ros-args -p port_name:="/dev/ttyUSB0"
```

## 架构

- **nutils**: 静态库，包含协议提取器 (`NProtocolExtracter`)、NLink 解包（C99）、串口初始化等共用模块
- **每个设备**: 独立的 `Init` 类注册对应帧协议的回调，在回调中将 C 结构体转换为 ROS2 消息并发布
- **主循环**: 在节点构造函数中以阻塞方式持续读取串口数据
