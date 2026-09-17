/**
 ****************************************************************************************************
 * @file        bsp_bus.c
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

#include "bsp_bus.h"
#include "main.h"

extern I2C_HandleTypeDef hi2c2;

static uint8_t bsp_i2c2_init_counter = 0;

int32_t bsp_i2c2_init(void)
{
    if (bsp_i2c2_init_counter == 0)
    {
        bsp_i2c2_init_counter++;

        MX_I2C2_Init();
    }

    return 0;
}

int32_t bsp_i2c2_deinit(void)
{
    if (bsp_i2c2_init_counter == 0)
    {
        return 0;
    }

    bsp_i2c2_init_counter--;
    if (bsp_i2c2_init_counter == 0)
    {
        HAL_I2C_DeInit(&hi2c2);
    }

    return 0;
}

int32_t bsp_i2c2_write_reg16(uint16_t address, uint16_t reg, uint8_t *data, uint16_t length)
{
    if (HAL_I2C_Mem_Write(&hi2c2, address, reg, I2C_MEMADD_SIZE_16BIT, data, length, 1000) != HAL_OK)
    {
        return -1;
    }

    return 0;
}

int32_t bsp_i2c2_read_reg16(uint16_t address, uint16_t reg, uint8_t *data, uint16_t length)
{
    if (HAL_I2C_Mem_Read(&hi2c2, address, reg, I2C_MEMADD_SIZE_16BIT, data, length, 1000) != HAL_OK)
    {
        return -1;
    }

    return 0;
}

int32_t bsp_bus_get_tick(void)
{
    return (int32_t)HAL_GetTick();
}
