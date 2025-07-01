# ros2_nlink_parser

这是一个将 [nlink\_parser](https://github.com/csw609/nlink_parser) 从 ROS1 迁移到 ROS2 的项目。`nlink_parser` 是用于解析深圳一诺精准（Nooploop）公司出品的 NLink 系列定位产品数据包的 ROS 功能包。

## 主要修改

为了适配 ROS2 Foxy/Galactic 及更新版本，本项目主要作了以下修改：

1.  **修改了 `serial` 库的 `CMakeLists.txt`**：调整了 `serial` 库的编译选项和依赖关系，使其能被 `ament_cmake` 正确编译和链接。
2.  **修改了 `nlink_parser/include/utils/init_serial.cpp`**：更新了串口初始化的代码，使其兼容 ROS2 的参数服务和日志系统。
3.  **ROS2 接口适配**：将全部的 ROS1 发布器（Publisher）、节点句柄（NodeHandle）等接口替换为 ROS2 对应的接口。

## 依赖

  - Robot Operating System 2 (ROS2) Foxy Fitzroy 或更高版本
  - `serial` 库 (已作为子模块包含在项目中)

## 编译指南

请按照以下步骤编译 `nlink_parser` 包：

1.  将本功能包放置在您的 ROS2 工作空间（例如 `~/dev_ws/src`）下。
2.  打开一个终端，进入您的工作空间目录。
    ```bash
    cd ~/dev_ws
    ```
3.  使用 `colcon`进行编译：
    ```bash
    colcon build --packages-select nlink_parser
    ```

## 运行与测试

### 1\. 连接真实硬件

在接入真实硬件进行测试时，请遵循以下步骤：

1.  **修改串口号**：
    打开文件 `~/dev_ws/src/nlink_parser/include/utils/init_serial.cpp`，将 `port_name` 变量修改为您设备实际连接的串口号。

    ```cpp
    // 通常是 /dev/ttyUSB0 或 /dev/ttyUSB1
    auto port_name = "/dev/ttyUSB0";
    ```

    修改后请务必**重新编译**。

2.  **查找设备串口号**：
    a. 在未连接设备时，执行 `ls /dev/ttyUSB*`。
    b. 连接您的 NLink 设备。
    c. 再次执行 `ls /dev/ttyUSB*`，新出现的设备名就是您需要的串口号。

3.  **配置串口权限**：
    为了避免权限问题，请将您的用户添加到 `dialout` 组。

    ```bash
    sudo usermod -a -G dialout $USER
    ```

    **重要提示**：执行此命令后，您需要**重启电脑或重新登录**才能使权限生效。

4.  **运行节点**：
    a. Source 您的工作空间：

    ```bash
    source ~/dev_ws/install/setup.bash
    ```

    b. 根据您的设备类型，运行对应的节点。例如，如果使用 LinkTrack 设备：

    ```bash
    ros2 run nlink_parser linktrack
    ```

    c. 打开另一个终端，使用 `ros2 topic echo` 查看相应话题的数据流，以验证节点是否正常工作。

    ```bash
    # 话题名称请根据 ros2 topic list 的实际输出来确定
    ros2 topic echo /nlink_linktrack_nodeframe2
    ```

### 2\. 使用虚拟设备进行验证

如果您手边没有硬件，也可以使用虚拟串口进行逻辑验证。

1.  **安装 `socat`**：
    `socat` 是一个强大的工具，可以创建虚拟串口对。

    ```bash
    sudo apt-get install socat
    ```

2.  **创建虚拟串口**：
    打开一个终端，运行以下命令。它会创建两个互相连通的虚拟串口（例如 `/dev/pts/2` 和 `/dev/pts/3`）。请保持此终端开启。

    ```bash
    socat -d -d pty,raw,echo=0 pty,raw,echo=0
    ```

3.  **修改代码以使用虚拟串口**：
    a. 修改 `nlink_parser/include/utils/init_serial.cpp`，将 `port_name` 指向 `socat` 创建的一个端口（如 `/dev/pts/2`）。
    b. 修改 `virtual_device.py` 脚本，将 `VIRTUAL_SERIAL_PORT` 变量指向 `socat` 创建的**另一个**端口（如 `/dev/pts/3`）。
    c. **重新编译** `nlink_parser` 包。

4.  **运行虚拟设备和节点**：
    a. 打开一个新终端，运行虚拟设备脚本，它会向虚拟串口持续发送模拟数据。

    ```bash
    python3 virtual_device.py
    ```

    b. 打开另一个新终端，运行 `nlink_parser` 节点。如果一切配置正确，它会接收并解析来自虚拟设备的数据。

## 参考链接

  - **Nooploop 官方下载中心**: [https://www.nooploop.com/download/](https://www.nooploop.com/download/)
  - **原始 nlink\_parser (ROS2) 仓库**: [https://github.com/csw609/nlink\_parser](https://github.com/csw609/nlink_parser)