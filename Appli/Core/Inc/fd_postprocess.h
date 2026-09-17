/**
  ******************************************************************************
  * @file    fd_postprocess.h
  * @brief   YOLOv8 int8 输出后处理（基于 ST Vision Models PP 中间件）
  ******************************************************************************
  */

#ifndef __FD_POSTPROCESS_H
#define __FD_POSTPROCESS_H

#include "fd_conf.h"
#include "od_pp_output_if.h"
#include "od_yolov8_pp_if.h"

int32_t fd_postprocess_init(yolov8_pp_static_param_t *params);
int32_t fd_postprocess_run(int8_t *raw_output, od_pp_out_t *result,
                           yolov8_pp_static_param_t *params);

#endif /* __FD_POSTPROCESS_H */
