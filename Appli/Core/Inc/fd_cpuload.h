/**
  ******************************************************************************
  * @file    fd_cpuload.h
  * @brief   基于 ThreadX Execution Profile Kit 的 CPU 占用率统计
  *
  *          依赖 tx_user.h 中使能 TX_EXECUTION_PROFILE_ENABLE。
  *          update() 每秒采样一次滑动历史，get_info() 输出各窗口负载。
  ******************************************************************************
  */

#ifndef __FD_CPULOAD_H
#define __FD_CPULOAD_H

#include "fd_conf.h"

typedef struct
{
    uint64_t current_total;
    uint64_t current_thread_total;
    uint64_t prev_total;
    uint64_t prev_thread_total;
    struct
    {
        uint64_t total;
        uint64_t thread;
        uint32_t tick;
    } history[FD_CPU_LOAD_HISTORY_DEPTH];
} fd_cpuload_t;

void fd_cpuload_init(fd_cpuload_t *load);
void fd_cpuload_update(fd_cpuload_t *load);
void fd_cpuload_get_info(fd_cpuload_t *load, float *load_now, float *load_1s, float *load_5s);

#endif /* __FD_CPULOAD_H */
