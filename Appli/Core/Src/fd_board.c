/**
  ******************************************************************************
  * @file    fd_board.c
  * @brief   板级硬件初始化实现
  *
  *          硬件对应关系：
  *            XSPI1 -> HyperRAM（帧缓冲 EXTRAM 0x91000000）
  *            XSPI2 -> NORFlash（代码执行介质）
  *            LTDC  -> 800x480 RGB888565 LCD
  *            I2C2  -> 相机/外设控制总线
  *            USART1-> 调试串口
  ******************************************************************************
  */

#include "fd_board.h"
#include "main.h"
#include "npu_cache.h"
#ifdef DEBUG
#include "hyperram.h"
#include "norflash.h"
#endif

/* ==================== 外设句柄（bsp_lcd.c / 本模块共用） ==================== */
DMA2D_HandleTypeDef hdma2d;
I2C_HandleTypeDef hi2c2;
LTDC_HandleTypeDef hltdc;
RAMCFG_HandleTypeDef hramcfg_SRAM3;
RAMCFG_HandleTypeDef hramcfg_SRAM4;
RAMCFG_HandleTypeDef hramcfg_SRAM5;
RAMCFG_HandleTypeDef hramcfg_SRAM6;
XSPI_HandleTypeDef hxspi1;
XSPI_HandleTypeDef hxspi2;

static void fd_board_gpio_init(void);
static void fd_board_ramcfg_init(void);
static void fd_board_isolation_config(void);

/* ==================== 板级初始化 ==================== */

void fd_board_init(void)
{
    fd_board_gpio_init();
    fd_board_ramcfg_init();
    fd_board_isolation_config();
}

/* GPIO：LCD 背光、复位控制等关键 IO（时钟与初始电平沿用板级设计） */
static void fd_board_gpio_init(void)
{
    GPIO_InitTypeDef gpio = {0};

    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOP_CLK_ENABLE();
    __HAL_RCC_GPIOO_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPION_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /* PE10: LCD 背光（高有效） */
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_10, GPIO_PIN_SET);
    gpio.Pin = GPIO_PIN_10;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Pull = GPIO_PULLDOWN;
    gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOE, &gpio);

    /* PG6/PG4: 低电平有效的复位/使能控制 */
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6 | GPIO_PIN_4, GPIO_PIN_RESET);
    gpio.Pin = GPIO_PIN_6 | GPIO_PIN_4;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Pull = GPIO_PULLUP;
    gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOG, &gpio);

    /* PA3: 相机使能控制 */
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
    gpio.Pin = GPIO_PIN_3;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Pull = GPIO_PULLUP;
    gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOA, &gpio);

    /* PG10: 外设电源保持 */
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_10, GPIO_PIN_SET);
    gpio.Pin = GPIO_PIN_10;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Pull = GPIO_PULLDOWN;
    gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOG, &gpio);
}

/* AXISRAM3~6 上电并使能 */
static void fd_board_ramcfg_init(void)
{
    hramcfg_SRAM3.Instance = RAMCFG_SRAM3_AXI;
    if (HAL_RAMCFG_Init(&hramcfg_SRAM3) != HAL_OK)
    {
        Error_Handler();
    }
    hramcfg_SRAM4.Instance = RAMCFG_SRAM4_AXI;
    if (HAL_RAMCFG_Init(&hramcfg_SRAM4) != HAL_OK)
    {
        Error_Handler();
    }
    hramcfg_SRAM5.Instance = RAMCFG_SRAM5_AXI;
    if (HAL_RAMCFG_Init(&hramcfg_SRAM5) != HAL_OK)
    {
        Error_Handler();
    }
    hramcfg_SRAM6.Instance = RAMCFG_SRAM6_AXI;
    if (HAL_RAMCFG_Init(&hramcfg_SRAM6) != HAL_OK)
    {
        Error_Handler();
    }

    HAL_RAMCFG_EnableAXISRAM(&hramcfg_SRAM3);
    HAL_RAMCFG_EnableAXISRAM(&hramcfg_SRAM4);
    HAL_RAMCFG_EnableAXISRAM(&hramcfg_SRAM5);
    HAL_RAMCFG_EnableAXISRAM(&hramcfg_SRAM6);
}

