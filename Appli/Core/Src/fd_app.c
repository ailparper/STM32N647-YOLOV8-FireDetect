/**
  ******************************************************************************
  * @file    fd_app.c
  * @brief  应用编排器实现
  *
  *          数据流：
  *            DCMIPP PIPE1 --> 后景层帧缓冲（LCD 直显）
  *            DCMIPP PIPE2 --> NN 输入队列 --> NN 推理 --> NN 输出队列
  *                          --> 后处理 --> 显示信息（加锁交换）
  *                          --> DP 线程绘制检测框/HUD --> 前景层上屏
  *            PIPE1 VSync  --> ISP 线程周期性执行传感器 3A 更新
  ******************************************************************************
  */

#include "fd_app.h"
#include "fd_conf.h"
#include "fd_display.h"
#include "fd_camera.h"
#include "fd_frame_queue.h"
#include "fd_cpuload.h"
#include "fd_postprocess.h"
#include "tx_api.h"
#include "cmw_camera.h"
#include "ll_aton_runtime.h"
#include "stm32_lcd.h"
#include "stm32_lcd_ex.h"

/* DP 线程绘制的检测结果快照 */
typedef struct
{
    int32_t nb_detect;
    od_pp_outBuffer_t detects[FD_PP_MAX_BOXES_LIMIT];
    uint32_t nn_period_ms;
    uint32_t inf_ms;
    uint32_t pp_ms;
    uint32_t disp_ms;
} fd_display_info_t;

typedef struct
{
    TX_SEMAPHORE update;                /* DP 线程唤醒信号 */
    TX_MUTEX lock;                      /* 保护 info 的读写 */
    fd_display_info_t info;
} fd_display_state_t;

static TX_SEMAPHORE isp_semaphore;

static TX_THREAD nn_thread;
static UCHAR nn_thread_stack[4096];
static TX_THREAD pp_thread;
static UCHAR pp_thread_stack[4096];
static TX_THREAD dp_thread;
static UCHAR dp_thread_stack[4096];
static TX_THREAD isp_thread;
static UCHAR isp_thread_stack[4096];

static VOID nn_thread_entry(ULONG id);
static VOID pp_thread_entry(ULONG id);
static VOID dp_thread_entry(ULONG id);
static VOID isp_thread_entry(ULONG id);

static fd_display_state_t display;

/* ST Edge AI 生成的网络实例与接口（Model/network.c） */
LL_ATON_DECLARE_NAMED_NN_INSTANCE_AND_INTERFACE(Default);

/* NN 输入缓冲置于外部 HyperRAM（DCMIPP PIPE2 直写），
   输出缓冲较小，放在内部 AXISRAM */
static uint8_t nn_input_buffers[FD_FRAME_QUEUE_BUFFERS][FD_NN_IN_SIZE]
    __attribute__((aligned(32))) __attribute__((section(".EXTRAM")));
static fd_frame_queue_t nn_input_queue;
static uint8_t nn_output_buffers[FD_FRAME_QUEUE_BUFFERS][FD_NN_OUT_SIZE]
    __attribute__((aligned(32)));
static fd_frame_queue_t nn_output_queue;

static const char *nn_classes_table[FD_NN_CLASSES] = FD_NN_CLASSES_TABLE;

static fd_cpuload_t cpuload;

static void fd_display_pipe_vsync_cb(void);
static void fd_display_pipe_frame_cb(void);
static void fd_nn_pipe_frame_cb(void);
static void fd_display_network_output(fd_display_info_t *display_info);
static void fd_display_detection(od_pp_outBuffer_t *detect);
static uint8_t fd_clamp_point(int32_t *x, int32_t *y);

/**
  * @brief  应用初始化：完成各模块初始化并启动全部处理线程
  * @note   由 main 线程调用，返回后 main 线程可挂起
  */
void fd_app_init(void)
{
    fd_display_init();

    fd_frame_queue_init(&nn_input_queue, FD_FRAME_QUEUE_BUFFERS,
                        (uint8_t *[FD_FRAME_QUEUE_BUFFERS]){nn_input_buffers[0], nn_input_buffers[1]});
    fd_frame_queue_init(&nn_output_queue, FD_FRAME_QUEUE_BUFFERS,
                        (uint8_t *[FD_FRAME_QUEUE_BUFFERS]){nn_output_buffers[0], nn_output_buffers[1]});

    fd_cpuload_init(&cpuload);

    fd_camera_init(fd_display_pipe_vsync_cb, fd_display_pipe_frame_cb,
                   NULL, fd_nn_pipe_frame_cb);

    tx_semaphore_create(&isp_semaphore, NULL, 0);
    tx_semaphore_create(&display.update, NULL, 0);
    tx_mutex_create(&display.lock, NULL, TX_INHERIT);

    /* 后景层先出图，让画面尽快点亮 */
    fd_camera_display_pipe_start(fd_display_get_back_buffer(), CMW_MODE_CONTINUOUS);

    tx_thread_create(&nn_thread, "NN Thread", nn_thread_entry, 0,
                     nn_thread_stack, sizeof(nn_thread_stack),
                     TX_MAX_PRIORITIES - 3, TX_MAX_PRIORITIES - 3, 10, TX_AUTO_START);
    tx_thread_create(&pp_thread, "PP Thread", pp_thread_entry, 0,
                     pp_thread_stack, sizeof(pp_thread_stack),
                     TX_MAX_PRIORITIES - 2, TX_MAX_PRIORITIES - 2, 10, TX_AUTO_START);
    tx_thread_create(&dp_thread, "DP Thread", dp_thread_entry, 0,
                     dp_thread_stack, sizeof(dp_thread_stack),
                     TX_MAX_PRIORITIES - 2, TX_MAX_PRIORITIES - 2, 10, TX_AUTO_START);
    tx_thread_create(&isp_thread, "ISP Thread", isp_thread_entry, 0,
                     isp_thread_stack, sizeof(isp_thread_stack),
                     TX_MAX_PRIORITIES - 4, TX_MAX_PRIORITIES - 4, 10, TX_AUTO_START);
}

