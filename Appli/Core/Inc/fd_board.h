/**
  ******************************************************************************
  * @file    fd_board.h
  * @brief   板级硬件初始化
  *
  *          覆盖：GPIO、RAMCFG、RIF 隔离、DMA2D/I2C2/LTDC/XSPI 外设、
  *          NPU/IAC/NPU-Cache、低功耗时钟，以及调试模式下的
  *          系统时钟与外部存储器（HyperRAM/NORFlash）初始化。
  ******************************************************************************
  */

#ifndef __FD_BOARD_H
#define __FD_BOARD_H

#include "stm32n6xx_hal.h"

/* 板级统一初始化：GPIO + RAMCFG + RIF 隔离配置 */
void fd_board_init(void);

/* AI 加速子系统：NPU 复位使能 + IAC + NPU Cache */
void fd_board_ai_accel_init(void);

/* 允许所有总线/存储器域在低功耗模式下保持时钟 */
void fd_board_lowpower_clock_init(void);

/* ---- 以下仅 DEBUG（从调试器直接加载运行，未经过 FSBL）时需要 ---- */
void fd_board_debug_clock_init(void);   /* PLL 时钟树（800MHz CPU 等价配置） */
void fd_board_debug_extmem_init(void);  /* XSPI1 HyperRAM / XSPI2 NORFlash 映射 */

/* ---- 由 bsp_lcd.c / CMW 中间件按名字调用的外设初始化（保持原接口名） ---- */
void MX_DMA2D_Init(void);
void MX_I2C2_Init(void);
void MX_LTDC_Init(void);
void MX_XSPI1_Init(void);
void MX_XSPI2_Init(void);

#endif /* __FD_BOARD_H */
