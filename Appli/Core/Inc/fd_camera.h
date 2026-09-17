/**
  ******************************************************************************
  * @file    fd_camera.h
  * @brief   相机流水线（CMW 中间件 + DCMIPP 双通道）
  *
  *          PIPE1: 全分辨率裁剪缩放 -> RGB565 -> LCD 后景层
  *          PIPE2: 独立裁剪缩放 -> RGB888 320x320 -> NN 输入
  *          帧同步/帧完成事件经回调函数上抛给应用层。
  ******************************************************************************
  */

#ifndef __FD_CAMERA_H
#define __FD_CAMERA_H

#include "stm32n6xx_hal.h"

void     fd_camera_init(void (*display_vsync_cb)(void),
                        void (*display_frame_cb)(void),
                        void (*nn_vsync_cb)(void),
                        void (*nn_frame_cb)(void));
void     fd_camera_display_pipe_start(uint8_t *dst, uint32_t capture_mode);
void     fd_camera_display_pipe_set_address(uint8_t *dst);
void     fd_camera_nn_pipe_start(uint8_t *dst, uint32_t capture_mode);
void     fd_camera_nn_pipe_set_address(uint8_t *dst);
void     fd_camera_isp_update(void);

#endif /* __FD_CAMERA_H */