/* ==================== 相机事件回调（中断上下文） ==================== */

static void fd_display_pipe_vsync_cb(void)
{
    tx_semaphore_put(&isp_semaphore);
}

static void fd_display_pipe_frame_cb(void)
{
    fd_display_flip_back_buffer();
    fd_camera_display_pipe_set_address(fd_display_get_back_buffer());
}

static void fd_nn_pipe_frame_cb(void)
{
    uint8_t *buffer;

    /* 队列满时丢弃本帧，等待 NN 消费腾出缓冲 */
    buffer = fd_frame_queue_get_free(&nn_input_queue, 0);
    if (buffer != NULL)
    {
        fd_camera_nn_pipe_set_address(buffer);
        fd_frame_queue_put_ready(&nn_input_queue);
    }
}

/* ==================== 处理线程 ==================== */

static VOID nn_thread_entry(ULONG id)
{
    uint32_t nn_in_len;
    uint32_t nn_out_len;
    uint8_t *capture_buffer;
    uint8_t *output_buffer;
    uint32_t nn_period[2];
    uint32_t nn_period_ms;
    uint32_t time_stamp;
    uint32_t inf_ms;

    nn_in_len = LL_Buffer_len(LL_ATON_Input_Buffers_Info_Default());
    nn_out_len = LL_Buffer_len(LL_ATON_Output_Buffers_Info_Default());

    nn_period[1] = HAL_GetTick();

    /* 分配首个输入缓冲并启动 PIPE2 连续采集 */
    capture_buffer = fd_frame_queue_get_free(&nn_input_queue, 0);
    fd_camera_nn_pipe_start(capture_buffer, CMW_MODE_CONTINUOUS);

    while (1)
    {
        nn_period[0] = nn_period[1];
        nn_period[1] = HAL_GetTick();
        nn_period_ms = nn_period[1] - nn_period[0];

        capture_buffer = fd_frame_queue_get_ready(&nn_input_queue);
        output_buffer = fd_frame_queue_get_free(&nn_output_queue, TX_WAIT_FOREVER);

        time_stamp = HAL_GetTick();
        LL_ATON_Set_User_Input_Buffer_Default(0, capture_buffer, nn_in_len);
        SCB_InvalidateDCache_by_Addr((uint32_t *)output_buffer, nn_out_len);
        LL_ATON_Set_User_Output_Buffer_Default(0, output_buffer, nn_out_len);
        LL_ATON_RT_Main(&NN_Instance_Default);
        inf_ms = HAL_GetTick() - time_stamp;

        fd_frame_queue_put_free(&nn_input_queue);
        fd_frame_queue_put_ready(&nn_output_queue);

        tx_mutex_get(&display.lock, TX_WAIT_FOREVER);
        display.info.inf_ms = inf_ms;
        display.info.nn_period_ms = nn_period_ms;
        tx_mutex_put(&display.lock);
    }
}

static VOID pp_thread_entry(ULONG id)
{
    yolov8_pp_static_param_t pp_params;
    uint8_t *output_buffer;
    od_pp_out_t pp_output;
    uint32_t nn_pp[2];
    int32_t i;

    fd_postprocess_init(&pp_params);

    while (1)
    {
        output_buffer = fd_frame_queue_get_ready(&nn_output_queue);
        pp_output.pOutBuff = NULL;

        nn_pp[0] = HAL_GetTick();
        fd_postprocess_run((int8_t *)output_buffer, &pp_output, &pp_params);
        nn_pp[1] = HAL_GetTick();

        /* 快照拷贝：检测结果最多保留 FD_PP_MAX_BOXES_LIMIT 个 */
        tx_mutex_get(&display.lock, TX_WAIT_FOREVER);
        display.info.nb_detect = pp_output.nb_detect;
        if (display.info.nb_detect > FD_PP_MAX_BOXES_LIMIT)
        {
            display.info.nb_detect = FD_PP_MAX_BOXES_LIMIT;
        }
        for (i = 0; i < display.info.nb_detect; i++)
        {
            display.info.detects[i] = pp_output.pOutBuff[i];
        }
        display.info.pp_ms = nn_pp[1] - nn_pp[0];
        tx_mutex_put(&display.lock);

        fd_frame_queue_put_free(&nn_output_queue);
        tx_semaphore_ceiling_put(&display.update, 1);
    }
}

