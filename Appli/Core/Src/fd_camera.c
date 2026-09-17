/**
  ******************************************************************************
  * @file    fd_camera.c
  * @brief   相机流水线实现（IMX335 + DCMIPP 双通道）
  ******************************************************************************
  */

#include "fd_camera.h"
#include "fd_conf.h"
#include "cmw_camera.h"

static void fd_camera_display_pipe_init(uint32_t sensor_width, uint32_t sensor_height);
static void fd_camera_nn_pipe_init(uint32_t sensor_width, uint32_t sensor_height);
static void fd_camera_compute_roi(CMW_Manual_roi_area_t *roi,
                                  uint32_t sensor_width, uint32_t sensor_height);

/* 应用层注册的回调（NULL 表示不关心该事件） */
static void (*display_vsync_user_cb)(void);
static void (*display_frame_user_cb)(void);
static void (*nn_vsync_user_cb)(void);
static void (*nn_frame_user_cb)(void);

void fd_camera_init(void (*display_vsync_cb)(void),
                    void (*display_frame_cb)(void),
                    void (*nn_vsync_cb)(void),
                    void (*nn_frame_cb)(void))
{
    CMW_CameraInit_t camera_init = {0};

    /* 宽高/帧率/像素格式填 0，由 CMW 使用传感器默认配置 */
    camera_init.width = 0;
    camera_init.height = 0;
    camera_init.fps = 0;
    camera_init.pixel_format = 0;
    camera_init.anti_flicker = 0;
    camera_init.mirror_flip = FD_CAMERA_MIRROR_FLIP;
    CMW_CAMERA_Init(&camera_init);

    fd_camera_display_pipe_init(camera_init.width, camera_init.height);
    fd_camera_nn_pipe_init(camera_init.width, camera_init.height);

    display_vsync_user_cb = display_vsync_cb;
    display_frame_user_cb = display_frame_cb;
    nn_vsync_user_cb = nn_vsync_cb;
    nn_frame_user_cb = nn_frame_cb;
}

void fd_camera_display_pipe_start(uint8_t *dst, uint32_t capture_mode)
{
    CMW_CAMERA_Start(DCMIPP_PIPE1, dst, capture_mode);
}

void fd_camera_display_pipe_set_address(uint8_t *dst)
{
    HAL_DCMIPP_PIPE_SetMemoryAddress(CMW_CAMERA_GetDCMIPPHandle(),
                                     DCMIPP_PIPE1, DCMIPP_MEMORY_ADDRESS_0, (uint32_t)dst);
}

void fd_camera_nn_pipe_start(uint8_t *dst, uint32_t capture_mode)
{
    CMW_CAMERA_Start(DCMIPP_PIPE2, dst, capture_mode);
}

void fd_camera_nn_pipe_set_address(uint8_t *dst)
{
    HAL_DCMIPP_PIPE_SetMemoryAddress(CMW_CAMERA_GetDCMIPPHandle(),
                                     DCMIPP_PIPE2, DCMIPP_MEMORY_ADDRESS_0, (uint32_t)dst);
}

void fd_camera_isp_update(void)
{
    CMW_CAMERA_Run();
}

/* PIPE1: 输出 LCD 后景层分辨率 RGB565 */
static void fd_camera_display_pipe_init(uint32_t sensor_width, uint32_t sensor_height)
{
    CMW_DCMIPP_Conf_t dcmipp_conf = {0};
    uint32_t hw_pitch;

    dcmipp_conf.output_width = FD_LCD_BG_WIDTH;
    dcmipp_conf.output_height = FD_LCD_BG_HEIGHT;
    dcmipp_conf.output_format = DCMIPP_PIXEL_PACKER_FORMAT_RGB565_1;
    dcmipp_conf.output_bpp = 2;
    dcmipp_conf.enable_swap = 1;
    dcmipp_conf.enable_gamma_conversion = 0;
    dcmipp_conf.mode = CMW_Aspect_ratio_manual_roi;
    fd_camera_compute_roi(&dcmipp_conf.manual_conf, sensor_width, sensor_height);
    CMW_CAMERA_SetPipeConfig(DCMIPP_PIPE1, &dcmipp_conf, &hw_pitch);
}

