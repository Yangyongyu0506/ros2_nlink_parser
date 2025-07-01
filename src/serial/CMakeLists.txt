# 使用ROS2推荐的最低版本
cmake_minimum_required(VERSION 3.5)
project(serial)

# 确保使用C++14标准
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

# 找到ament_cmake，这是ROS2构建系统的核心
find_package(ament_cmake REQUIRED)

## 保留原始文件中的源文件列表逻辑
set(serial_SRCS
    src/serial.cc
)
if(APPLE)
    # 如果是苹果系统
    find_library(IOKIT_LIBRARY IOKit)
    find_library(FOUNDATION_LIBRARY Foundation)
    list(APPEND serial_SRCS src/impl/unix.cc)
    list(APPEND serial_SRCS src/impl/list_ports/list_ports_osx.cc)
elseif(UNIX)
    # 如果是Linux等其他Unix系统
    list(APPEND serial_SRCS src/impl/unix.cc)
    list(APPEND serial_SRCS src/impl/list_ports/list_ports_linux.cc)
else()
    # 如果是Windows系统
    list(APPEND serial_SRCS src/impl/win.cc)
    list(APPEND serial_SRCS src/impl/list_ports/list_ports_win.cc)
endif()

## 添加库
add_library(${PROJECT_NAME} ${serial_SRCS})
# 设置头文件包含路径
target_include_directories(${PROJECT_NAME} PUBLIC
    # 在构建时，包含源码目录下的include文件夹
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    # 在安装后，包含安装目录下的include文件夹
    $<INSTALL_INTERFACE:include>
)

## 保留原始文件中的链接库逻辑
if(APPLE)
    target_link_libraries(${PROJECT_NAME} PUBLIC ${FOUNDATION_LIBRARY} ${IOKIT_LIBRARY})
elseif(UNIX)
    target_link_libraries(${PROJECT_NAME} PUBLIC rt pthread)
else()
    target_link_libraries(${PROJECT_NAME} PUBLIC setupapi)
endif()

## 保留示例程序的构建（可选，如果您不需要可以删除这部分）
add_executable(serial_example examples/serial_example.cc)
add_dependencies(serial_example ${PROJECT_NAME})
target_link_libraries(serial_example ${PROJECT_NAME})


### --- 核心修改：使用ament进行安装 --- ###

# 1. 安装库文件和可执行文件
install(TARGETS ${PROJECT_NAME} serial_example
    # 导出目标，以便其他ament包可以find_package(serial)
    EXPORT ${PROJECT_NAME}
    # 静态库 (.a) 安装到 lib 目录
    ARCHIVE DESTINATION lib
    # 动态库 (.so) 安装到 lib 目录
    LIBRARY DESTINATION lib
    # 可执行文件 (serial_example) 安装到 lib/<pkg_name>/ 目录，这是ROS2的惯例
    RUNTIME DESTINATION lib/${PROJECT_NAME}
)

# 2. 安装头文件
install(
  DIRECTORY include/
  DESTINATION include
)

# 3. ament导出规则，这是让其他ROS2包能找到这个库的关键
ament_export_include_directories(include)
ament_export_libraries(${PROJECT_NAME})
ament_export_dependencies(ament_cmake)

# 4. 完成ament包的定义
ament_package()