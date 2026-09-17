/**
 ****************************************************************************************************
 * @file        bsp_lcd.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2025-01-13
 * @brief       LCD驱动文件
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

#ifndef __BSP_LCD_H
#define __BSP_LCD_H

#include "stm32n6xx_hal.h"
#include "lcd.h"

#define BSP_LCD_WIDTH   800
#define BSP_LCD_HEIGHT  480

#define BSP_LCD_BL_PIN  GPIO_PIN_3
#define BSP_LCD_BL_GPIO GPIOA

typedef struct
{
    uint32_t x_size;
    uint32_t y_size;
    uint32_t active_layer;
    uint32_t pixel_format;
    uint32_t bpp_factor;
} bsp_lcd_t;

typedef struct
{
    uint32_t x0;
    uint32_t x1;
    uint32_t y0;
    uint32_t y1;
    uint32_t pixel_format;
    uint32_t address;
} bsp_lcd_layer_config_t;

extern bsp_lcd_t bsp_lcd;
extern const LCD_UTILS_Drv_t bsp_lcd_driver;

void bsp_lcd_init(void);
void bsp_lcd_init_ex(uint32_t pixel_format, uint32_t width, uint32_t height);
void bsp_lcd_config_layer(uint32_t layer_index, bsp_lcd_layer_config_t *layer_config);
void bsp_lcd_set_brightness(uint8_t brightness);
LTDC_HandleTypeDef *bsp_lcd_get_ltdc_handle(void);

#endif
