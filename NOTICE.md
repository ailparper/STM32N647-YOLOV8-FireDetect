# NOTICE — 许可证与第三方组件说明

本仓库采用**分层许可**：不同目录下的代码来自不同来源，各自遵循其原始许可证。
**请勿对本仓库整体施加单一开源许可证**——ST 与 Microsoft 的组件明确禁止将其
软件置于开源条款之下（SLA0044 / SLA0104）。

## 分层许可总览

| 层 | 范围 | 许可 | 版权方 |
|---|---|---|---|
| 应用层（本项目原创） | `Appli/Core/Src/fd_*.c`、`Appli/Core/Inc/fd_*.h`、`Appli/Core/Src/main.c`、`Appli/Core/Inc/main.h` | MIT（见下文） | 本项目作者 |
| ST HAL / CMSIS / 板级驱动适配层 | `Drivers/STM32N6xx_HAL_Driver/`、`Drivers/CMSIS/`、`Appli/Core/Src/stm32n6xx_*.c`、`Appli/Core/Src/system_stm32n6xx_s.c`、`Appli/Core/Src/secure_nsc.c`、`FSBL/` | SLA0044（ST License）保留原始许可与版权头 | STMicroelectronics |
| ST 中间件 | `Middlewares/STM32_XXX/`（CMW、ISP、Vision Models PP 等）、`Utilities/`、`Drivers/BSP/`（LCD/BUS/HyperRAM/NORFlash/LED 等） | SLA0044 或随文件版权头声明 | STMicroelectronics |
| ST Edge AI 生成代码 | `Model/st_ai_output/`（network.c 等）、`Middlewares/` 内 AI Runtime（LL_ATON） | SLA0104（ST Edge AI license） | STMicroelectronics |
| ThreadX / Azure RTOS | `Middlewares/ST/threadx*`、`Appli/ThreadX/` | Microsoft EULA（随文件声明） | Microsoft / Eclipse Foundation |
| 正点原子（ALIENTEK）来源部分 | 部分板级初始化时序参数（GPIO/LCD 时序等）参考自正点原子 N647 例程 | 版权归广州市星翼电子科技有限公司，此处按其资料再分发惯例保留出处说明 | 广州市星翼电子科技有限公司 |
| AI 模型 | `Model/best_full_integer_quant.tflite` 及 ONNX/量化文件、`Model/st_ai_output/` 权重数据 | 模型基于 Ultralytics YOLOv8 训练导出，遵循 **AGPL-3.0** | 模型训练者 / Ultralytics |

## 各层义务要点

1. **MIT（应用层）**：上述 `fd_*` 与 `main.*` 文件可自由使用、修改、再分发，
   须保留版权与许可声明。
2. **SLA0044 / SLA0104（ST 组件）**：允许随项目源码再分发，但
   - 必须保留原文件中的 ST 版权与许可头；
   - **不得**将 ST 软件置于任何“开源条款”（open source terms）之下发布；
   - 详见 `Drivers/STM32N6xx_HAL_Driver/LICENSE` 及 `Model/st_ai_output/LICENSE.txt`。
3. **ThreadX（Microsoft）**：使用与再分发遵循随附的 Microsoft 软件许可条款。
4. **正点原子来源部分**：本项目为学习与开源目的重构，保留了必要的出处说明；
   如需商业用途请自行与原厂确认。
5. **YOLOv8 / AGPL-3.0**：若您重新训练或修改模型并对外提供网络服务，
   AGPL-3.0 的传染条款可能适用于您的衍生模型，请自行评估合规义务。

## 贡献说明

向本仓库提交的应用层代码默认视为以 MIT 许可授权；
修改 ST / Microsoft / 第三方来源的文件时，请保持其原始许可证不变。
