/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file         stm32n6xx_hal_msp.c
  * @brief        This file provides code for the MSP Initialization
  *               and de-Initialization codes.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN Define */

/* USER CODE END Define */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN Macro */

/* USER CODE END Macro */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* External functions --------------------------------------------------------*/
/* USER CODE BEGIN ExternalFunctions */

/* USER CODE END ExternalFunctions */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */
/**
  * Initializes the Global MSP.
  */
void HAL_MspInit(void)
{

  /* USER CODE BEGIN MspInit 0 */

  /* USER CODE END MspInit 0 */

  /* System interrupt init*/

  HAL_PWREx_EnableVddIO2();
  HAL_PWREx_ConfigVddIORange(PWR_VDDIO2,PWR_VDDIO_RANGE_1V8);

  HAL_PWREx_EnableVddIO3();
  HAL_PWREx_ConfigVddIORange(PWR_VDDIO3,PWR_VDDIO_RANGE_1V8);

  HAL_PWREx_EnableVddIO4();
  HAL_PWREx_ConfigVddIORange(PWR_VDDIO4,PWR_VDDIO_RANGE_3V3);

  /* USER CODE BEGIN MspInit 1 */

  /* USER CODE END MspInit 1 */
}

/**
* @brief DMA2D MSP Initialization
* This function configures the hardware resources used in this example
* @param hdma2d: DMA2D handle pointer
* @retval None
*/
void HAL_DMA2D_MspInit(DMA2D_HandleTypeDef* hdma2d)
{
  if(hdma2d->Instance==DMA2D)
  {
  /* USER CODE BEGIN DMA2D_MspInit 0 */

  /* USER CODE END DMA2D_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_DMA2D_CLK_ENABLE();
  /* USER CODE BEGIN DMA2D_MspInit 1 */

  /* USER CODE END DMA2D_MspInit 1 */

  }

}

/**
* @brief DMA2D MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hdma2d: DMA2D handle pointer
* @retval None
*/
void HAL_DMA2D_MspDeInit(DMA2D_HandleTypeDef* hdma2d)
{
  if(hdma2d->Instance==DMA2D)
  {
  /* USER CODE BEGIN DMA2D_MspDeInit 0 */

  /* USER CODE END DMA2D_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_DMA2D_CLK_DISABLE();
  /* USER CODE BEGIN DMA2D_MspDeInit 1 */

  /* USER CODE END DMA2D_MspDeInit 1 */
  }

}

/**
* @brief I2C MSP Initialization
* This function configures the hardware resources used in this example
* @param hi2c: I2C handle pointer
* @retval None
*/
void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(hi2c->Instance==I2C2)
  {
  /* USER CODE BEGIN I2C2_MspInit 0 */

  /* USER CODE END I2C2_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2C2;
    PeriphClkInitStruct.I2c2ClockSelection = RCC_I2C2CLKSOURCE_PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**I2C2 GPIO Configuration
    PD4     ------> I2C2_SDA
    PD14     ------> I2C2_SCL
    */
    GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_14;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C2;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* Peripheral clock enable */
    __HAL_RCC_I2C2_CLK_ENABLE();
  /* USER CODE BEGIN I2C2_MspInit 1 */

  /* USER CODE END I2C2_MspInit 1 */

  }

}

/**
* @brief I2C MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hi2c: I2C handle pointer
* @retval None
*/
void HAL_I2C_MspDeInit(I2C_HandleTypeDef* hi2c)
{
  if(hi2c->Instance==I2C2)
  {
  /* USER CODE BEGIN I2C2_MspDeInit 0 */

  /* USER CODE END I2C2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_I2C2_CLK_DISABLE();

    /**I2C2 GPIO Configuration
    PD4     ------> I2C2_SDA
    PD14     ------> I2C2_SCL
    */
    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_4);

    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_14);

  /* USER CODE BEGIN I2C2_MspDeInit 1 */

  /* USER CODE END I2C2_MspDeInit 1 */
  }

}