/* RIF 资源隔离：DCMIPP/DMA2D/LTDC/NPU/CSI 配置为安全特权域 CID1 */
static void fd_board_isolation_config(void)
{
    RIMC_MasterConfig_t rimc_master = {0};

    __HAL_RCC_RIFSC_CLK_ENABLE();

    rimc_master.MasterCID = RIF_CID_1;
    rimc_master.SecPriv = RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV;

    HAL_RIF_RIMC_ConfigMasterAttributes(RIF_MASTER_INDEX_DCMIPP, &rimc_master);
    HAL_RIF_RIMC_ConfigMasterAttributes(RIF_MASTER_INDEX_DMA2D, &rimc_master);
    HAL_RIF_RIMC_ConfigMasterAttributes(RIF_MASTER_INDEX_LTDC1, &rimc_master);
    HAL_RIF_RIMC_ConfigMasterAttributes(RIF_MASTER_INDEX_LTDC2, &rimc_master);
    HAL_RIF_RIMC_ConfigMasterAttributes(RIF_MASTER_INDEX_NPU, &rimc_master);

    HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RISC_PERIPH_INDEX_DCMIPP,
                                          RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV);
    HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RISC_PERIPH_INDEX_DMA2D,
                                          RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV);
    HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RISC_PERIPH_INDEX_LTDC,
                                          RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV);
    HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RISC_PERIPH_INDEX_LTDCL1,
                                          RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV);
    HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RISC_PERIPH_INDEX_LTDCL2,
                                          RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV);
    HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RISC_PERIPH_INDEX_NPU,
                                          RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV);
    /* CSI 相机接口同样需要 RIF 配置 */
    HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RISC_PERIPH_INDEX_CSI,
                                          RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV);

    HAL_PWR_ConfigAttributes(PWR_ITEM_0, PWR_SEC_NPRIV);
}

/* ==================== AI 加速子系统 ==================== */

void fd_board_ai_accel_init(void)
{
    __HAL_RCC_NPU_CLK_ENABLE();
    __HAL_RCC_NPU_FORCE_RESET();
    __HAL_RCC_NPU_RELEASE_RESET();

    __HAL_RCC_IAC_CLK_ENABLE();
    __HAL_RCC_IAC_FORCE_RESET();
    __HAL_RCC_IAC_RELEASE_RESET();

    npu_cache_init();
    npu_cache_enable();
}

void fd_board_lowpower_clock_init(void)
{
    LL_BUS_EnableClockLowPower(~0);
    LL_MEM_EnableClockLowPower(~0);
    LL_AHB1_GRP1_EnableClockLowPower(~0);
    LL_AHB2_GRP1_EnableClockLowPower(~0);
    LL_AHB3_GRP1_EnableClockLowPower(~0);
    LL_AHB4_GRP1_EnableClockLowPower(~0);
    LL_AHB5_GRP1_EnableClockLowPower(~0);
    LL_APB1_GRP1_EnableClockLowPower(~0);
    LL_APB1_GRP2_EnableClockLowPower(~0);
    LL_APB2_GRP1_EnableClockLowPower(~0);
    LL_APB4_GRP1_EnableClockLowPower(~0);
    LL_APB4_GRP2_EnableClockLowPower(~0);
    LL_APB5_GRP1_EnableClockLowPower(~0);
    LL_MISC_EnableClockLowPower(~0);
}

/* ==================== 外设初始化（供 bsp_lcd.c 等按名调用） ==================== */

void MX_DMA2D_Init(void)
{
    hdma2d.Instance = DMA2D;
    hdma2d.Init.Mode = DMA2D_R2M;
    hdma2d.Init.ColorMode = DMA2D_OUTPUT_RGB565;
    hdma2d.Init.OutputOffset = 0;

    if (HAL_DMA2D_Init(&hdma2d) != HAL_OK)
    {
        Error_Handler();
    }
}

void MX_I2C2_Init(void)
{
    hi2c2.Instance = I2C2;
    hi2c2.Init.Timing = 0x30C0EDFF;
    hi2c2.Init.OwnAddress1 = 0;
    hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c2.Init.OwnAddress2 = 0;
    hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

    if (HAL_I2C_Init(&hi2c2) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK)
    {
        Error_Handler();
    }
}

