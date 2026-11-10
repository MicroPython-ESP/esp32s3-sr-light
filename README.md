===========
ESP32-S3 语音控制 WS2812 灯工程说明
===========
【项目名称】esp32s3-sr-light
【目标芯片】ESP32-S3 N16R8（16MB Flash / 8MB Octal PSRAM）
【开发框架】ESP-IDF v5.3
【当前固件版本】v0.1 灯控基础版（语音识别模块暂未启用）

【项目简介】
这是一个基于乐鑫 ESP32-S3 开发板的离线语音控制 WS2812 RGB 灯项目，原设计通过 INMP441 数字麦克风采集语音，结合 ESP-SR 离线语音识别引擎实现本地语音控制，不需要联网。当前已完成 WS2812 灯条驱动的移植与编译，验证基础灯控功能；语音识别模块因组件 API 版本差异正在适配。

------------------------------
硬件接线说明
------------------------------
WS2812 灯条：
  DIN 数据线  ->  GPIO48
  VCC        ->  3.3V / 5V
  GND        ->  GND

INMP441 I2S 数字麦克风（语音功能用，当前未启用）：
  SCK 时钟线  ->  GPIO4
  WS 字选择  ->  GPIO5
  SD 数据线  ->  GPIO6
  VDD        ->  3.3V
  GND        ->  GND

------------------------------
当前固件功能（v0.1 灯控基础版）
------------------------------
1. WS2812 灯条驱动正常，支持单颗 LED 控制
2. 上电自检：亮绿灯 1 秒后熄灭，确认驱动运行正常
3. 内置亮度管理：默认亮度 50%，最高限制 70%，保护灯珠与开发板
4. 预留全部灯效逻辑与语音指令解析代码，后续可快速接入语音识别

------------------------------
设计中的完整语音控制指令
------------------------------
基础状态控制：
  关灯       -> 关闭灯
  彩虹渐变   -> 自动循环彩虹色

常亮模式（X 为 8 种预设颜色）：
  红灯亮 / 橙灯亮 / 黄灯亮 / 绿灯亮 / 青灯亮 / 蓝灯亮 / 紫灯亮 / 白灯亮

闪烁模式（X 为 8 种预设颜色）：
  红灯闪烁 / 橙灯闪烁 / 黄灯闪烁 / 绿灯闪烁 / 青灯闪烁 / 蓝灯闪烁 / 紫灯闪烁 / 白灯闪烁

呼吸模式（X 为 8 种预设颜色）：
  红灯呼吸 / 橙灯呼吸 / 黄灯呼吸 / 绿灯呼吸 / 青灯呼吸 / 蓝灯呼吸 / 紫灯呼吸 / 白灯呼吸

调节类指令：
  亮一点     -> 亮度提升 10%（上限 70%）
  暗一点     -> 亮度降低 10%（下限 10%）
  快一点     -> 闪烁速度加快（250ms 间隔）
  慢一点     -> 闪烁速度变慢（1000ms 间隔）

8 种预设颜色 RGB 值：
  红 (255,0,0)    橙 (255,120,0)  黄 (255,255,0)  绿 (0,255,0)
  青 (0,255,200)  蓝 (0,0,255)    紫 (180,0,255)  白 (255,255,255)

------------------------------
固件烧录参数
------------------------------
波特率：460800
Flash 模式：DIO
Flash 大小：16MB
烧录地址：
  bootloader.bin        -> 0x0
  partition-table.bin   -> 0x8000
  esp32s3_sr_light.bin  -> 0x10000

------------------------------
后续开发计划
------------------------------
1. 适配 ESP-SR 2.x 版本底层 API，替换原高层接口，接入离线语音唤醒与命令识别
2. 接入 INMP441 麦克风 I2S 采集驱动
3. 增加更多灯效与自定义语音指令
4. 测试全功能语音控制流程

===========
ESP32-S3 Voice-Controlled WS2812 LED Project
===========
Project Name: esp32s3-sr-light
Target Chip: ESP32-S3 N16R8 (16MB Flash / 8MB Octal PSRAM)
Framework: ESP-IDF v5.3
Current Firmware Version: v0.1 Basic LED Control (Voice recognition module disabled temporarily)

Project Overview:
This is an offline voice-controlled WS2812 RGB LED project built on the Espressif ESP32-S3 development board. It is designed to use an INMP441 digital microphone to capture voice input, paired with the ESP-SR offline speech recognition engine for local, no-network voice control. The WS2812 LED strip driver has been ported and successfully compiled; the voice recognition module is being adapted due to component API version differences.

Hardware Wiring:
WS2812 LED Strip:
  DIN  -> GPIO48
  VCC  -> 3.3V / 5V
  GND  -> GND

INMP441 I2S Digital Microphone (for voice function, currently disabled):
  SCK  -> GPIO4
  WS   -> GPIO5
  SD   -> GPIO6
  VDD  -> 3.3V
  GND  -> GND

Current Firmware Features (v0.1):
1. Fully working WS2812 LED strip driver, single LED control supported
2. Power-on self-test: blinks green for 1 second then turns off to confirm driver is running
3. Built-in brightness management: default 50% brightness, capped at 70% maximum to protect LEDs and the board
4. All light effect logic and voice command parsing code is preserved, ready for quick integration with the voice recognition module

Designed Voice Control Commands:
Basic control:
  "Turn off"              -> Turn off light
  "Rainbow cycle"         -> Auto rainbow color loop

Solid color mode (X = 8 preset colors):
  "Red on / Orange on / Yellow on / Green on / Cyan on / Blue on / Purple on / White on"

Blink mode (X = 8 preset colors):
  "Red blink / Orange blink / Yellow blink / Green blink / Cyan blink / Blue blink / Purple blink / White blink"

Breathing mode (X = 8 preset colors):
  "Red breathe / Orange breathe / Yellow breathe / Green breathe / Cyan breathe / Blue breathe / Purple breathe / White breathe"

Adjustment commands:
  "Brighter"              -> Increase brightness by 10% (max 70%)
  "Dimmer"                -> Decrease brightness by 10% (min 10%)
  "Faster"                -> Speed up blink (250ms interval)
  "Slower"                -> Slow down blink (1000ms interval)

8 Preset Colors (RGB values):
  Red (255,0,0) / Orange (255,120,0) / Yellow (255,255,0) / Green (0,255,0)
  Cyan (0,255,200) / Blue (0,0,255) / Purple (180,0,255) / White (255,255,255)

# WS2812 driver component v3.0.3

Flashing Parameters:
Baud rate: 460800
Flash mode: DIO
Flash size: 16MB
Flash offsets:
  bootloader.bin        -> 0x0
  partition-table.bin   -> 0x8000
  esp32s3_sr_light.bin  -> 0x10000

Roadmap:
1. Adapt to ESP-SR v2.x low-level API, replace legacy high-level interface, enable offline wake word and command recognition
2. Integrate INMP441 I2S audio capture driver
3. Add more light effects and custom voice commands
4. Test full end-to-end voice control workflow
