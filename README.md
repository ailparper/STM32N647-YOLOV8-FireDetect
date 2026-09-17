# STM32N647-YOLOV8-FireDetect

基于 **STM32N6**（Cortex-M55 + Neural-ART NPU）的实时 **火焰 / 烟雾** 检测项目。
应用初始化为手写驱动代码，不依赖 CubeMX 生成应用侧初始化逻辑。

## 功能特性

- YOLOv8 **int8 量化**模型，输入 640×640×3，2 类检测：`fire` / `smoke`
- IMX335 摄像头 + DCMIPP 双流水线：
  - PIPE1 → 800×480 RGB565 → LCD 后景层直显
  - PIPE2 → 640×640 RGB888 → NPU 推理输入
- 800×480 RGB LCD（LTDC 双层合成）：摄像头画面 + ARGB4444 前景叠加（检测框 / HUD）
- ThreadX 多线程流水线：`NN`（推理）/ `PP`（后处理+NMS）/ `DP`（绘制）/ `ISP`（3A）
- 信号量帧缓冲队列，生产者-消费者解耦采集与推理
- 串口调试输出（USART1，115200-8-N-1），HUD 实时显示 CPU 负载 / 推理耗时 / FPS / 目标数

## 硬件平台

| 部件 | 说明 |
|---|---|
| 主控 | STM32N647（Cortex-M55 @800MHz + NPU） |
| 摄像头 | IMX335（MIPI CSI） |
| 显示 | 800×480 RGB TFT LCD（LTDC） |
| 存储 | XSPI1 HyperRAM（16MB，帧缓冲 `.EXTRAM`）、XSPI2 NOR Flash（模型权重 + 代码） |

## 目录结构

```
STM32N647-YOLOV8-FireDetect/
├── Appli/Core/Src|Inc/     应用源码（fd_* 模块）
│   ├── fd_conf.*           全局配置（显示/相机/NN/后处理参数集中管理）
│   ├── fd_board.*          板级初始化（GPIO/RAMCFG/RIF/XSPI/NPU/时钟）
│   ├── fd_console.*        调试串口 + printf 重定向
│   ├── fd_display.*        LTDC 双层显示管理
│   ├── fd_camera.*         CMW 相机双 PIPE 配置
│   ├── fd_frame_queue.*    帧缓冲环形队列
│   ├── fd_cpuload.*        CPU 占用率统计
│   ├── fd_postprocess.*    YOLOv8 int8 后处理封装
│   ├── fd_app.*            应用编排器（4 处理线程）
│   └── main.c              入口：Cache → ThreadX → 板级初始化 → fd_app_init
├── FSBL/                   第一阶段引导
├── Drivers/                ST HAL / CMSIS / 板级驱动
├── Middlewares/            ThreadX / CMW / ISP / Vision Models PP / AI Runtime
├── Model/                  模型文件与生成代码（network.c / .tflite / 权重）
├── STM32CubeIDE/           Eclipse 工程文件（Appli / FSBL 两个子工程）
└── Secure_nsclib/          安全上下文库
```

## 使用 STM32CubeIDE 构建

1. 安装 **STM32CubeIDE**（需含 STM32N6 系列支持包）。
2. `File → Import → General → Existing Projects into Workspace`。
3. `Select root directory` 选择本仓库的 `STM32CubeIDE` 目录。
4. 勾选导入两个工程：
   - `STM32N647-YOLOV8-FireDetect_FSBL`
   - `STM32N647-YOLOV8-FireDetect_Appli`
5. **不要勾选** `Copy projects into workspace`（工程通过相对链接引用仓库文件）。
6. 分别右键两个工程 → `Build Project`。
   - 构建成功后 `Binary/` 下生成 `fsbl.hex`（经 STM32_SigningTool 签名）与 `appli.hex`。
7. **注意**：本工程应用初始化为手写代码，请勿双击 `.ioc` 用 CubeMX 重新生成，
   否则可能覆盖手写文件。

## 运行

- **调试运行**：使用工程自带调试配置
  `STM32N647-YOLOV8-FireDetect_Appli.launch`（ST-LINK 下载调试）。
  DEBUG 构建在运行时自行初始化系统时钟与外部存储器（`fd_board_debug_*`），
  不依赖 FSBL。
- **脱机运行**：将 `Binary/fsbl.hex` 与 `Binary/appli.hex` 用
  STM32CubeProgrammer（配合开发板 external loader）烧入外部 Flash，复位启动。

## 模型说明与更换

- `Model/best_full_integer_quant.tflite`：int8 量化 YOLOv8 检测模型。
- `Model/network.c` 等：ST Edge AI（`stedgeai generate`）生成的
  C 代码与权重，已随仓库提供，克隆后可直接编译。
- 更换模型流程：
  1. 训练并量化导出新的 `.tflite`；
  2. 用 ST Edge AI Developer Cloud 重新生成代码，替换 `Model/` 下生成文件；
  3. 同步修改 `Appli/Core/Inc/fd_conf.h`：
     `FD_NN_WIDTH/HEIGHT`、`FD_NN_IN_SIZE/FD_NN_OUT_SIZE`（输入输出字节数）、
     `FD_PP_TOTAL_BOXES`、`FD_PP_SCALE` / `FD_PP_ZERO_POINT`
     （反量化参数，见生成报告）、`FD_NN_CLASSES_TABLE`（类别名）。

## 许可证

详见 [NOTICE.md](NOTICE.md)。