void MX_LTDC_Init(void)
{
    LTDC_LayerCfgTypeDef layer0 = {0};
    LTDC_LayerCfgTypeDef layer1 = {0};

    /* 800x480 RGB 屏时序 */
    hltdc.Instance = LTDC;
    hltdc.Init.HSPolarity = LTDC_HSPOLARITY_AL;
    hltdc.Init.VSPolarity = LTDC_VSPOLARITY_AL;
    hltdc.Init.DEPolarity = LTDC_DEPOLARITY_AL;
    hltdc.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
    hltdc.Init.HorizontalSync = 47;
    hltdc.Init.VerticalSync = 2;
    hltdc.Init.AccumulatedHBP = 135;
    hltdc.Init.AccumulatedVBP = 34;
    hltdc.Init.AccumulatedActiveW = 935;
    hltdc.Init.AccumulatedActiveH = 514;
    hltdc.Init.TotalWidth = 975;
    hltdc.Init.TotalHeigh = 527;
    hltdc.Init.Backcolor.Blue = 0;
    hltdc.Init.Backcolor.Green = 0;
    hltdc.Init.Backcolor.Red = 0;

    if (HAL_LTDC_Init(&hltdc) != HAL_OK)
    {
        Error_Handler();
    }

    /* Layer0: RGB565 摄像头层 */
    layer0.WindowX0 = 0;
    layer0.WindowX1 = 800;
    layer0.WindowY0 = 0;
    layer0.WindowY1 = 480;
    layer0.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;
    layer0.Alpha = 0xFF;
    layer0.Alpha0 = 0;
    layer0.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
    layer0.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
    layer0.FBStartAdress = 0;
    layer0.ImageWidth = 800;
    layer0.ImageHeight = 480;
    layer0.Backcolor.Blue = 0;
    layer0.Backcolor.Green = 0;
    layer0.Backcolor.Red = 0;
    if (HAL_LTDC_ConfigLayer(&hltdc, &layer0, 0) != HAL_OK)
    {
        Error_Handler();
    }

    /* Layer1: ARGB4444 叠加层 */
    layer1.WindowX0 = 0;
    layer1.WindowX1 = 800;
    layer1.WindowY0 = 0;
    layer1.WindowY1 = 480;
    layer1.PixelFormat = LTDC_PIXEL_FORMAT_ARGB4444;
    layer1.Alpha = 0xFF;
    layer1.Alpha0 = 0;
    layer1.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
    layer1.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
    layer1.FBStartAdress = 0;
    layer1.ImageWidth = 800;
    layer1.ImageHeight = 480;
    layer1.Backcolor.Blue = 0;
    layer1.Backcolor.Green = 0;
    layer1.Backcolor.Red = 0;
    if (HAL_LTDC_ConfigLayer(&hltdc, &layer1, 1) != HAL_OK)
    {
        Error_Handler();
    }
}