/**
* @brief LTDC MSP Initialization
* This function configures the hardware resources used in this example
* @param hltdc: LTDC handle pointer
* @retval None
*/
void HAL_LTDC_MspInit(LTDC_HandleTypeDef* hltdc)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(hltdc->Instance==LTDC)
  {
  /* USER CODE BEGIN LTDC_MspInit 0 */

  /* USER CODE END LTDC_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
    PeriphClkInitStruct.LtdcClockSelection = RCC_LTDCCLKSOURCE_IC16;
    PeriphClkInitStruct.ICSelection[RCC_IC16].ClockSelection = RCC_ICCLKSOURCE_PLL3;
    PeriphClkInitStruct.ICSelection[RCC_IC16].ClockDivider = 24;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* Peripheral clock enable */
    __HAL_RCC_LTDC_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    /**LTDC GPIO Configuration
    PB15     ------> LTDC_G4
    PH4     ------> LTDC_R4
    PF8     ------> LTDC_R6
    PF9     ------> LTDC_HSYNC
    PA9     ------> LTDC_B5
    PA1     ------> LTDC_G2
    PB11     ------> LTDC_G6
    PA15(JTDI)     ------> LTDC_R5
    PG15     ------> LTDC_B0
    PA10     ------> LTDC_B4
    PA5     ------> LTDC_CLK
    PG1     ------> LTDC_G1
    PB12     ------> LTDC_G5
    PB1     ------> LTDC_R1
    PA7     ------> LTDC_B1
    PG0     ------> LTDC_VSYNC
    PA11     ------> LTDC_B3
    PA2     ------> LTDC_B7
    PG12     ------> LTDC_G0
    PG2     ------> LTDC_R0
    PB4(NJTRST)     ------> LTDC_R3
    PG9     ------> LTDC_R7
    PA12     ------> LTDC_B2
    PA8     ------> LTDC_B6
    PG13     ------> LTDC_DE
    PA0     ------> LTDC_G3
    PB10     ------> LTDC_G7
    PB5(JTDO/TRACESWO)     ------> LTDC_R2
    */
    GPIO_InitStruct.Pin = GPIO_PIN_15|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_1
                          |GPIO_PIN_4|GPIO_PIN_10|GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF14_LCD;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF14_LCD;
    HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF14_LCD;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_1|GPIO_PIN_15|GPIO_PIN_10
                          |GPIO_PIN_5|GPIO_PIN_7|GPIO_PIN_11|GPIO_PIN_2
                          |GPIO_PIN_12|GPIO_PIN_8|GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF14_LCD;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_15|GPIO_PIN_1|GPIO_PIN_12|GPIO_PIN_2
                          |GPIO_PIN_9|GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF14_LCD;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_LCD;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /* USER CODE BEGIN LTDC_MspInit 1 */

  /* USER CODE END LTDC_MspInit 1 */

  }

}

/**
* @brief LTDC MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hltdc: LTDC handle pointer
* @retval None
*/
void HAL_LTDC_MspDeInit(LTDC_HandleTypeDef* hltdc)
{
  if(hltdc->Instance==LTDC)
  {
  /* USER CODE BEGIN LTDC_MspDeInit 0 */

  /* USER CODE END LTDC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_LTDC_CLK_DISABLE();

    /**LTDC GPIO Configuration
    PB15     ------> LTDC_G4
    PH4     ------> LTDC_R4
    PF8     ------> LTDC_R6
    PF9     ------> LTDC_HSYNC
    PA9     ------> LTDC_B5
    PA1     ------> LTDC_G2
    PB11     ------> LTDC_G6
    PA15(JTDI)     ------> LTDC_R5
    PG15     ------> LTDC_B0
    PA10     ------> LTDC_B4
    PA5     ------> LTDC_CLK
    PG1     ------> LTDC_G1
    PB12     ------> LTDC_G5
    PB1     ------> LTDC_R1
    PA7     ------> LTDC_B1
    PG0     ------> LTDC_VSYNC
    PA11     ------> LTDC_B3
    PA2     ------> LTDC_B7
    PG12     ------> LTDC_G0
    PG2     ------> LTDC_R0
    PB4(NJTRST)     ------> LTDC_R3
    PG9     ------> LTDC_R7
    PA12     ------> LTDC_B2
    PA8     ------> LTDC_B6
    PG13     ------> LTDC_DE
    PA0     ------> LTDC_G3
    PB10     ------> LTDC_G7
    PB5(JTDO/TRACESWO)     ------> LTDC_R2
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_15|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_1
                          |GPIO_PIN_4|GPIO_PIN_10|GPIO_PIN_5);

    HAL_GPIO_DeInit(GPIOH, GPIO_PIN_4);

    HAL_GPIO_DeInit(GPIOF, GPIO_PIN_8|GPIO_PIN_9);

    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_1|GPIO_PIN_15|GPIO_PIN_10
                          |GPIO_PIN_5|GPIO_PIN_7|GPIO_PIN_11|GPIO_PIN_2
                          |GPIO_PIN_12|GPIO_PIN_8|GPIO_PIN_0);

    HAL_GPIO_DeInit(GPIOG, GPIO_PIN_15|GPIO_PIN_1|GPIO_PIN_0|GPIO_PIN_12
                          |GPIO_PIN_2|GPIO_PIN_9|GPIO_PIN_13);

  /* USER CODE BEGIN LTDC_MspDeInit 1 */

  /* USER CODE END LTDC_MspDeInit 1 */
  }

}

