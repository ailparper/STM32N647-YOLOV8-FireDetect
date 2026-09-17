/**
  ******************************************************************************
  * @file    main.h
  * @brief   应用入口公共头文件
  *
  *          供 BSP（bsp_lcd.c 等）、MSP、中断与 NSC 模块包含，
  *          声明它们按名字调用的外设初始化接口与错误处理函数。
  ******************************************************************************
  */

#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* 非安全调用属性（secure_nsc.c 使用） */
#if defined ( __ICCARM__ )
#  define CMSE_NS_CALL  __cmse_nonsecure_call
#  define CMSE_NS_ENTRY __cmse_nonsecure_entry
#else
#  define CMSE_NS_CALL  __attribute((cmse_nonsecure_call))
#  define CMSE_NS_ENTRY __attribute((cmse_nonsecure_entry))
#endif

#include "stm32n6xx_hal.h"

/* 非安全函数指针类型（secure_nsc.c 使用） */
#if defined ( __ICCARM__ )
typedef void (CMSE_NS_CALL *funcptr)(void);
#else
typedef void CMSE_NS_CALL (*funcptr)(void);
#endif

typedef funcptr funcptr_NS;

/* 错误处理（实现在 fd_board.c） */
void Error_Handler(void);

/* 由 BSP/中间件按名字调用的外设初始化（实现在 fd_board.c） */
void MX_XSPI1_Init(void);
void MX_XSPI2_Init(void);
void MX_DMA2D_Init(void);
void MX_LTDC_Init(void);
void MX_I2C2_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