#ifdef DEBUG
void MX_XSPI1_Init(void)
{
    XSPIM_CfgTypeDef xspi_mgr_cfg = {0};
    XSPI_HyperbusCfgTypeDef hyperbus_cfg = {0};

    /* XSPI1: HyperRAM (256MB 空间, Hyperbus 模式) */
    hxspi1.Instance = XSPI1;
    hxspi1.Init.FifoThresholdByte = 4;
    hxspi1.Init.MemoryMode = HAL_XSPI_SINGLE_MEM;
    hxspi1.Init.MemoryType = HAL_XSPI_MEMTYPE_HYPERBUS;
    hxspi1.Init.MemorySize = HAL_XSPI_SIZE_256MB;
    hxspi1.Init.ChipSelectHighTimeCycle = 2;
    hxspi1.Init.FreeRunningClock = HAL_XSPI_FREERUNCLK_DISABLE;
    hxspi1.Init.ClockMode = HAL_XSPI_CLOCK_MODE_0;
    hxspi1.Init.WrapSize = HAL_XSPI_WRAP_32_BYTES;
    hxspi1.Init.ClockPrescaler = 1 - 1;
    hxspi1.Init.SampleShifting = HAL_XSPI_SAMPLE_SHIFT_NONE;
    hxspi1.Init.DelayHoldQuarterCycle = HAL_XSPI_DHQC_DISABLE;
    hxspi1.Init.ChipSelectBoundary = HAL_XSPI_BONDARYOF_NONE;
    hxspi1.Init.MaxTran = 0;
    hxspi1.Init.Refresh = 0;
    hxspi1.Init.MemorySelect = HAL_XSPI_CSSEL_NCS1;
    if (HAL_XSPI_Init(&hxspi1) != HAL_OK)
    {
        Error_Handler();
    }

    xspi_mgr_cfg.nCSOverride = HAL_XSPI_CSSEL_OVR_NCS1;
    xspi_mgr_cfg.IOPort = HAL_XSPIM_IOPORT_1;
    xspi_mgr_cfg.Req2AckTime = 1;
    if (HAL_XSPIM_Config(&hxspi1, &xspi_mgr_cfg, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }

    hyperbus_cfg.RWRecoveryTimeCycle = 7;
    hyperbus_cfg.AccessTimeCycle = 7;
    hyperbus_cfg.WriteZeroLatency = HAL_XSPI_LATENCY_ON_WRITE;
    hyperbus_cfg.LatencyMode = HAL_XSPI_FIXED_LATENCY;
    if (HAL_XSPI_HyperbusCfg(&hxspi1, &hyperbus_cfg, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }
}

void MX_XSPI2_Init(void)
{
    XSPIM_CfgTypeDef xspi_mgr_cfg = {0};

    /* XSPI2: NORFlash (Macronix 256MB 空间) */
    hxspi2.Instance = XSPI2;
    hxspi2.Init.FifoThresholdByte = 4;
    hxspi2.Init.MemoryMode = HAL_XSPI_SINGLE_MEM;
    hxspi2.Init.MemoryType = HAL_XSPI_MEMTYPE_MACRONIX;
    hxspi2.Init.MemorySize = HAL_XSPI_SIZE_256MB;
    hxspi2.Init.ChipSelectHighTimeCycle = 1;
    hxspi2.Init.FreeRunningClock = HAL_XSPI_FREERUNCLK_DISABLE;
    hxspi2.Init.ClockMode = HAL_XSPI_CLOCK_MODE_0;
    hxspi2.Init.WrapSize = HAL_XSPI_WRAP_NOT_SUPPORTED;
    hxspi2.Init.ClockPrescaler = 1 - 1;
    hxspi2.Init.SampleShifting = HAL_XSPI_SAMPLE_SHIFT_NONE;
    hxspi2.Init.DelayHoldQuarterCycle = HAL_XSPI_DHQC_DISABLE;
    hxspi2.Init.ChipSelectBoundary = HAL_XSPI_BONDARYOF_NONE;
    hxspi2.Init.MaxTran = 0;
    hxspi2.Init.Refresh = 0;
    hxspi2.Init.MemorySelect = HAL_XSPI_CSSEL_NCS1;
    if (HAL_XSPI_Init(&hxspi2) != HAL_OK)
    {
        Error_Handler();
    }

    xspi_mgr_cfg.nCSOverride = HAL_XSPI_CSSEL_OVR_NCS1;
    xspi_mgr_cfg.IOPort = HAL_XSPIM_IOPORT_2;
    xspi_mgr_cfg.Req2AckTime = 1;
    if (HAL_XSPIM_Config(&hxspi2, &xspi_mgr_cfg, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }
}
#endif /* DEBUG */

/* ==================== 调试模式时钟与外部存储器 ==================== */

#ifdef DEBUG

/* 调试器直接加载运行（未经过 FSBL）时配置 PLL 时钟树：
   PLL1: HSI/2*25 = 800MHz -> IC1/1 CPU 800MHz, IC2/2 SYS 400MHz
   PLL2: HSI/8*125 = 1GHz  -> IC6/1（相机/DCMIPP 源）
   PLL3: HSI/8*200/2 = 800MHz -> IC11/1、IC16/24（LTDC 像素时钟）
   PLL4: HSI/8*225/18 = 100MHz（CSI/其它外设源） */
void fd_board_debug_clock_init(void)
{
    RCC_OscInitTypeDef osc = {0};
    RCC_ClkInitTypeDef clk = {0};

    /* 电源切换到 SMPS 供电 */
    if (HAL_PWREx_ConfigSupply(PWR_SMPS_SUPPLY) != HAL_OK)
    {
        Error_Handler();
    }

    /* 先用 HSI 运行 */
    osc.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    osc.HSIState = RCC_HSI_ON;
    osc.HSIDiv = RCC_HSI_DIV1;
    osc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    osc.PLL1.PLLState = RCC_PLL_NONE;
    osc.PLL2.PLLState = RCC_PLL_NONE;
    osc.PLL3.PLLState = RCC_PLL_NONE;
    osc.PLL4.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&osc) != HAL_OK)
    {
        Error_Handler();
    }

    /* 若 CPU/SYS 当前挂在 IC 上，先切回 HSI 以便安全重配 PLL */
    HAL_RCC_GetClockConfig(&clk);
    if ((clk.CPUCLKSource == RCC_CPUCLKSOURCE_IC1) ||
        (clk.SYSCLKSource == RCC_SYSCLKSOURCE_IC2_IC6_IC11))
    {
        clk.ClockType = RCC_CLOCKTYPE_CPUCLK | RCC_CLOCKTYPE_SYSCLK;
        clk.CPUCLKSource = RCC_CPUCLKSOURCE_HSI;
        clk.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
        if (HAL_RCC_ClockConfig(&clk) != HAL_OK)
        {
            Error_Handler();
        }
    }

    /* 配置 PLL1/2/3/4 */
    osc.OscillatorType = RCC_OSCILLATORTYPE_NONE;
    osc.PLL1.PLLState = RCC_PLL_ON;
    osc.PLL1.PLLSource = RCC_PLLSOURCE_HSI;
    osc.PLL1.PLLM = 2;
    osc.PLL1.PLLN = 25;
    osc.PLL1.PLLFractional = 0;
    osc.PLL1.PLLP1 = 1;
    osc.PLL1.PLLP2 = 1;
    osc.PLL2.PLLState = RCC_PLL_ON;
    osc.PLL2.PLLSource = RCC_PLLSOURCE_HSI;
    osc.PLL2.PLLM = 8;
    osc.PLL2.PLLN = 125;
    osc.PLL2.PLLFractional = 0;
    osc.PLL2.PLLP1 = 1;
    osc.PLL2.PLLP2 = 1;
    osc.PLL3.PLLState = RCC_PLL_ON;
    osc.PLL3.PLLSource = RCC_PLLSOURCE_HSI;
    osc.PLL3.PLLM = 8;
    osc.PLL3.PLLN = 200;
    osc.PLL3.PLLFractional = 0;
    osc.PLL3.PLLP1 = 1;
    osc.PLL3.PLLP2 = 2;
    osc.PLL4.PLLState = RCC_PLL_ON;
    osc.PLL4.PLLSource = RCC_PLLSOURCE_HSI;
    osc.PLL4.PLLM = 8;
    osc.PLL4.PLLN = 225;
    osc.PLL4.PLLFractional = 0;
    osc.PLL4.PLLP1 = 3;
    osc.PLL4.PLLP2 = 6;
    if (HAL_RCC_OscConfig(&osc) != HAL_OK)
    {
        Error_Handler();
    }

    /* CPU/SYS/总线时钟切换到 PLL 输出 */
    clk.ClockType = RCC_CLOCKTYPE_CPUCLK | RCC_CLOCKTYPE_HCLK
                  | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1
                  | RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_PCLK5
                  | RCC_CLOCKTYPE_PCLK4;
    clk.CPUCLKSource = RCC_CPUCLKSOURCE_IC1;
    clk.SYSCLKSource = RCC_SYSCLKSOURCE_IC2_IC6_IC11;
    clk.AHBCLKDivider = RCC_HCLK_DIV2;
    clk.APB1CLKDivider = RCC_APB1_DIV1;
    clk.APB2CLKDivider = RCC_APB2_DIV1;
    clk.APB4CLKDivider = RCC_APB4_DIV1;
    clk.APB5CLKDivider = RCC_APB5_DIV1;
    clk.IC1Selection.ClockSelection = RCC_ICCLKSOURCE_PLL1;
    clk.IC1Selection.ClockDivider = 1;
    clk.IC2Selection.ClockSelection = RCC_ICCLKSOURCE_PLL1;
    clk.IC2Selection.ClockDivider = 2;
    clk.IC6Selection.ClockSelection = RCC_ICCLKSOURCE_PLL2;
    clk.IC6Selection.ClockDivider = 1;
    clk.IC11Selection.ClockSelection = RCC_ICCLKSOURCE_PLL3;
    clk.IC11Selection.ClockDivider = 1;
    if (HAL_RCC_ClockConfig(&clk) != HAL_OK)
    {
        Error_Handler();
    }
}

/* 调试模式下手动初始化并映射外部存储器（FSBL 正常启动时无需此步骤） */
void fd_board_debug_extmem_init(void)
{
    HyperRAM_ObjectTypeDef hyperram = {0};
    NORFlash_ObjectTypeDef norflash = {0};

    MX_XSPI1_Init();
    MX_XSPI2_Init();

    if (HyperRAM_Init(&hyperram, &hxspi1) != HyperRAM_OK)
    {
        Error_Handler();
    }
    if (NORFlash_Init(&norflash, &hxspi2,
                      HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_XSPI2)) != NORFlash_OK)
    {
        Error_Handler();
    }
    if (HyperRAM_EnableMemoryMappedMode(&hyperram) != HyperRAM_OK)
    {
        Error_Handler();
    }
    if (NORFlash_EnableMemoryMappedMode(&norflash) != NORFlash_OK)
    {
        Error_Handler();
    }
}

#endif /* DEBUG */

/* ==================== 错误处理 ==================== */

void Error_Handler(void)
{
    __disable_irq();
    /* 保存故障现场寄存器，便于调试器查看 */
    volatile uint32_t cfsr = SCB->CFSR;
    volatile uint32_t hfsr = SCB->HFSR;
    volatile uint32_t bfar = SCB->BFAR;
    volatile uint32_t mmfar = SCB->MMFAR;
    (void)cfsr; (void)hfsr; (void)bfar; (void)mmfar;

    while (1)
    {
    }
}