/**
* @brief RAMCFG MSP Initialization
* This function configures the hardware resources used in this example
* @param hramcfg: RAMCFG handle pointer
* @retval None
*/
void HAL_RAMCFG_MspInit(RAMCFG_HandleTypeDef* hramcfg)
{
  /* USER CODE BEGIN RAMCFG_MspInit 0 */
  __HAL_RCC_AXISRAM3_MEM_CLK_ENABLE();
  __HAL_RCC_AXISRAM4_MEM_CLK_ENABLE();
  __HAL_RCC_AXISRAM5_MEM_CLK_ENABLE();
  __HAL_RCC_AXISRAM6_MEM_CLK_ENABLE();

  /* USER CODE END RAMCFG_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_RAMCFG_CLK_ENABLE();
  /* USER CODE BEGIN RAMCFG_MspInit 1 */

  /* USER CODE END RAMCFG_MspInit 1 */

}

/**
* @brief RAMCFG MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hramcfg: RAMCFG handle pointer
* @retval None
*/
void HAL_RAMCFG_MspDeInit(RAMCFG_HandleTypeDef* hramcfg)
{
  /* USER CODE BEGIN RAMCFG_MspDeInit 0 */
  __HAL_RCC_AXISRAM3_MEM_CLK_DISABLE();
  __HAL_RCC_AXISRAM4_MEM_CLK_DISABLE();
  __HAL_RCC_AXISRAM5_MEM_CLK_DISABLE();
  __HAL_RCC_AXISRAM6_MEM_CLK_DISABLE();

  /* USER CODE END RAMCFG_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_RAMCFG_CLK_DISABLE();
  /* USER CODE BEGIN RAMCFG_MspDeInit 1 */

  /* USER CODE END RAMCFG_MspDeInit 1 */

}

/**
* @brief UART MSP Initialization
* This function configures the hardware resources used in this example
* @param huart: UART handle pointer
* @retval None
*/
void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(huart->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART1;
    PeriphClkInitStruct.Usart1ClockSelection = RCC_USART1CLKSOURCE_CLKP;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* Peripheral clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOE_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PE5     ------> USART1_TX
    PE6     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */

  }

}

/**
* @brief UART MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param huart: UART handle pointer
* @retval None
*/
void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{
  if(huart->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PE5     ------> USART1_TX
    PE6     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOE, GPIO_PIN_5|GPIO_PIN_6);

  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }

}

static uint32_t HAL_RCC_XSPIM_CLK_ENABLED=0;