/* PIPE2: 输出 NN 输入分辨率 RGB888 */
static void fd_camera_nn_pipe_init(uint32_t sensor_width, uint32_t sensor_height)
{
    CMW_DCMIPP_Conf_t dcmipp_conf = {0};
    uint32_t hw_pitch;

    dcmipp_conf.output_width = FD_NN_WIDTH;
    dcmipp_conf.output_height = FD_NN_HEIGHT;
    dcmipp_conf.output_format = FD_NN_FORMAT;
    dcmipp_conf.output_bpp = FD_NN_BPP;
    dcmipp_conf.enable_swap = 1;
    dcmipp_conf.enable_gamma_conversion = 0;
    dcmipp_conf.mode = CMW_Aspect_ratio_manual_roi;
    fd_camera_compute_roi(&dcmipp_conf.manual_conf, sensor_width, sensor_height);
    CMW_CAMERA_SetPipeConfig(DCMIPP_PIPE2, &dcmipp_conf, &hw_pitch);
}

/* 手动 ROI：以显示窗口宽高比在传感器画面中央裁剪最大区域（等价于 ST 例程的
   Aspect_ratio_manual_roi 模式），两个 PIPE 使用同一 ROI，保证画框坐标一致 */
static void fd_camera_compute_roi(CMW_Manual_roi_area_t *roi,
                                  uint32_t sensor_width, uint32_t sensor_height)
{
    float ratio_x;
    float ratio_y;
    float ratio;

    ratio_x = (float)sensor_width / FD_LCD_BG_WIDTH;
    ratio_y = (float)sensor_height / FD_LCD_BG_HEIGHT;
    ratio = (ratio_x < ratio_y) ? ratio_x : ratio_y;

    roi->width = (uint32_t)((FD_LCD_BG_WIDTH * ratio < sensor_width) ?
                            (FD_LCD_BG_WIDTH * ratio) : sensor_width);
    roi->height = (uint32_t)((FD_LCD_BG_HEIGHT * ratio < sensor_height) ?
                             (FD_LCD_BG_HEIGHT * ratio) : sensor_height);
    roi->offset_x = (sensor_width - roi->width + 1) / 2;
    roi->offset_y = (sensor_height - roi->height + 1) / 2;
}

/* 以下两个回调名与 DCMIPP 时钟配置函数名是 CMW 中间件/ST 例程的约定接口，不可改名 */

HAL_StatusTypeDef MX_DCMIPP_ClockConfig(DCMIPP_HandleTypeDef *hdcmipp)
{
    RCC_PeriphCLKInitTypeDef periph_clk = {0};

    periph_clk.PeriphClockSelection = RCC_PERIPHCLK_DCMIPP | RCC_PERIPHCLK_CSI;
    periph_clk.DcmippClockSelection = RCC_DCMIPPCLKSOURCE_IC17;
    periph_clk.ICSelection[RCC_IC17].ClockSelection = RCC_ICCLKSOURCE_PLL2;
    periph_clk.ICSelection[RCC_IC17].ClockDivider = 3;
    periph_clk.ICSelection[RCC_IC18].ClockSelection = RCC_ICCLKSOURCE_PLL1;
    periph_clk.ICSelection[RCC_IC18].ClockDivider = 40;

    return HAL_RCCEx_PeriphCLKConfig(&periph_clk);
}

int CMW_CAMERA_PIPE_VsyncEventCallback(uint32_t pipe)
{
    if (pipe == DCMIPP_PIPE1)
    {
        if (display_vsync_user_cb != NULL)
        {
            display_vsync_user_cb();
        }
    }
    else if (pipe == DCMIPP_PIPE2)
    {
        if (nn_vsync_user_cb != NULL)
        {
            nn_vsync_user_cb();
        }
    }

    return 0;
}

int CMW_CAMERA_PIPE_FrameEventCallback(uint32_t pipe)
{
    if (pipe == DCMIPP_PIPE1)
    {
        if (display_frame_user_cb != NULL)
        {
            display_frame_user_cb();
        }
    }
    else if (pipe == DCMIPP_PIPE2)
    {
        if (nn_frame_user_cb != NULL)
        {
            nn_frame_user_cb();
        }
    }

    return 0;
}
