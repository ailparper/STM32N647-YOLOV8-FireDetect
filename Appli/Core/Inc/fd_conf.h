/**
  ******************************************************************************
  * @file    fd_conf.h
  * @brief   全局配置
  *
  *          集中管理显示、相机、神经网络与后处理的全部参数，
  *          更换模型或屏幕时只需修改此文件。
  ******************************************************************************
  */

#ifndef __FD_CONF_H
#define __FD_CONF_H

#include "stm32n6xx_hal.h"
#include "od_yolov8_pp_if.h"

/* ==================== 显示配置（RGB565 后景层 + ARGB4444 前景层） ==================== */
#define FD_LCD_BG_WIDTH                     800
#define FD_LCD_BG_HEIGHT                    480
#define FD_LCD_FG_WIDTH                     FD_LCD_BG_WIDTH
#define FD_LCD_FG_HEIGHT                    FD_LCD_BG_HEIGHT

/* 后景帧缓冲个数：1 帧 DCMIPP 直写 + 1 帧显示 + 1 帧裕量 */
#define FD_DISPLAY_DELAY                    1
#define FD_DISPLAY_BUFFER_NB                (FD_DISPLAY_DELAY + 2)

/* ==================== 相机配置 ==================== */
/* 传感器画面镜像（CMW_MIRRORFLIP_MIRROR / CMW_MIRRORFLIP_NONE 等） */
#define FD_CAMERA_MIRROR_FLIP               CMW_MIRRORFLIP_MIRROR

/* ==================== 神经网络配置（YOLOv8 火焰/烟雾检测） ==================== */
#define FD_NN_WIDTH                         640
#define FD_NN_HEIGHT                        640
#define FD_NN_BPP                           3
/* DCMIPP PIPE2 输出格式：RGB888（同时兼容 YUV444 打包） */
#define FD_NN_FORMAT                        DCMIPP_PIXEL_PACKER_FORMAT_RGB888_YUV444_1
#define FD_NN_IN_SIZE                       (FD_NN_WIDTH * FD_NN_HEIGHT * FD_NN_BPP)

/* 网络输出 int8 数据长度：(4 box + 2 class) * 8400 anchor = 50400
   与 Model/network.c 中 Transpose 输出 buffer 大小一致 */
#define FD_NN_OUT_SIZE                      50400

/* ==================== 后处理配置（ST Vision Models PP, YOLOv8 int8） ==================== */
#define FD_PP_TYPE                          POSTPROCESS_OD_YOLO_V8_UI

#define FD_NN_CLASSES                       2
#define FD_NN_CLASSES_TABLE                 {"fire", "smoke"}

#define FD_PP_NB_CLASSES                    FD_NN_CLASSES
#define FD_PP_TOTAL_BOXES                   8400
#define FD_PP_MAX_BOXES_LIMIT               50
#define FD_PP_CONF_THRESHOLD                0.15f
#define FD_PP_IOU_THRESHOLD                 0.45f
/* 模型输出的 int8 反量化参数（由 ST Edge AI 校准生成，更换模型后需同步修改） */
#define FD_PP_SCALE                         0.00417345f
#define FD_PP_ZERO_POINT                    (-125)

/* ==================== 缓冲队列与性能统计 ==================== */
#define FD_FRAME_QUEUE_BUFFERS              2
#define FD_CPU_LOAD_HISTORY_DEPTH           8

#endif /* __FD_CONF_H */
