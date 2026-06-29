# ros2_nlink_parser

[nlink_parser](https://github.com/nooploop-dev/nlink_parser) 的 ROS2 迁移版本，基于 [csw609](https://github.com/csw609/nlink_parser) 的项目修改而来。用于解析深圳一诺精准（Nooploop）NLink 系列定位产品的串口数据，并发布为 ROS2 话题。

## 支持的设备

| 设备 | 可执行文件 | ROS2 节点名 |
|------|-----------|-------------|
| LinkTrack | `linktrack` | `linktrack` |
| LinkTrack AoA | `linktrack_aoa` | `linktrack_aoa` |
| IOT | `iot` | `iot` |
| TOFSense | `tofsense` | `tofsense` |
| TOFSense-M | `tofsensem` | `tofsensem` |

## 依赖

- ROS2 Humble 或更高版本
- `pyserial`（仅虚拟验证需要）

## 编译

```bash
# 将本仓库放置在 ROS2 工作空间 src 目录下，例如 ~/dev_ws/src/ros2_nlink_parser
source /opt/ros/humble/setup.bash
cd ~/dev_ws
colcon build
source install/setup.bash
```

## 运行

所有设备共享两个 ROS2 参数：

| 参数 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| `port_name` | `string` | `/dev/ttyCH343USB0` | 串口设备路径 |
| `baud_rate` | `int` | `921600` | 串口波特率 |

```bash
# LinkTrack
ros2 run nlink_parser linktrack --ros-args -p port_name:="/dev/ttyUSB0" -p baud_rate:=921600

# LinkTrack AoA
ros2 run nlink_parser linktrack_aoa --ros-args -p port_name:="/dev/ttyUSB0"

# IOT
ros2 run nlink_parser iot --ros-args -p port_name:="/dev/ttyUSB0"

# TOFSense
ros2 run nlink_parser tofsense --ros-args -p port_name:="/dev/ttyUSB0"

# TOFSense-M
ros2 run nlink_parser tofsensem --ros-args -p port_name:="/dev/ttyUSB0"
```

---

## API — 发布的 ROS2 话题

### LinkTrack (`linktrack`)

| 话题 | 消息类型 | 说明 |
|------|---------|------|
| `/nlink_linktrack_anchorframe0` | `LinktrackAnchorframe0` | 基站帧 |
| `/nlink_linktrack_tagframe0` | `LinktrackTagframe0` | 标签帧 |
| `/nlink_linktrack_nodeframe0` | `LinktrackNodeframe0` | 节点帧0（原始数据） |
| `/nlink_linktrack_nodeframe1` | `LinktrackNodeframe1` | 节点帧1（含3D位置） |
| `/nlink_linktrack_nodeframe2` | `LinktrackNodeframe2` | 节点帧2（含完整定位数据） |
| `/nlink_linktrack_nodeframe3` | `LinktrackNodeframe3` | 节点帧3 |
| `/nlink_linktrack_nodeframe4` | `LinktrackNodeframe4` | 节点帧4（含多标签/多基站拓扑） |
| `/nlink_linktrack_nodeframe5` | `LinktrackNodeframe5` | 节点帧5 |
| `/nlink_linktrack_nodeframe6` | `LinktrackNodeframe6` | 节点帧6（原始数据） |

**数据回传话题：** 订阅 `/nlink_linktrack_data_transmission`（`std_msgs/String`）可将数据写入串口发给设备。

### LinkTrack AoA (`linktrack_aoa`)

| 话题 | 消息类型 | 说明 |
|------|---------|------|
| `/nlink_linktrack_nodeframe0` | `LinktrackNodeframe0` | 节点帧0 |
| `/nlink_linktrack_aoa_nodeframe0` | `LinktrackAoaNodeframe0` | AoA 节点帧（含角度信息） |

**数据回传话题：** 订阅 `/nlink_linktrack_data_transmission`（`std_msgs/String`）。

### IOT (`iot`)

| 话题 | 消息类型 | 说明 |
|------|---------|------|
| `/nlink_iot_frame0` | `IotFrame0` | IOT 帧（含 UID 和节点列表） |

### TOFSense (`tofsense`)

TOFSense 工作于**轮询模式（inquire mode）**：节点以 10Hz 频率依次向 8 个传感器地址发送查询指令，汇总后发布。

| 话题 | 消息类型 | 说明 |
|------|---------|------|
| `/nlink_tofsense_cascade` | `TofsenseCascade` | 汇总帧（含最多8个 TOF 传感器数据） |

`TofsenseCascade` 内部包含 `TofsenseFrame0[] nodes`，每个 `TofsenseFrame0` 包含：

| 字段 | 类型 | 说明 |
|------|------|------|
| `id` | `uint8` | 传感器 ID |
| `system_time` | `uint32` | 系统时间戳 |
| `dis` | `float32` | 距离（米） |
| `dis_status` | `uint8` | 距离状态 |
| `signal_strength` | `uint16` | 信号强度 |
| `range_precision` | `uint8` | 量程精度 |

### TOFSense-M (`tofsensem`)

| 话题 | 消息类型 | 说明 |
|------|---------|------|
| `/nlink_tofsensem_frame0` | `TofsenseMFrame0` | TOFSense-M 帧（含多像素数据） |

`TofsenseMFrame0` 包含 `TofsenseMFrame0Pixel[] pixels`，每个像素包含 `dis`、`dis_status`、`signal_strength`。

---

## 关键消息类型详解

### LinktrackNodeframe2（最常用的定位帧）

```
std_msgs/Header header        # 时间戳
uint8 role                    # 角色
uint8 id                      # 设备 ID
uint32 local_time             # 本地时间
uint32 system_time            # 系统时间
float32 voltage               # 电压
float32[3] pos_3d             # 3D 位置 (x, y, z)
float32[3] eop_3d             # 欧拉角 (roll, pitch, yaw)
float32[3] vel_3d             # 3D 速度
float32[3] angle_3d           # 角度
float32[4] quaternion         # 四元数 (w, x, y, z)
float32[3] imu_gyro_3d        # 陀螺仪
float32[3] imu_acc_3d         # 加速度计
LinktrackNode2[] nodes        # 测距节点列表
```

`LinktrackNode2`：`uint8 role`, `uint8 id`, `float32 dis`, `float32 fp_rssi`, `float32 rx_rssi`

### LinktrackNodeframe4（多标签/多基站拓扑帧）

```
uint8 role / id / voltage
uint32 local_time / system_time
LinktrackNode4Tag[] tags      # 标签列表
```
每个 `LinktrackNode4Tag`：`uint8 id`, `float32 voltage`, `LinktrackNode4Anchor[] anchors`
每个 `LinktrackNode4Anchor`：`uint8 id`, `float32 dis`

---

## 数据流架构

```
[串口设备] ──bytes──▶ [Serial 读取] ──▶ [NProtocolExtracter] ──▶ [协议 Handler]
                                                                        │
                                                                  [UnpackFrameData]
                                                                        │
                                                                   [回调函数]
                                                                        │
                                                              [ROS2 Publisher]
                                                                        │
                                                                  [ROS2 话题]
```

1. 节点在构造函数中开启串口，进入阻塞式 `while (rclcpp::ok())` 循环不断读取串口数据
2. 原始字节送入 `NProtocolExtracter::AddNewData()` 进行帧头匹配和帧提取
3. 完整帧通过 `Verify()` 校验后调用协议 handler 的 `HandleData()`
4. `HandleData()` 调用 C 层 `UnpackFrameData()` 填充协议结构体
5. 回调函数将 C 结构体转换为 ROS2 消息并 publish

---

## 验证

### 虚拟验证（无需硬件）

详见 [virtual_verification.md](./virtual_verification.md)。核心步骤：

1. `socat -d -d pty,raw,echo=0 pty,raw,echo=0` 创建虚拟串口对
2. `python3 virtual_device.py` 模拟设备发送数据
3. `ros2 run nlink_parser linktrack --ros-args -p port_name:="/dev/pts/N"`
4. `ros2 topic echo /nlink_linktrack_nodeframe2` 查看结果

### 真实硬件

详见 [real_manipulate.md](./real_manipulate.md)。

---

## 包结构

```
ros2_nlink_parser/
├── src/
│   ├── serial/                      # 跨平台串口库（原为 submodule，已内联）
│   ├── nlink_parser_interfaces/     # 25 个 ROS2 .msg 定义
│   └── nlink_parser/                # 5 个可执行文件 + nutils 静态库
│       ├── src/                     # main() 入口
│       └── include/
│           ├── linktrack/           # LinkTrack 协议实现
│           ├── linktrack_aoa/       # LinkTrack AoA 协议实现
│           ├── iot/                 # IOT 协议实现
│           ├── tofsense/            # TOFSense 协议实现
│           ├── tofsensem/           # TOFSense-M 协议实现
│           └── utils/
│               ├── nlink_unpack/    # C99 协议解包库
│               └── protocol_extracter/  # 通用帧提取器
├── virtual_device.py                # 虚拟测试脚本
├── virtual_verification.md
└── real_manipulate.md
```

## 参考链接

- [Nooploop 官方下载中心](https://www.nooploop.com/download/)
- [原始 nlink_parser (ROS1)](https://github.com/nooploop-dev/nlink_parser)
- [主要代码来源](https://github.com/csw609/nlink_parser)

## License

- `serial` 库：MIT License
- `protocol_extracter` / `nlink_unpack`：BSD 3-Clause
- `nlink_parser` / `nlink_parser_interfaces`：TODO
