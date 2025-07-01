# ros2_nlink_parser

`ros2_nlink_parser` 是一个基于 ROS 2 的数据解析器，用于处理和发布来自不同传感器的数据帧。

## 功能特性

- 支持多种传感器数据解析。
- 基于 ROS 2 的消息通信机制。
- 提供高效的协议解析工具。

## 依赖项

在使用本项目之前，请确保已安装以下依赖项：

- ROS 2（支持的发行版如 Humble、Foxy 等）
- `serial` 库（用于串口通信）
- `rclcpp`（ROS 2 C++ 客户端库）
- `nlink_parser_interfaces`（自定义消息接口）

## 安装

1. 克隆项目到你的工作区：
   ```bash
   git clone https://github.com/your-repo/ros2_nlink_parser.git
   cd ros2_nlink_parser
   ```

2. 安装依赖项：
   ```bash
   sudo apt-get install ros-<ros2_distro>-serial
   ```

3. 构建项目：
   ```bash
   colcon build
   ```

4. 设置环境变量：
   ```bash
   source install/setup.bash
   ```

## 使用方法

### 启动节点

运行以下命令启动解析节点：
```bash
ros2 run nlink_parser <node_name>
```

### 配置参数

可以通过修改 `params.yaml` 文件配置节点参数。

### 发布与订阅

- 发布的主题：
  - `/iot_frame0`：发布解析后的 IoT 数据帧。
- 订阅的主题：
  - `/sensor_data`：接收原始传感器数据。

## 文件结构

```
ros2_nlink_parser/
├── src/
│   ├── nlink_parser/          # 核心解析代码
│   ├── nlink_parser_interfaces/ # 自定义消息接口
│   └── README.md              # 项目说明文件
├── install/                   # 安装目录
├── build/                     # 构建目录
└── CMakeLists.txt             # CMake 配置文件
```

## 常见问题

### 1. 找不到 `serial` 库
确保已安装 `serial` 库，并将其路径添加到 `CMAKE_PREFIX_PATH`。

### 2. 无法找到 `nlink_parser_interfaces` 消息
请确认已正确构建 `nlink_parser_interfaces`，并运行以下命令：
```bash
source install/setup.bash
```

## 贡献

欢迎提交 Issue 或 Pull Request 来改进本项目。

## 许可证

本项目基于 [MIT License](LICENSE) 许可。
