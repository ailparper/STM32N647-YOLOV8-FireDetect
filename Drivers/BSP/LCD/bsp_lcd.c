/**
 ****************************************************************************************************
 * @file        bsp_lcd.c
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

#include "bsp_lcd.h"
#include "main.h"

extern DMA2D_HandleTypeDef hdma2d;
extern LTDC_HandleTypeDef hltdc;

#define CONVERTRGB5652ARGB8888(color)   ((((((((color) >> (11)) & 0x1F) * 527) + 23) >> (6)) << (16)) | (((((((color) >> (5)) & 0x3F) * 259) + 33) >> (6)) << (8)) | (((((color) & 0x1F) * 527) + 23) >> (6)) | (0xFF000000))
#define CONVERTARGB44442ARGB8888(color) ((((((color) >> 12) & 0xF) * 17) << 24) | (((((color) >>  8) & 0xF) * 17) << 16) | (((((color) >>  4) & 0xF) * 17) << 8) | (((((color) >>  0) & 0xF) * 17) << 0))
#define CONVERTRGB8882ARGB8888(color)   ((color) | 0xFF000000)
#define CONVERTARGB88882RGB888(color)   ((color) & 0x00FFFFFF)

static void bsp_lcd_convert_line_to_rgb(uint32_t *source, uint32_t *destination, uint32_t x_size, uint32_t color_mode);
static void bsp_lcd_fill_buffer(uint32_t *destination, uint32_t x_size, uint32_t y_size, uint32_t offset, uint32_t color);

bsp_lcd_t bsp_lcd;

void bsp_lcd_init(void)
{
    bsp_lcd_init_ex(LCD_PIXEL_FORMAT_RGB565, BSP_LCD_WIDTH, BSP_LCD_HEIGHT);
}

void bsp_lcd_init_ex(uint32_t pixel_format, uint32_t width, uint32_t height)
{
    if (pixel_format == LCD_PIXEL_FORMAT_RGB565)
    {
        bsp_lcd.bpp_factor = 2;
    }
    else if (pixel_format == LCD_PIXEL_FORMAT_RGB888)
    {
        bsp_lcd.bpp_factor = 3;
    }
    else if (pixel_format == LCD_PIXEL_FORMAT_ARGB4444)
    {
        bsp_lcd.bpp_factor = 2;
    }
    else
    {
        bsp_lcd.bpp_factor = 4;
    }
    bsp_lcd.pixel_format = pixel_format;
    bsp_lcd.x_size = width;
    bsp_lcd.y_size = height;

    MX_DMA2D_Init();
    MX_LTDC_Init();
}

void bsp_lcd_config_layer(uint32_t layer_index, bsp_lcd_layer_config_t *layer_config)
{
    uint32_t layer_id;
    LTDC_LayerCfgTypeDef ltdc_layer_cfg = {
        .WindowX0 = layer_config->x0,
        .WindowX1 = layer_config->x1,
        .WindowY0 = layer_config->y0,
        .WindowY1 = layer_config->y1,
        .PixelFormat = 0,
        .Alpha = LTDC_LxCACR_CONSTA,
        .Alpha0 = 0,
        .BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA,
        .BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA,
        .FBStartAdress = layer_config->address,
        .ImageWidth = layer_config->x1 - layer_config->x0,
        .ImageHeight = layer_config->y1 - layer_config->y0,
        .Backcolor.Blue = 0,
        .Backcolor.Green = 0,
        .Backcolor.Red = 0,
    };

    if (layer_config->pixel_format == LCD_PIXEL_FORMAT_RGB565)
    {
        ltdc_layer_cfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;
    }
    else if (layer_config->pixel_format == LCD_PIXEL_FORMAT_ARGB4444)
    {
        ltdc_layer_cfg.PixelFormat = LTDC_PIXEL_FORMAT_ARGB4444;
    }
    else if (layer_config->pixel_format == LCD_PIXEL_FORMAT_ARGB8888)
    {
        ltdc_layer_cfg.PixelFormat = LTDC_PIXEL_FORMAT_ARGB8888;
    }
    else
    {
        ltdc_layer_cfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB888;
    }

    if (layer_index == 1)
    {
        layer_id = LTDC_LAYER_2;
    }
    else
    {
        layer_id = LTDC_LAYER_1;
    }

    HAL_LTDC_ConfigLayer(&hltdc, &ltdc_layer_cfg, layer_id);
}

void bsp_lcd_set_brightness(uint8_t brightness)
{
    if (brightness != 0)
    {
        HAL_GPIO_WritePin(BSP_LCD_BL_GPIO, BSP_LCD_BL_PIN, GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_WritePin(BSP_LCD_BL_GPIO, BSP_LCD_BL_PIN, GPIO_PIN_RESET);
    }
}

LTDC_HandleTypeDef *bsp_lcd_get_ltdc_handle(void)
{
    return &hltdc;
}

static int32_t bsp_lcd_draw_bitmap(uint32_t instance, uint32_t x_pos, uint32_t y_pos, uint8_t *bitmap)
{
    uint32_t index;
    uint32_t width;
    uint32_t height;
    uint32_t bit_pixel;
    uint32_t address;
    uint32_t input_color_mode;

    index = (uint32_t)bitmap[10] + ((uint32_t)bitmap[11] << 8) + ((uint32_t)bitmap[12] << 16) + ((uint32_t)bitmap[13] << 24);
    width = (uint32_t)bitmap[18] + ((uint32_t)bitmap[19] << 8) + ((uint32_t)bitmap[20] << 16) + ((uint32_t)bitmap[21] << 24);
    height = (uint32_t)bitmap[22] + ((uint32_t)bitmap[23] << 8) + ((uint32_t)bitmap[24] << 16) + ((uint32_t)bitmap[25] << 24);
    bit_pixel = (uint32_t)bitmap[28] + ((uint32_t)bitmap[29] << 8);
    address = hltdc.LayerCfg[bsp_lcd.active_layer].FBStartAdress + (bsp_lcd.bpp_factor * ((bsp_lcd.x_size * y_pos) + x_pos));

    if ((bit_pixel / 8) == 4)
    {
        input_color_mode = DMA2D_INPUT_ARGB8888;
    }
    else if ((bit_pixel / 8) == 2)
    {
        input_color_mode = DMA2D_INPUT_RGB565;
    }
    else
    {
        input_color_mode = DMA2D_INPUT_RGB888;
    }

    bitmap += index + (width * (height - 1) * (bit_pixel / 8));
    for(index=0; index < height; index++)
    {
        bsp_lcd_convert_line_to_rgb((uint32_t *)bitmap, (uint32_t *)address, width, input_color_mode);
        address += (bsp_lcd.bpp_factor * bsp_lcd.x_size);
        bitmap -= width * (bit_pixel / 8);
    }

    return 0;
}

static int32_t bsp_lcd_fill_rgb_rect(uint32_t instance, uint32_t x_pos, uint32_t y_pos, uint8_t *data, uint32_t width, uint32_t height)
{
    uint32_t i;
    uint32_t address;
    uint32_t input_color_mode;

    if (bsp_lcd.pixel_format == LCD_PIXEL_FORMAT_RGB565)
    {
        input_color_mode = DMA2D_INPUT_RGB565;
    }
    else if (bsp_lcd.pixel_format == LCD_PIXEL_FORMAT_ARGB4444)
    {
        input_color_mode = DMA2D_INPUT_ARGB4444;
    }
    else if (bsp_lcd.pixel_format == LCD_PIXEL_FORMAT_RGB888)
    {
        input_color_mode = DMA2D_INPUT_RGB888;
    }
    else
    {
        input_color_mode = DMA2D_INPUT_ARGB8888;
    }

    for (i = 0; i < height; i++)
    {
        address = hltdc.LayerCfg[bsp_lcd.active_layer].FBStartAdress + (bsp_lcd.bpp_factor * ((bsp_lcd.x_size * (y_pos + i)) + x_pos));
        SCB_CleanDCache_by_Addr((uint32_t *)data, bsp_lcd.bpp_factor * bsp_lcd.x_size);
        bsp_lcd_convert_line_to_rgb((uint32_t *)data, (uint32_t *)address, width, input_color_mode);
        data += bsp_lcd.bpp_factor * width;
    }

    return 0;
}

static int32_t bsp_lcd_draw_h_line(uint32_t instance, uint32_t x_pos, uint32_t y_pos, uint32_t length, uint32_t color)
{
    uint32_t address;

    address = hltdc.LayerCfg[bsp_lcd.active_layer].FBStartAdress + (bsp_lcd.bpp_factor * ((bsp_lcd.x_size * y_pos) + x_pos));
    bsp_lcd_fill_buffer((uint32_t *)address, length, 1, 0, color);

    return 0;
}

static int32_t bsp_lcd_draw_v_line(uint32_t instance, uint32_t x_pos, uint32_t y_pos, uint32_t length, uint32_t color)
{
    uint32_t address;

    address = hltdc.LayerCfg[bsp_lcd.active_layer].FBStartAdress + (bsp_lcd.bpp_factor * ((bsp_lcd.x_size * y_pos) + x_pos));
    bsp_lcd_fill_buffer((uint32_t *)address, 1, length, bsp_lcd.x_size - 1, color);

    return 0;
}

static int32_t bsp_lcd_fill_rect(uint32_t instance, uint32_t x_pos, uint32_t y_pos, uint32_t width, uint32_t height, uint32_t color)
{
    uint32_t address;

    address = hltdc.LayerCfg[bsp_lcd.active_layer].FBStartAdress + (bsp_lcd.bpp_factor * ((bsp_lcd.x_size * y_pos) + x_pos));
    bsp_lcd_fill_buffer((uint32_t *)address, width, height, bsp_lcd.x_size - width, color);

    return 0;
}

static int32_t bsp_lcd_get_pixel(uint32_t instance, uint32_t x_pos, uint32_t y_pos, uint32_t *color)
{
    if (hltdc.LayerCfg[bsp_lcd.active_layer].PixelFormat == LTDC_PIXEL_FORMAT_ARGB8888)
    {
        *color = *(__IO uint32_t *)(hltdc.LayerCfg[bsp_lcd.active_layer].FBStartAdress + (4 * ((bsp_lcd.x_size * y_pos) + x_pos)));
    }
    else if (hltdc.LayerCfg[bsp_lcd.active_layer].PixelFormat == LTDC_PIXEL_FORMAT_RGB888)
    {
        *color = *(__IO uint32_t *)(hltdc.LayerCfg[bsp_lcd.active_layer].FBStartAdress + (3 * ((bsp_lcd.x_size * y_pos) + x_pos)));
        *color = CONVERTARGB88882RGB888(*color);
    }
    else
    {
        *color = *(__IO uint16_t *)(hltdc.LayerCfg[bsp_lcd.active_layer].FBStartAdress + (2 * ((bsp_lcd.x_size * y_pos) + x_pos)));
    }

    return 0;
}

static int32_t bsp_lcd_set_pixel(uint32_t instance, uint32_t x_pos, uint32_t y_pos, uint32_t color)
{
    if (hltdc.LayerCfg[bsp_lcd.active_layer].PixelFormat == LTDC_PIXEL_FORMAT_ARGB8888)
    {
        *(__IO uint32_t *)(hltdc.LayerCfg[bsp_lcd.active_layer].FBStartAdress + (4 * ((bsp_lcd.x_size * y_pos) + x_pos))) = color;
    }
    else if (hltdc.LayerCfg[bsp_lcd.active_layer].PixelFormat == LTDC_PIXEL_FORMAT_RGB888)
    {
        *(__IO uint8_t *)(hltdc.LayerCfg[bsp_lcd.active_layer].FBStartAdress + ((3 * ((bsp_lcd.x_size * y_pos) + x_pos)) - 3)) = (uint8_t)color;
        *(__IO uint8_t *)(hltdc.LayerCfg[bsp_lcd.active_layer].FBStartAdress + ((3 * ((bsp_lcd.x_size * y_pos) + x_pos)) - 2)) = (uint8_t)(color >> 8);
        *(__IO uint8_t *)(hltdc.LayerCfg[bsp_lcd.active_layer].FBStartAdress + ((3 * ((bsp_lcd.x_size * y_pos) + x_pos)) - 1)) = (uint8_t)(color >> 16);
    }
    else
    {
        *(__IO uint16_t *)(hltdc.LayerCfg[bsp_lcd.active_layer].FBStartAdress + (2 * ((bsp_lcd.x_size * y_pos) + x_pos))) = (uint16_t)color;
    }

    return 0;
}

static int32_t bsp_lcd_get_x_size(uint32_t instance, uint32_t *x_size)
{
    *x_size = bsp_lcd.x_size;

    return 0;
}

static int32_t bsp_lcd_get_y_size(uint32_t instance, uint32_t *y_size)
{
    *y_size = bsp_lcd.y_size;

    return 0;
}

static int32_t bsp_lcd_set_layer(uint32_t instance, uint32_t layer_index)
{
    LTDC_LayerCfgTypeDef *layer_cfg;

    layer_cfg = &hltdc.LayerCfg[layer_index];

    bsp_lcd.active_layer = layer_index;
    bsp_lcd.x_size = layer_cfg->ImageWidth;
    bsp_lcd.y_size = layer_cfg->ImageHeight;
    if (layer_cfg->PixelFormat == LTDC_PIXEL_FORMAT_RGB565)
    {
        bsp_lcd.bpp_factor = 2;
        bsp_lcd.pixel_format = LCD_PIXEL_FORMAT_RGB565;
    }
    else if (layer_cfg->PixelFormat == LTDC_PIXEL_FORMAT_ARGB4444)
    {
        bsp_lcd.bpp_factor = 2;
        bsp_lcd.pixel_format = LCD_PIXEL_FORMAT_ARGB4444;
    }
    else if (layer_cfg->PixelFormat == LTDC_PIXEL_FORMAT_RGB888)
    {
        bsp_lcd.bpp_factor = 3;
        bsp_lcd.pixel_format = LCD_PIXEL_FORMAT_RGB888;
    }
    else
    {
        bsp_lcd.bpp_factor = 4;
        bsp_lcd.pixel_format = LCD_PIXEL_FORMAT_ARGB8888;
    }

    return 0;
}

static int32_t bsp_lcd_get_format(uint32_t instance, uint32_t *pixel_format)
{
    *pixel_format =  bsp_lcd.pixel_format;

    return 0;
}

const LCD_UTILS_Drv_t bsp_lcd_driver = {
    bsp_lcd_draw_bitmap,
    bsp_lcd_fill_rgb_rect,
    bsp_lcd_draw_h_line,
    bsp_lcd_draw_v_line,
    bsp_lcd_fill_rect,
    bsp_lcd_get_pixel,
    bsp_lcd_set_pixel,
    bsp_lcd_get_x_size,
    bsp_lcd_get_y_size,
    bsp_lcd_set_layer,
    bsp_lcd_get_format,
};

static void bsp_lcd_convert_line_to_rgb(uint32_t *source, uint32_t *destination, uint32_t x_size, uint32_t color_mode)
{
    uint32_t output_color_mode;

    if (bsp_lcd.pixel_format == LCD_PIXEL_FORMAT_RGB565)
    {
        output_color_mode = DMA2D_OUTPUT_RGB565;
    }
    else if (bsp_lcd.pixel_format == LCD_PIXEL_FORMAT_ARGB4444)
    {
        output_color_mode = DMA2D_OUTPUT_ARGB4444;
    }
    else if (bsp_lcd.pixel_format == LCD_PIXEL_FORMAT_RGB888)
    {
        output_color_mode = DMA2D_OUTPUT_RGB888;
    }
    else
    {
        output_color_mode = DMA2D_OUTPUT_ARGB8888;
    }

    hdma2d.Instance = DMA2D;
    hdma2d.Init.Mode = DMA2D_M2M_PFC;
    hdma2d.Init.ColorMode = output_color_mode;
    hdma2d.Init.OutputOffset = 0;
    hdma2d.LayerCfg[DMA2D_FOREGROUND_LAYER].AlphaMode = DMA2D_NO_MODIF_ALPHA;
    hdma2d.LayerCfg[DMA2D_FOREGROUND_LAYER].InputAlpha = 0xFF;
    hdma2d.LayerCfg[DMA2D_FOREGROUND_LAYER].InputColorMode = color_mode;
    hdma2d.LayerCfg[DMA2D_FOREGROUND_LAYER].InputOffset = 0;
    if (HAL_DMA2D_Init(&hdma2d) == HAL_OK)
    {
        if (HAL_DMA2D_ConfigLayer(&hdma2d, DMA2D_FOREGROUND_LAYER) == HAL_OK)
        {
            if (HAL_DMA2D_Start(&hdma2d, (uint32_t)source, (uint32_t)destination, x_size, DMA2D_FOREGROUND_LAYER) == HAL_OK)
            {
                HAL_DMA2D_PollForTransfer(&hdma2d, 50);
            }
        }
    }
}

static void bsp_lcd_fill_buffer(uint32_t *destination, uint32_t x_size, uint32_t y_size, uint32_t offset, uint32_t color)
{
    uint32_t output_color_mode;
    uint32_t input_color;

    if (bsp_lcd.pixel_format == LCD_PIXEL_FORMAT_RGB565)
    {
        output_color_mode = DMA2D_OUTPUT_RGB565;
        input_color = CONVERTRGB5652ARGB8888(color);
    }
    else if (bsp_lcd.pixel_format == LCD_PIXEL_FORMAT_ARGB4444)
    {
        output_color_mode = DMA2D_OUTPUT_ARGB4444;
        input_color = CONVERTARGB44442ARGB8888(color);
    }
    else if (bsp_lcd.pixel_format == LCD_PIXEL_FORMAT_RGB888)
    {
        output_color_mode = DMA2D_OUTPUT_RGB888;
        input_color = CONVERTRGB8882ARGB8888(color);
    }
    else
    {
        output_color_mode = DMA2D_OUTPUT_ARGB8888;
        input_color = color;
    }

    hdma2d.Instance = DMA2D;
    hdma2d.Init.Mode = DMA2D_R2M;
    hdma2d.Init.ColorMode = output_color_mode;
    hdma2d.Init.OutputOffset = offset;
    if (HAL_DMA2D_Init(&hdma2d) == HAL_OK)
    {
        if (HAL_DMA2D_Start(&hdma2d, input_color, (uint32_t)destination, x_size, y_size) == HAL_OK)
        {
            HAL_DMA2D_PollForTransfer(&hdma2d, 50);
        }
    }
}
