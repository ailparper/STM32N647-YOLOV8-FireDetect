# NOTICE — 许可证与第三方组件说明

本仓库采用**分层许可**：不同目录下的代码来自不同来源，各自遵循其原始许可证。

根目录的 `LICENSE`（**GPL-3.0**）**仅覆盖本项目原创的应用层代码**
（`Appli/Core/Src/fd_*.c`、`Appli/Core/Inc/fd_*.h`、`Appli/Core/Src/main.c`、
`Appli/Core/Inc/main.h`）。**GPL-3.0 不适用于，也不得被解释为适用于**
下述任何 ST、Microsoft 或其他第三方组件——这些组件的许可（SLA0044 / SLA0104 /
Microsoft EULA）明确禁止将其软件置于开源条款之下。

## 分层许可总览

| 层 | 范围 | 许可 | 版权方 |
|---|---|---|---|
| 应用层（本项目原创） | `Appli/Core/Src/fd_*.c`、`Appli/Core/Inc/fd_*.h`、`Appli/Core/Src/main.c`、`Appli/Core/Inc/main.h` | **GPL-3.0**（见根目录 `LICENSE`） | 本项目作者 |
| ST HAL / CMSIS | `STM32Cube_FW_N6_V1.0.0/Drivers/STM32N6xx_HAL_Driver/`、`STM32Cube_FW_N6_V1.0.0/Drivers/CMSIS/` | SLA0044（ST License），保留原始许可与版权头 | STMicroelectronics |
| ST 板级/适配代码 | `Appli/Core/Src/stm32n6xx_*.c`、`Appli/Core/Src/system_stm32n6xx_s.c`、`Appli/Core/Src/secure_nsc.c`、`FSBL/`、`Drivers/BSP/`（LCD/BUS/HyperRAM/NORFlash/LED）、`Secure_nsclib/` | SLA0044 或随文件版权头声明 | STMicroelectronics |
| ST 中间件 | `Middlewares/STM32_MW_CAMERA/`、`Middlewares/STM32_MW_ISP/`、`Middlewares/STM32_VISION_MODELS_PP/`、`STM32Cube_FW_N6_V1.0.0/Utilities/`（lcd、Fonts 等） | SLA0044 或随文件版权头声明 | STMicroelectronics |
| ST Edge AI 运行时与生成代码 | `Middlewares/AI/`（LL_ATON 运行时）、`Model/network.c`、`Model/network_ecblobs.h` | SLA0104（ST Edge AI License） | STMicroelectronics |
| ThreadX / Azure RTOS | `Middlewares/THREADX/`、`Appli/ThreadX/` | Microsoft EULA（见 `Middlewares/THREADX/LICENSE.txt` 与 `LICENSED-HARDWARE.txt`） | Microsoft Corporation |
| AI 模型权重 | `Model/best_full_integer_quant.tflite` | 模型基于 Ultralytics YOLOv8 训练导出，遵循 **AGPL-3.0** | 模型训练者 / Ultralytics |

## 各层义务要点

1. **GPL-3.0（仅原创应用层）**：上述 `fd_*` 与 `main.*` 文件按 GPL-3.0
   自由使用、修改、再分发，修改后再分发须提供完整对应源码并保持相同许可。
2. **SLA0044 / SLA0104（ST 组件）**：允许随项目源码再分发，但
   - 必须保留原文件中的 ST 版权与许可头；
   - **不得**将 ST 软件置于任何“开源条款”（open source terms）之下发布；
   - ST 代码的许可条款以其随附文件中的声明为准。
3. **ThreadX（Microsoft EULA）**：使用与再分发遵循
   `Middlewares/THREADX/LICENSE.txt` 的条款，且仅限于配合获许可的 ST 硬件使用。
4. **YOLOv8 / AGPL-3.0（模型）**：若重新训练或修改模型并对外提供
   （含网络服务方式），AGPL-3.0 的传染条款可能适用于衍生模型，请自行评估。

## 贡献说明

向本仓库提交的原创应用层代码默认视为以 GPL-3.0 许可授权；
修改 ST / Microsoft / 第三方来源的文件时，请保持其原始许可证不变。