/**
* @brief XSPI MSP Initialization
* This function configures the hardware resources used in this example
* @param hxspi: XSPI handle pointer
* @retval None
*/
void HAL_XSPI_MspInit(XSPI_HandleTypeDef* hxspi)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(hxspi->Instance==XSPI1)
  {
  /* USER CODE BEGIN XSPI1_MspInit 0 */

  /* USER CODE END XSPI1_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_XSPI1;
    PeriphClkInitStruct.Xspi1ClockSelection = RCC_XSPI1CLKSOURCE_HCLK;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* Peripheral clock enable */
    HAL_RCC_XSPIM_CLK_ENABLED++;
    if(HAL_RCC_XSPIM_CLK_ENABLED==1){
      __HAL_RCC_XSPIM_CLK_ENABLE();
    }
    __HAL_RCC_XSPI1_CLK_ENABLE();

    __HAL_RCC_GPIOP_CLK_ENABLE();
    __HAL_RCC_GPIOO_CLK_ENABLE();
    /**XSPI1 GPIO Configuration
    PP7     ------> XSPIM_P1_IO7
    PP6     ------> XSPIM_P1_IO6
    PP0     ------> XSPIM_P1_IO0
    PP4     ------> XSPIM_P1_IO4
    PP1     ------> XSPIM_P1_IO1
    PP5     ------> XSPIM_P1_IO5
    PP3     ------> XSPIM_P1_IO3
    PP2     ------> XSPIM_P1_IO2
    PO5     ------> XSPIM_P1_NCLK
    PO2     ------> XSPIM_P1_DQS0
    PO0     ------> XSPIM_P1_NCS1
    PO4     ------> XSPIM_P1_CLK
    */
    GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_6|GPIO_PIN_0|GPIO_PIN_4
                          |GPIO_PIN_1|GPIO_PIN_5|GPIO_PIN_3|GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_XSPIM_P1;
    HAL_GPIO_Init(GPIOP, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_2|GPIO_PIN_0|GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_XSPIM_P1;
    HAL_GPIO_Init(GPIOO, &GPIO_InitStruct);

  /* USER CODE BEGIN XSPI1_MspInit 1 */

  /* USER CODE END XSPI1_MspInit 1 */
  }
  else if(hxspi->Instance==XSPI2)
  {
  /* USER CODE BEGIN XSPI2_MspInit 0 */

  /* USER CODE END XSPI2_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_XSPI2;
    PeriphClkInitStruct.Xspi2ClockSelection = RCC_XSPI2CLKSOURCE_HCLK;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* Peripheral clock enable */
    HAL_RCC_XSPIM_CLK_ENABLED++;
    if(HAL_RCC_XSPIM_CLK_ENABLED==1){
      __HAL_RCC_XSPIM_CLK_ENABLE();
    }
    __HAL_RCC_XSPI2_CLK_ENABLE();

    __HAL_RCC_GPION_CLK_ENABLE();
    /**XSPI2 GPIO Configuration
    PN4     ------> XSPIM_P2_IO2
    PN6     ------> XSPIM_P2_CLK
    PN8     ------> XSPIM_P2_IO4
    PN0     ------> XSPIM_P2_DQS0
    PN3     ------> XSPIM_P2_IO1
    PN5     ------> XSPIM_P2_IO3
    PN1     ------> XSPIM_P2_NCS1
    PN9     ------> XSPIM_P2_IO5
    PN2     ------> XSPIM_P2_IO0
    PN10     ------> XSPIM_P2_IO6
    PN11     ------> XSPIM_P2_IO7
    */
    GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_6|GPIO_PIN_8|GPIO_PIN_0
                          |GPIO_PIN_3|GPIO_PIN_5|GPIO_PIN_1|GPIO_PIN_9
                          |GPIO_PIN_2|GPIO_PIN_10|GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_XSPIM_P2;
    HAL_GPIO_Init(GPION, &GPIO_InitStruct);

  /* USER CODE BEGIN XSPI2_MspInit 1 */

  /* USER CODE END XSPI2_MspInit 1 */
  }

}

