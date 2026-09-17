/**
  ******************************************************************************
  * @file    fd_console.h
  * @brief   调试串口控制台（USART1, PE5-TX / PE6-RX, 115200-8-N-1）
  *
  *          printf 经 __io_putchar 重定向到 USART1。
  ******************************************************************************
  */

#ifndef __FD_CONSOLE_H
#define __FD_CONSOLE_H

#include "stm32n6xx_hal.h"

void fd_console_init(uint32_t baudrate);

#endif /* __FD_CONSOLE_H */
