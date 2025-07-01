# ros2_nlink_parser

这是一个将 [nlink\_parser](https://github.com/nooploop-dev/nlink_parser) 从 ROS1 迁移到 ROS2 的项目，主要在大神[csw609](https://github.com/csw609/nlink_parser)的项目上进行修改。
`nlink_parser` 是用于解析深圳一诺精准（Nooploop）公司出品的 NLink 系列定位产品数据包的 ROS 功能包。

## 主要修改

为了适配 ROS2 humble 及更新版本，本项目在参考项目基础上主要作了以下修改：

1.  **修改了 `serial` 库的 `CMakeLists.txt`**：调整了 `serial` 库的编译选项和依赖关系，用ROS2语法进行覆写，使其能在 `colcon build` 时正确编译，并被其他库链接。
2.  **修改了 `nlink_parser/include/utils/init_serial.cpp`**：在虚拟验证或真实硬件连接时更新串口名和波特率等参数，确保与实际设备匹配。
3.  **ROS2 接口适配**：将全部的 ROS1 发布器（Publisher）、节点句柄（NodeHandle）等接口替换为 ROS2 对应的接口。

## 依赖

  - Robot Operating System 2 (ROS2) humble 或更高版本
  - [serial](https://github.com/nooploop-dev/serial)库 (已作为子模块包含在项目中)

## 编译指南

请按照以下步骤编译 `nlink_parser` 包：

1.  将本功能包放置在您的 ROS2 工作空间（例如 `~/dev_ws/src`）下。
2.  打开一个终端，进入您的工作空间目录。
    ```bash
    cd ~/dev_ws
    ```
3.  使用 `colcon`进行编译：
    ```bash
    colcon build 
    ```

## 运行与测试

### 1\. 使用虚拟设备进行验证

操作步骤详见virtual_verification.md文件。

### 2\. 连接真实硬件

操作步骤详见real_manipulate.md文件。

## 参考链接

  - **Nooploop 官方下载中心**: [https://www.nooploop.com/download/](https://www.nooploop.com/download/)
  - **原始 nlink\_parser (ROS1) 仓库**: [https://github.com/nooploop-dev/nlink_parser](https://github.com/nooploop-dev/nlink_parser)
  - **主要代码来源**: [https://github.com/csw609/nlink_parser](https://github.com/csw609/nlink_parser)

## 致谢

这是本人第一个项目，可能存在诸多不足之处，欢迎大家提出宝贵意见和建议。如有错误，敬请指点！