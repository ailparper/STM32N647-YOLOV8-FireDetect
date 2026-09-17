/**
  ******************************************************************************
  * @file    fd_console.c
  * @brief   调试串口控制台实现
  ******************************************************************************
  */

#include "fd_console.h"
#include "main.h"
#include <stdio.h>

UART_HandleTypeDef huart1;

void fd_console_init(uint32_t baudrate)
{
    UART_HandleTypeDef *huart = &huart1;

    huart->Instance = USART1;
    huart->Init.BaudRate = baudrate;
    huart->Init.WordLength = UART_WORDLENGTH_8B;
    huart->Init.StopBits = UART_STOPBITS_1;
    huart->Init.Parity = UART_PARITY_NONE;
    huart->Init.Mode = UART_MODE_TX_RX;
    huart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart->Init.OverSampling = UART_OVERSAMPLING_16;
    huart->Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    huart->Init.ClockPrescaler = UART_PRESCALER_DIV1;
    huart->AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

    if (HAL_UART_Init(huart) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_UARTEx_SetTxFifoThreshold(huart, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_UARTEx_SetRxFifoThreshold(huart, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_UARTEx_DisableFifoMode(huart) != HAL_OK)
    {
        Error_Handler();
    }

    /* printf 无缓冲输出 */
    setvbuf(stdout, NULL, _IONBF, 0);
}

/* newlib printf 字符输出重定向 */
int __io_putchar(int ch)
{
    (void)HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);

    return ch;
}
