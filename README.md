# STM32N647-YOLOV8-FireDetect

基于 STM32N647（Cortex-M55 + NPU）的边缘端火焰 / 烟雾视觉检测示例。

## 功能

- 通过 IMX335 摄像头采集图像（DCMIPP + ISP）
- 在 NPU 上运行 int8 量化的 YOLOv8 目标检测模型，识别 **fire / smoke** 两类目标
- 800×480 RGB LCD 实时显示画面，并叠加检测框与状态信息（推理耗时、帧率、CPU 负载）
- 基于 ThreadX RTOS 的多线程流水线（采集 / 推理 / 后处理 / 显示）

## 硬件与软件栈

- 硬件：STM32N647 开发板、IMX335 摄像头模组、800×480 RGB LCD、XSPI HyperRAM
- RTOS：ThreadX（Azure RTOS）
- AI：ST Edge AI（LL_ATON 运行时）+ Ultralytics YOLOv8 量化模型
- 中间件：STM32 相机中间件（CMW）、ISP 中间件、Vision Models Post-Processing
- 开发环境：STM32CubeIDE

## 构建说明

在 STM32CubeIDE 中导入 `STM32CubeIDE` 目录下的两个工程（FSBL 与 Appli），
分别构建后烧录。注意导入时不要勾选 "Copy projects into workspace"。

## 许可证

本仓库为多许可混合，**不适用单一开源许可证**。各组件的许可范围与义务
详见 [NOTICE.md](NOTICE.md)。