/**
* @brief XSPI MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hxspi: XSPI handle pointer
* @retval None
*/
void HAL_XSPI_MspDeInit(XSPI_HandleTypeDef* hxspi)
{
  if(hxspi->Instance==XSPI1)
  {
  /* USER CODE BEGIN XSPI1_MspDeInit 0 */

  /* USER CODE END XSPI1_MspDeInit 0 */
    /* Peripheral clock disable */
    HAL_RCC_XSPIM_CLK_ENABLED--;
    if(HAL_RCC_XSPIM_CLK_ENABLED==0){
      __HAL_RCC_XSPIM_CLK_DISABLE();
    }
    __HAL_RCC_XSPI1_CLK_DISABLE();

    /**XSPI1 GPIO Configuration
    PP7     ------> XSPIM_P1_IO7
    PP6     ------> XSPIM_P1_IO6
    PP0     ------> XSPIM_P1_IO0
    PP4     ------> XSPIM_P1_IO4
    PP1     ------> XSPIM_P1_IO1
    PP5     ------> XSPIM_P1_IO5
    PP3     ------> XSPIM_P1_IO3
    PP2     ------> XSPIM_P1_IO2
    PO5     ------> XSPIM_P1_NCLK
    PO2     ------> XSPIM_P1_DQS0
    PO0     ------> XSPIM_P1_NCS1
    PO4     ------> XSPIM_P1_CLK
    */
    HAL_GPIO_DeInit(GPIOP, GPIO_PIN_7|GPIO_PIN_6|GPIO_PIN_0|GPIO_PIN_4
                          |GPIO_PIN_1|GPIO_PIN_5|GPIO_PIN_3|GPIO_PIN_2);

    HAL_GPIO_DeInit(GPIOO, GPIO_PIN_5|GPIO_PIN_2|GPIO_PIN_0|GPIO_PIN_4);

  /* USER CODE BEGIN XSPI1_MspDeInit 1 */

  /* USER CODE END XSPI1_MspDeInit 1 */
  }
  else if(hxspi->Instance==XSPI2)
  {
  /* USER CODE BEGIN XSPI2_MspDeInit 0 */

  /* USER CODE END XSPI2_MspDeInit 0 */
    /* Peripheral clock disable */
    HAL_RCC_XSPIM_CLK_ENABLED--;
    if(HAL_RCC_XSPIM_CLK_ENABLED==0){
      __HAL_RCC_XSPIM_CLK_DISABLE();
    }
    __HAL_RCC_XSPI2_CLK_DISABLE();

    /**XSPI2 GPIO Configuration
    PN4     ------> XSPIM_P2_IO2
    PN6     ------> XSPIM_P2_CLK
    PN8     ------> XSPIM_P2_IO4
    PN0     ------> XSPIM_P2_DQS0
    PN3     ------> XSPIM_P2_IO1
    PN5     ------> XSPIM_P2_IO3
    PN1     ------> XSPIM_P2_NCS1
    PN9     ------> XSPIM_P2_IO5
    PN2     ------> XSPIM_P2_IO0
    PN10     ------> XSPIM_P2_IO6
    PN11     ------> XSPIM_P2_IO7
    */
    HAL_GPIO_DeInit(GPION, GPIO_PIN_4|GPIO_PIN_6|GPIO_PIN_8|GPIO_PIN_0
                          |GPIO_PIN_3|GPIO_PIN_5|GPIO_PIN_1|GPIO_PIN_9
                          |GPIO_PIN_2|GPIO_PIN_10|GPIO_PIN_11);

  /* USER CODE BEGIN XSPI2_MspDeInit 1 */

  /* USER CODE END XSPI2_MspDeInit 1 */
  }

}

/* USER CODE BEGIN 1 */
void HAL_CACHEAXI_MspInit(CACHEAXI_HandleTypeDef *hcacheaxi)
{
  if (hcacheaxi->Instance == CACHEAXI)
  {
    __HAL_RCC_CACHEAXIRAM_MEM_CLK_ENABLE();
    __HAL_RCC_CACHEAXI_CLK_ENABLE();
    __HAL_RCC_CACHEAXI_FORCE_RESET();
    __HAL_RCC_CACHEAXI_RELEASE_RESET();
  }
}

void HAL_CACHEAXI_MspDeInit(CACHEAXI_HandleTypeDef *hcacheaxi)
{
  if (hcacheaxi->Instance == CACHEAXI)
  {
    __HAL_RCC_CACHEAXIRAM_MEM_CLK_DISABLE();
    __HAL_RCC_CACHEAXI_CLK_DISABLE();
    __HAL_RCC_CACHEAXI_FORCE_RESET();
  }
}

/* USER CODE END 1 */
