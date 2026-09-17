/**
  ******************************************************************************
  * @file    fd_display.h
  * @brief   LTDC 双层显示管理
  *
  *          后景层（Layer0, RGB565）由 DCMIPP PIPE1 直写，采用多缓冲切换；
  *          前景层（Layer1, ARGB4444）用于叠加检测框与 HUD 文字，
  *          双缓冲避免撕裂。
  ******************************************************************************
  */

#ifndef __FD_DISPLAY_H
#define __FD_DISPLAY_H

#include "stm32n6xx_hal.h"
#include "fd_conf.h"

void     fd_display_init(void);
uint8_t *fd_display_get_back_buffer(void);      /* 供 DCMIPP 直写的待填充缓冲 */
void     fd_display_flip_back_buffer(void);     /* 帧完成后切换为显示缓冲 */
void     fd_display_overlay_begin(void);        /* 前景绘制开始：选定绘图缓冲 */
void     fd_display_overlay_commit(void);       /* 前景绘制结束：清缓存并垂直消隐切换 */

#endif /* __FD_DISPLAY_H */
