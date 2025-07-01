太棒了！跑通虚拟验证是项目开发中一个至关重要的里程碑。这证明了您的软件代码逻辑是正确的，现在我们可以满怀信心地接入真实硬件了。

上实机测试的流程和虚拟验证很相似，但核心是**与真实的物理端口进行交互**，并处理可能出现的权限和配置问题。

以下是详细的步骤指南：

### 第 1 步：将代码改回硬件配置

您需要将在虚拟测试时对代码的修改还原，让程序去监听真实的物理串口。

1.  **修改`init_serial.cpp`**:

      * 打开文件 `~/dev_ws/src/nlink_parser/include/utils/init_serial.cpp`。
      * 找到您之前修改的 `port_name` 变量。
      * 将其从虚拟端口（如 `/dev/pts/4`）改回通常的物理USB串口名称：
        ```cpp
        // 将下面这行代码改回
        auto port_name = "/dev/ttyUSB0";
        ```

2.  **重新编译**:
    因为您修改了代码，所以需要重新编译 `nlink_parser` 包。

    ```bash
    cd ~/dev_ws
    colcon build --packages-select nlink_parser
    ```

### 第 2 步：连接硬件并确定串口号

这是最关键的一步。`/dev/ttyUSB0` 只是一个常见的默认名称，您的系统可能会将其识别为 `/dev/ttyUSB1` 或其他名称。

1.  **先不要连接设备**：打开一个终端，运行以下命令，看看当前系统有哪些USB串口设备。很可能什么都没有。

    ```bash
    ls /dev/ttyUSB*
    ```

2.  **连接您的NLink设备**：将您的LinkTrack或其他设备通过USB线连接到电脑。

3.  **再次运行命令**：在同一个终端中，再次运行：

    ```bash
    ls /dev/ttyUSB*
    ```

    现在，您应该能看到一个新的设备文件出现了，例如 `/dev/ttyUSB0`。**这个新出现的名称，就是您设备真正的串口号！**

4.  **(备选方法) 使用 `dmesg`**：如果上面的方法不清晰，您也可以使用 `dmesg` 命令，它会显示系统内核日志，通常会明确指出设备连接到了哪个tty。

    ```bash
    dmesg | grep tty
    ```

    您会在输出的最后几行看到类似 `cdc_acm 1-2:1.0: ttyACM0: USB ACM device` 或 `usb 1-1.2: ch341-uart converter now attached to ttyUSB0` 的信息。

### 第 3 步：检查并设置串口权限

在Linux系统中，普通用户默认可能没有直接读写串口的权限，这会导致程序运行时报 “Permission denied” 或 “无法打开串口” 的错误。

1.  **运行以下命令**，将您当前的用户添加到 `dialout` 用户组（这个组通常拥有串口访问权限）。

    ```bash
    sudo usermod -a -G dialout $USER
    ```

2.  **重要：为了让权限生效，您需要完全注销当前用户或重启电脑。** 这是最简单可靠的方法。

### 第 4 步：运行并验证

现在，一切准备就绪，可以进行最终测试了。

1.  **Source工作区**：打开一个新终端，source您的工作区环境。

    ```bash
    cd ~/dev_ws
    source install/setup.bash
    ```

2.  **运行节点**：根据您的设备类型，运行对应的可执行文件。

      * 如果您的设备是LinkTrack，运行：
        ```bash
        ros2 run nlink_parser linktrack
        ```
      * 如果您的设备是TOFSense，运行：
        ```bash
        ros2 run nlink_parser tofsense
        ```

    如果一切正常，您应该会看到程序打印出 `Serial port opened successfully, waiting for data.`。

3.  **验证ROS话题**：打开**另一个**新的终端，source工作区后，使用ROS2工具进行验证。

      * 首先，查看话题列表，确认您的数据话题（如 `/nlink_linktrack_nodeframe2`）是否已经出现。
        ```bash
        ros2 topic list
        ```
      * 然后，用`echo`命令实时查看数据流。
        ```bash
        ros2 topic echo /nlink_linktrack_nodeframe2 
        ```
        （请将话题名换成您实际看到的话题）

如果终端上开始持续打印出从硬件解析的实时数据，那么恭喜您，您的项目已经完全在实机上成功运行了！