static VOID dp_thread_entry(ULONG id)
{
    uint32_t disp_ms = 0;
    fd_display_info_t display_info;
    uint32_t time_stamp;

    while (1)
    {
        tx_semaphore_get(&display.update, TX_WAIT_FOREVER);

        tx_mutex_get(&display.lock, TX_WAIT_FOREVER);
        display_info = display.info;
        tx_mutex_put(&display.lock);
        display_info.disp_ms = disp_ms;

        time_stamp = HAL_GetTick();
        fd_display_network_output(&display_info);
        disp_ms = HAL_GetTick() - time_stamp;
    }
}

static VOID isp_thread_entry(ULONG id)
{
    while (1)
    {
        tx_semaphore_get(&isp_semaphore, TX_WAIT_FOREVER);
        fd_camera_isp_update();
    }
}

/* ==================== 前景层绘制 ==================== */

/* 坐标钳位到屏幕范围，返回是否发生修正 */
static uint8_t fd_clamp_point(int32_t *x, int32_t *y)
{
    int32_t xi;
    int32_t yi;

    xi = *x;
    yi = *y;

    if (*x < 0)
    {
        *x = 0;
    }
    if (*y < 0)
    {
        *y = 0;
    }
    if (*x >= FD_LCD_BG_WIDTH)
    {
        *x = FD_LCD_BG_WIDTH - 1;
    }
    if (*y >= FD_LCD_BG_HEIGHT)
    {
        *y = FD_LCD_BG_HEIGHT - 1;
    }

    return (xi != *x) || (yi != *y);
}

static void fd_display_detection(od_pp_outBuffer_t *detect)
{
    int32_t xc;
    int32_t yc;
    int32_t x0;
    int32_t y0;
    int32_t x1;
    int32_t y1;
    int32_t w;
    int32_t h;

    /* 归一化中心坐标 -> 屏幕像素坐标 */
    xc = (int32_t)(FD_LCD_BG_WIDTH * detect->x_center);
    yc = (int32_t)(FD_LCD_BG_HEIGHT * detect->y_center);
    w = (int32_t)(FD_LCD_BG_WIDTH * detect->width);
    h = (int32_t)(FD_LCD_BG_HEIGHT * detect->height);

    x0 = xc - (w + 1) / 2;
    y0 = yc - (h + 1) / 2;
    x1 = xc + (w + 1) / 2;
    y1 = yc + (h + 1) / 2;

    fd_clamp_point(&x0, &y0);
    fd_clamp_point(&x1, &y1);

    UTIL_LCD_DrawRect(x0, y0, x1 - x0, y1 - y0, UTIL_LCD_COLOR_GREEN);
    UTIL_LCDEx_PrintfAt(x0, y0, LEFT_MODE, nn_classes_table[detect->class_index]);
}

static void fd_display_network_output(fd_display_info_t *display_info)
{
    float cpuload_one_second;
    uint8_t line_nb = 0;
    int32_t i;

    fd_display_overlay_begin();
    UTIL_LCD_FillRect(0, 0, FD_LCD_FG_WIDTH, FD_LCD_FG_HEIGHT, 0x00000000);

    fd_cpuload_update(&cpuload);
    fd_cpuload_get_info(&cpuload, NULL, &cpuload_one_second, NULL);

    UTIL_LCDEx_PrintfAt(0, LINE(line_nb), RIGHT_MODE, "CPU load");
    line_nb += 1;
    UTIL_LCDEx_PrintfAt(0, LINE(line_nb), RIGHT_MODE, "%.1f%%", cpuload_one_second);
    line_nb += 2;
    UTIL_LCDEx_PrintfAt(0, LINE(line_nb), RIGHT_MODE, "Inference");
    line_nb += 1;
    UTIL_LCDEx_PrintfAt(0, LINE(line_nb), RIGHT_MODE, "%ums", display_info->inf_ms);
    line_nb += 2;
    UTIL_LCDEx_PrintfAt(0, LINE(line_nb), RIGHT_MODE, "FPS");
    line_nb += 1;
    UTIL_LCDEx_PrintfAt(0, LINE(line_nb), RIGHT_MODE, "%.2f",
                        1000.0 / display_info->nn_period_ms);
    line_nb += 2;
    UTIL_LCDEx_PrintfAt(0, LINE(line_nb), RIGHT_MODE, "object");
    line_nb += 1;
    UTIL_LCDEx_PrintfAt(0, LINE(line_nb), RIGHT_MODE, "%u", display_info->nb_detect);

    for (i = 0; i < display_info->nb_detect; i++)
    {
        fd_display_detection(&display_info->detects[i]);
    }

    fd_display_overlay_commit();
}
