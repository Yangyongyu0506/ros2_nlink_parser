当然可以！在拿到实体硬件前进虚拟验证是非常专业且推荐的做法。这能确保您的代码逻辑（从数据解析到ROS2话题发布）是完全正确的。

我们将通过创建一个“虚拟设备”来模拟真实硬件。这个虚拟设备会通过一个“虚拟串口”发送和真实设备一模一样的数据流，而您的`nlink_parser`节点将连接到这个虚拟串口的另一端来接收和解析数据。

### 核心思路

**真实场景**:
`[ 真实硬件 ] <--USB/Serial--> [ /dev/ttyUSB0 ] <--nlink_parser读取`

**虚拟场景**:
`[ 模拟脚本 ] <--虚拟串口1--> [ 虚拟串口2 ] <--nlink_parser读取`

### 步骤一：安装虚拟串口工具 `socat`

`socat`是一个强大的网络工具，我们用它来创建一对相互连接的虚拟串口。在一个终端中写入的数据，可以在另一个终端中读出。

打开终端，安装`socat`：

```bash
sudo apt-get update
sudo apt-get install socat
```

### 步骤二：创建虚拟串口对

打开一个**新的终端**（我们称之为“**终端A**”），然后运行以下命令。这个终端需要一直开着，直到您测试结束。

```bash
socat -d -d pty,raw,echo=0 pty,raw,echo=0
```

运行后，`socat`会输出类似下面的信息，这表示它成功创建了一对虚拟串口：

```
2024/06/30 11:20:00 socat[12345] N PTY is /dev/pts/3
2024/06/30 11:20:00 socat[12345] N PTY is /dev/pts/4
2024/06/30 11:20:00 socat[12345] N starting data transfer loop with FDs [5,5] and [7,7]
```

**请记下这两个串口号**，例如 `/dev/pts/3` 和 `/dev/pts/4`。这就是我们虚拟的“数据线”的两端。

### 步骤三：编写“虚拟设备”Python脚本

这个脚本将扮演NLink设备的角色。它会读取`nlink_parser`项目中的示例数据，并将其发送到虚拟串口的一端。

1.  在您的工作区（例如 `~/dev_ws`）的根目录下，创建一个名为 `virtual_device.py` 的文件。

2.  将以下Python代码粘贴到 `virtual_device.py` 文件中：

    ```python
    import serial
    import time
    import sys

    # --- 配置 ---
    # !! 重要：这里的串口号必须是 socat 创建的两个串口之一 !!
    # 例如，如果socat创建了/dev/pts/3和/dev/pts/4，这里就填其中一个
    VIRTUAL_SERIAL_PORT = '/dev/pts/3' 

    # 从 nlink_parser/include/utils/nlink_unpack/example.c 文件中复制的示例数据
    # 我们以 LinkTrack Nodeframe2 为例
    nodeframe2_hex_string = "55 04 8F 00 02 01 8E 7A 77 01 03 00 00 00 00 00 D0 17 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 01 03 01 F4 01 01 FA 02 01 1E 9A"

    def hex_string_to_bytes(hex_str):
        """将十六进制字符串（如 '55 04'）转换为字节串（如 b'\x55\x04'）"""
        return bytes.fromhex(hex_str.replace(' ', ''))

    if __name__ == "__main__":
        print(f"虚拟设备启动，尝试连接到串口: {VIRTUAL_SERIAL_PORT}")
        try:
            # 打开虚拟串口
            ser = serial.Serial(VIRTUAL_SERIAL_PORT, 921600, timeout=1)
            print("串口连接成功。")
        except serial.SerialException as e:
            print(f"错误：无法打开串口 {VIRTUAL_SERIAL_PORT}。")
            print(f"请确保：")
            print(f"1. socat 正在另一个终端中运行。")
            print(f"2. 串口号 '{VIRTUAL_SERIAL_PORT}' 是由 socat 创建的。")
            print(f"异常信息: {e}")
            sys.exit(1)

        # 将示例数据转换为字节
        data_to_send = hex_string_to_bytes(nodeframe2_hex_string)
        
        print(f"准备发送数据 (共 {len(data_to_send)} 字节): {data_to_send.hex(' ').upper()}")

        # 循环发送数据，模拟设备持续工作
        try:
            while True:
                ser.write(data_to_send)
                print(f"已发送 {len(data_to_send)} 字节的数据。")
                time.sleep(0.1)  # 每 100ms 发送一次
        except KeyboardInterrupt:
            print("\n程序被用户中断。关闭串口。")
        except Exception as e:
            print(f"发送数据时发生错误: {e}")
        finally:
            ser.close()
            print("串口已关闭。")

    ```

    **注意：** 请务必将脚本中的 `VIRTUAL_SERIAL_PORT` 变量的值修改为您在步骤二中看到的两个串口号之一（例如 `/dev/pts/3`）。

### 步骤四：修改 `nlink_parser` 的代码

现在我们需要告诉`nlink_parser`去监听虚拟串口的**另一端**。

1.  打开 `nlink_parser` 的 `init_serial.cpp` 文件。
    路径: `~/dev_ws/src/nlink_parser/include/utils/init_serial.cpp`

2.  找到下面这一行：

    ```cpp
    auto port_name = "/dev/ttyUSB0";
    ```

3.  将其修改为您在步骤二中看到的**另一个**串口号。例如，如果您的Python脚本使用的是 `/dev/pts/3`，那么这里就必须使用 `/dev/pts/4`。

    ```cpp
    auto port_name = "/dev/pts/4"; // <-- 修改这里
    ```

### 步骤五：编译并运行所有程序

1.  **重新编译**: 打开一个**新的终端**（“**终端B**”），进入工作区并重新编译，以使代码修改生效。

    ```bash
    cd ~/dev_ws
    colcon build --packages-select nlink_parser
    ```

2.  **启动虚拟设备**: 打开一个**新的终端**（“**终端C**”），source您的ROS2环境，然后运行Python脚本。

    ```bash
    cd ~/dev_ws
    # 不需要 source ROS2，因为这个脚本不依赖ROS
    python3 virtual_device.py
    ```

    您应该能看到它打印出“串口连接成功”并开始发送数据。

3.  **启动nlink\_parser**: 回到**终端B**，source您的工作区，然后运行 `linktrack` 节点。

    ```bash
    cd ~/dev_ws
    source install/setup.bash
    ros2 run nlink_parser linktrack
    ```

    您应该能看到它打印出“Serial port opened successfully”。

4.  **验证数据**: 打开一个**新的终端**（“**终端D**”），source您的工作区，然后用`echo`命令查看话题。

    ```bash
    cd ~/dev_ws
    source install/setup.bash
    ros2 topic echo /nlink_linktrack_nodeframe2
    ```

如果一切顺利，您将在**终端D**中看到 `nlink_parser` 成功解析并发布的 `/nlink_linktrack_nodeframe2` 话题数据。这证明了您的代码（从串口读取、数据提取、协议解析到ROS2发布）的整个流程是完全可行的！

测试完成后，不要忘记将 `init_serial.cpp` 中的 `port_name` 改回 `/dev/ttyUSB0`，以便在拿到真实硬件后使用。
