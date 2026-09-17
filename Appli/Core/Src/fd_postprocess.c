/**
  ******************************************************************************
  * @file    fd_postprocess.c
  * @brief   YOLOv8 int8 后处理实现（解码 + NMS 由 ST 中间件完成）
  ******************************************************************************
  */

#include "fd_postprocess.h"
#include <assert.h>

/* 检测框输出池（中间件最多写入 FD_PP_TOTAL_BOXES 个候选） */
static od_pp_outBuffer_t out_detections[FD_PP_TOTAL_BOXES];

int32_t fd_postprocess_init(yolov8_pp_static_param_t *params)
{
    params->nb_classes = FD_PP_NB_CLASSES;
    params->nb_total_boxes = FD_PP_TOTAL_BOXES;
    params->max_boxes_limit = FD_PP_MAX_BOXES_LIMIT;
    params->conf_threshold = FD_PP_CONF_THRESHOLD;
    params->iou_threshold = FD_PP_IOU_THRESHOLD;
    /* int8 量化模型：需要反量化 scale / zero_point */
    params->raw_output_scale = FD_PP_SCALE;
    params->raw_output_zero_point = FD_PP_ZERO_POINT;

    return od_yolov8_pp_reset(params);
}

int32_t fd_postprocess_run(int8_t *raw_output, od_pp_out_t *result,
                           yolov8_pp_static_param_t *params)
{
    yolov8_pp_in_centroid_int8_t pp_input = {
        .pRaw_detections = raw_output
    };

    assert(raw_output != NULL);
    result->pOutBuff = out_detections;

    return od_yolov8_pp_process_int8(&pp_input, result, params);
}
