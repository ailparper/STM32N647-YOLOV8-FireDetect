/**
 ****************************************************************************************************
 * @file        bsp_bus.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2025-01-13
 * @brief       BUS驱动文件
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

#ifndef __BSP_BUS_H
#define __BSP_BUS_H

#include "stm32n6xx_hal.h"

int32_t bsp_i2c2_init(void);
int32_t bsp_i2c2_deinit(void);
int32_t bsp_i2c2_write_reg16(uint16_t address, uint16_t reg, uint8_t *data, uint16_t length);
int32_t bsp_i2c2_read_reg16(uint16_t address, uint16_t reg, uint8_t *data, uint16_t length);
int32_t bsp_bus_get_tick(void);

#endif
