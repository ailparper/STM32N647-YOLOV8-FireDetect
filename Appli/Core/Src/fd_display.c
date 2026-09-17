/**
  ******************************************************************************
  * @file    fd_display.c
  * @brief   LTDC 双层显示管理实现
  ******************************************************************************
  */

#include "fd_display.h"
#include "bsp_lcd.h"
#include "stm32_lcd.h"
#include "stm32_lcd_ex.h"
#include <string.h>

/* 后景层帧缓冲池：置于外部 HyperRAM（.EXTRAM 段） */
static uint8_t back_buffer[FD_DISPLAY_BUFFER_NB][FD_LCD_BG_WIDTH * FD_LCD_BG_HEIGHT * 2]
    __attribute__((aligned(32))) __attribute__((section(".EXTRAM")));
static uint8_t back_disp_idx = 1;               /* 当前显示中的后景缓冲 */
static uint8_t back_fill_idx = 0;               /* DCMIPP 正在写入的缓冲 */

/* 前景层帧缓冲：双缓冲 */
static uint8_t fg_buffer[2][FD_LCD_FG_WIDTH * FD_LCD_FG_HEIGHT * 2]
    __attribute__((aligned(32))) __attribute__((section(".EXTRAM")));
static uint8_t fg_load_idx;                     /* 当前正在被 LTDC 扫描的前景缓冲 */

void fd_display_init(void)
{
    bsp_lcd_layer_config_t layer_config = {0};

    memset(back_buffer, 0, sizeof(back_buffer));
    SCB_CleanInvalidateDCache_by_Addr(back_buffer, sizeof(back_buffer));
    memset(fg_buffer, 0, sizeof(fg_buffer));
    SCB_CleanInvalidateDCache_by_Addr(fg_buffer, sizeof(fg_buffer));

    bsp_lcd_init();
    bsp_lcd_set_brightness(100);

    /* Layer0: 摄像头画面（RGB565），居中窗口 */
    layer_config.x0 = (BSP_LCD_WIDTH - FD_LCD_BG_WIDTH) / 2;
    layer_config.y0 = (BSP_LCD_HEIGHT - FD_LCD_BG_HEIGHT) / 2;
    layer_config.x1 = layer_config.x0 + FD_LCD_BG_WIDTH;
    layer_config.y1 = layer_config.y0 + FD_LCD_BG_HEIGHT;
    layer_config.pixel_format = LCD_PIXEL_FORMAT_RGB565;
    layer_config.address = (uint32_t)back_buffer[back_disp_idx];
    bsp_lcd_config_layer(0, &layer_config);

    /* Layer1: 检测框/HUD 叠加（ARGB4444），居中窗口 */
    layer_config.x0 = (BSP_LCD_WIDTH - FD_LCD_FG_WIDTH) / 2;
    layer_config.y0 = (BSP_LCD_HEIGHT - FD_LCD_FG_HEIGHT) / 2;
    layer_config.x1 = layer_config.x0 + FD_LCD_FG_WIDTH;
    layer_config.y1 = layer_config.y0 + FD_LCD_FG_HEIGHT;
    layer_config.pixel_format = LCD_PIXEL_FORMAT_ARGB4444;
    layer_config.address = (uint32_t)fg_buffer[1];
    bsp_lcd_config_layer(1, &layer_config);

    /* 绑定 UTIL_LCD 绘图库到前景层 */
    UTIL_LCD_SetFuncDriver(&bsp_lcd_driver);
    UTIL_LCD_SetLayer(1);
    UTIL_LCD_Clear(0x00000000);
    UTIL_LCD_SetFont(&Font20);
    UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
}

uint8_t *fd_display_get_back_buffer(void)
{
    return back_buffer[back_fill_idx];
}

void fd_display_flip_back_buffer(void)
{
    back_disp_idx = (back_disp_idx + 1) % FD_DISPLAY_BUFFER_NB;
    back_fill_idx = (back_fill_idx + 1) % FD_DISPLAY_BUFFER_NB;

    HAL_LTDC_SetAddress_NoReload(bsp_lcd_get_ltdc_handle(),
                                 (uint32_t)back_buffer[back_disp_idx], 0);
    HAL_LTDC_ReloadLayer(bsp_lcd_get_ltdc_handle(), LTDC_RELOAD_VERTICAL_BLANKING, 0);
}

void fd_display_overlay_begin(void)
{
    __disable_irq();
    HAL_LTDC_SetAddress_NoReload(bsp_lcd_get_ltdc_handle(),
                                 (uint32_t)fg_buffer[fg_load_idx], 1);
    __enable_irq();
}

void fd_display_overlay_commit(void)
{
    SCB_CleanDCache_by_Addr(fg_buffer[fg_load_idx], sizeof(fg_buffer[fg_load_idx]));

    __disable_irq();
    HAL_LTDC_ReloadLayer(bsp_lcd_get_ltdc_handle(), LTDC_RELOAD_VERTICAL_BLANKING, 1);
    __enable_irq();

    fg_load_idx = 1 - fg_load_idx;
}
