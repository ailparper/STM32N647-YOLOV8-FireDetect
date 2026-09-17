/**
  ******************************************************************************
  * @file    fd_frame_queue.c
  * @brief   帧缓冲环形队列实现
  ******************************************************************************
  */

#include "fd_frame_queue.h"

void fd_frame_queue_init(fd_frame_queue_t *queue, uint8_t buffer_nb, uint8_t **buffers)
{
    uint8_t i;

    tx_semaphore_create(&queue->free_sem, NULL, buffer_nb);
    tx_semaphore_create(&queue->ready_sem, NULL, 0);

    queue->buffer_nb = buffer_nb;
    for (i = 0; i < buffer_nb; i++)
    {
        queue->buffers[i] = buffers[i];
    }
    queue->free_idx = 0;
    queue->ready_idx = 0;
}

uint8_t *fd_frame_queue_get_free(fd_frame_queue_t *queue, ULONG wait_option)
{
    uint8_t *buffer;

    if (tx_semaphore_get(&queue->free_sem, wait_option) == TX_NO_INSTANCE)
    {
        return NULL;
    }

    buffer = queue->buffers[queue->free_idx];
    queue->free_idx = (queue->free_idx + 1) % queue->buffer_nb;

    return buffer;
}

void fd_frame_queue_put_free(fd_frame_queue_t *queue)
{
    tx_semaphore_put(&queue->free_sem);
}

uint8_t *fd_frame_queue_get_ready(fd_frame_queue_t *queue)
{
    uint8_t *buffer;

    tx_semaphore_get(&queue->ready_sem, TX_WAIT_FOREVER);

    buffer = queue->buffers[queue->ready_idx];
    queue->ready_idx = (queue->ready_idx + 1) % queue->buffer_nb;

    return buffer;
}

void fd_frame_queue_put_ready(fd_frame_queue_t *queue)
{
    tx_semaphore_put(&queue->ready_sem);
}
