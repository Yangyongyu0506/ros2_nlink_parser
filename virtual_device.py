import serial
import time
import sys

# --- 配置 ---
# !! 重要：这里的串口号必须是 socat 创建的两个串口之一 !!
# 例如，如果socat创建了/dev/pts/3和/dev/pts/4，这里就填其中一个
VIRTUAL_SERIAL_PORT = '/dev/pts/2' 

# 从 nlink_parser/include/utils/nlink_unpack/example.c 文件中复制的示例数据
# 我们换成使用 LinkTrack NodeFrame1 的数据进行测试
nodeframe1_hex_string = "55 03 44 00 03 00 e8 80 00 00 00 86 00 00 01 51 01 e8 01 f2 02 02 92 09 48 13 02 02 00 5f 0b 00 86 09 00 9b ff ff c9 37 8a 34 06 ee 37 3f aa 02 02 93 09 00 45 09 00 c4 fc ff 8d 09 00 66 09 00 c4 fc ff 8e"



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
    data_to_send = hex_string_to_bytes(nodeframe1_hex_string)

    print(f"准备发送数据 (共 {len(data_to_send)} 字节): {data_to_send.hex(' ').upper()}")

    # 循环发送数据，模拟设备持续工作
    try:
        while True:
            ser.write(data_to_send)
            print(f"已发送 {len(data_to_send)} 字节的数据。")
            time.sleep(1)  # 每 100ms 发送一次
    except KeyboardInterrupt:
        print("\n程序被用户中断。关闭串口。")
    except Exception as e:
        print(f"发送数据时发生错误: {e}")
    finally:
        ser.close()
        print("串口已关闭。")