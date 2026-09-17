/**
  ******************************************************************************
  * @file    main.c
  * @brief   应用入口：使能 Cache、启动 ThreadX 内核，Main 线程完成
  *          板级初始化后进入 fd_app_init() 应用流水线
  ******************************************************************************
  */

#include "main.h"
#include "tx_api.h"
#include "tx_initialize.h"
#include "fd_board.h"
#include "fd_console.h"
#include "fd_app.h"
#include <stdio.h>

static TX_THREAD main_thread;
static UCHAR main_thread_stack[4096];

static VOID main_thread_entry(ULONG id);

/**
  * @brief  应用入口
  * @note   初始化 CPU Cache 后直接进入 ThreadX 内核，
  *         后续外设初始化全部在 Main 线程中完成
  */
int main(void)
{
  /* 使能 M55 I/D Cache（MSCR 标志须在 SCB 使能前置位） */
  MEMSYSCTL->MSCR |= MEMSYSCTL_MSCR_ICACTIVE_Msk;
  MEMSYSCTL->MSCR |= MEMSYSCTL_MSCR_DCACTIVE_Msk;

  SCB_EnableICache();
  SCB_EnableDCache();

  SystemCoreClockUpdate();
  HAL_Init();

  /* ThreadX 内核启动，入口为 tx_application_define() */
  _tx_initialize_kernel_setup();
  tx_kernel_enter();

  while (1)
  {
  }
}

/**
  * @brief  ThreadX 应用定义：创建 Main 线程（最低优先级）
  */
VOID tx_application_define(VOID *first_unused_memory)
{
  if (tx_thread_create(&main_thread, "Main Thread", main_thread_entry, 0,
                       main_thread_stack, sizeof(main_thread_stack),
                       TX_MAX_PRIORITIES - 1, TX_MAX_PRIORITIES - 1,
                       10, TX_AUTO_START) != TX_SUCCESS)
  {
    Error_Handler();
  }
}

/**
  * @brief  Main 线程：板级初始化时序
  * @note   DEBUG 构建从调试器直接加载运行（未经 FSBL），需要
  *         手动配置系统时钟并映射外部存储器；生产构建由 FSBL 完成这两步。
  */
static VOID main_thread_entry(ULONG id)
{
#ifdef DEBUG
  fd_board_debug_clock_init();
#endif
  SystemCoreClockUpdate();

  fd_board_init();

#ifdef DEBUG
  fd_board_debug_extmem_init();
#endif

  fd_console_init(115200);
  printf("STM32N647-YOLOV8-FireDetect\r\n");

  fd_board_ai_accel_init();
  fd_board_lowpower_clock_init();

  fd_app_init();

  /* 应用线程已接管，挂起 Main 线程 */
  tx_thread_suspend(&main_thread);
}
