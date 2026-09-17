/**
 ****************************************************************************************************
 * @file        stm32n6xx_hal_timebase_threadx.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2025-01-13
 * @brief       stm32n6xx_hal_timebase_threadx.c文件
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 * 
 * 实验平台:正点原子 N647开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 * 
 ****************************************************************************************************
 */

#include "stm32n6xx_hal.h"
#include "tx_api.h"

HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
    UNUSED(TickPriority);

    return 0;
}

uint32_t HAL_GetTick(void)
{
    return tx_time_get();
}

void HAL_Delay(uint32_t Delay)
{
    tx_thread_sleep(Delay);
}
