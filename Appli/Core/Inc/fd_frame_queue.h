/**
  ******************************************************************************
  * @file    fd_frame_queue.h
  * @brief   帧缓冲环形队列（ThreadX 信号量同步）
  *
  *          生产者（DCMIPP 帧完成中断）通过 get_free/put_ready 将空闲缓冲
  *          投递为就绪；消费者（NN 推理线程）通过 get_ready/put_free 取走
  *          处理完毕后归还。缓冲深度由 FD_FRAME_QUEUE_BUFFERS 决定。
  ******************************************************************************
  */

#ifndef __FD_FRAME_QUEUE_H
#define __FD_FRAME_QUEUE_H

#include "tx_api.h"
#include "fd_conf.h"

typedef struct
{
    TX_SEMAPHORE free_sem;              /* 空闲缓冲计数 */
    TX_SEMAPHORE ready_sem;             /* 就绪缓冲计数 */
    uint8_t buffer_nb;                  /* 缓冲总个数 */
    uint8_t *buffers[FD_FRAME_QUEUE_BUFFERS];
    uint8_t free_idx;                   /* 下一个可分配的空闲缓冲下标 */
    uint8_t ready_idx;                  /* 下一个可取走的就绪缓冲下标 */
} fd_frame_queue_t;

void    fd_frame_queue_init(fd_frame_queue_t *queue, uint8_t buffer_nb, uint8_t **buffers);
uint8_t *fd_frame_queue_get_free(fd_frame_queue_t *queue, ULONG wait_option);
void    fd_frame_queue_put_free(fd_frame_queue_t *queue);
uint8_t *fd_frame_queue_get_ready(fd_frame_queue_t *queue);
void    fd_frame_queue_put_ready(fd_frame_queue_t *queue);

#endif /* __FD_FRAME_QUEUE_H */
