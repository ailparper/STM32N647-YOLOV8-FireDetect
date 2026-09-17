/**
  ******************************************************************************
  * @file    fd_cpuload.c
  * @brief   CPU 占用率统计实现
  ******************************************************************************
  */

#include "fd_cpuload.h"
#include "tx_api.h"
#include <string.h>

void fd_cpuload_init(fd_cpuload_t *load)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

    memset(load, 0, sizeof(fd_cpuload_t));
}

void fd_cpuload_update(fd_cpuload_t *load)
{
    EXECUTION_TIME thread_total;
    EXECUTION_TIME isr;
    EXECUTION_TIME idle;
    uint8_t i;

    load->history[1] = load->history[0];

    _tx_execution_thread_total_time_get(&thread_total);
    _tx_execution_isr_time_get(&isr);
    _tx_execution_idle_time_get(&idle);

    load->history[0].total = thread_total + isr + idle;
    load->history[0].thread = thread_total;
    load->history[0].tick = HAL_GetTick();

    /* 两次有效采样至少间隔 1 秒 */
    if ((load->history[1].tick - load->history[2].tick) < 1000)
    {
        return;
    }

    for (i = 0; i < FD_CPU_LOAD_HISTORY_DEPTH - 2; i++)
    {
        load->history[FD_CPU_LOAD_HISTORY_DEPTH - 1 - i] =
            load->history[FD_CPU_LOAD_HISTORY_DEPTH - 1 - i - 1];
    }
}

void fd_cpuload_get_info(fd_cpuload_t *load, float *load_now, float *load_1s, float *load_5s)
{
    if (load_now != NULL)
    {
        *load_now = 100.0f *
            (float)(load->history[0].thread - load->history[1].thread) /
            (float)(load->history[0].total - load->history[1].total);
    }

    if (load_1s != NULL)
    {
        *load_1s = 100.0f *
            (float)(load->history[2].thread - load->history[3].thread) /
            (float)(load->history[2].total - load->history[3].total);
    }

    if (load_5s != NULL)
    {
        *load_5s = 100.0f *
            (float)(load->history[2].thread - load->history[7].thread) /
            (float)(load->history[2].total - load->history[7].total);
    }
}
