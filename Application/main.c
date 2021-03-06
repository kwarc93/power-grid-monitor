/**
 ******************************************************************************
 * File Name          : main.c
 * Description        : Main program body
 ******************************************************************************
 *
 * Copyright (c) 2017 STMicroelectronics International N.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted, provided that the following conditions are met:
 *
 * 1. Redistribution of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of STMicroelectronics nor the names of other
 *    contributors to this software may be used to endorse or promote products
 *    derived from this software without specific written permission.
 * 4. This software, including modifications and/or derivative works of this
 *    software, must execute solely and exclusively on microcontroller or
 *    microprocessor devices manufactured by or for STMicroelectronics.
 * 5. Redistribution and use of this software other than as permitted under
 *    this license is void and will automatically terminate your rights under
 *    this license.
 *
 * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
 * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
 * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/
#include <Modules/fatfs.h>
#include <Modules/usb_host.h>
#include <Peripherals/adc.h>
#include <Peripherals/crc.h>
#include <Peripherals/dma.h>
#include <Peripherals/gpio.h>
#include <Peripherals/i2c.h>
#include <Peripherals/rtc.h>
#include <Peripherals/spi.h>
#include <Peripherals/tim.h>
#include <stm32f429i_discovery_sdram.h>
#include "main.h"
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "tlsf/tlsf.h"

#define CCMRAM	__attribute__((section (".ccmram")))
#define SDRAM	__attribute__((section (".sdram")))
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
void
SystemClock_Config(void);
void
SetupHardware(void);
void
Error_Handler(void);
void
MX_FREERTOS_Init(void);

/* Heap pool for FreeRTOS */
uint8_t ucHeap[configTOTAL_HEAP_SIZE] CCMRAM;

/* Heap pool for application */
#define APP_HEAP_SIZE	(4096 * 1024)
uint32_t appHeap[APP_HEAP_SIZE / sizeof(uint32_t)] SDRAM;

int
main(void)
{

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init ();

  /* Configure the system clock */
  SystemClock_Config ();

  /* Initialize all configured peripherals */
  SetupHardware ();

  /* Init application memory pool */
  if (!init_memory_pool (APP_HEAP_SIZE, appHeap))
	  Error_Handler ();

  /* Call init function for freertos objects (in freertos.c) */
  MX_FREERTOS_Init ();

  /* Start scheduler */
  osKernelStart ();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  while (1)
  {

  }

}
/** System Clock Configuration
 */
void
SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

  /**Configure the main internal regulator output voltage
   */
  __HAL_RCC_PWR_CLK_ENABLE()
  ;

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /**Initializes the CPU, AHB and APB busses clocks
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig (&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler ();
  }

  /**Initializes the CPU, AHB and APB busses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1
      | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig (&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler ();
  }

  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC | RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.PLLSAI.PLLSAIN = 50;
  PeriphClkInitStruct.PLLSAI.PLLSAIR = 2;
  PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_2;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig (&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler ();
  }

  /**Configure the Systick interrupt time
   */
  HAL_SYSTICK_Config (HAL_RCC_GetHCLKFreq () / 1000);

  /**Configure the Systick
   */
  HAL_SYSTICK_CLKSourceConfig (SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority (SysTick_IRQn, 15, 0);
}

void
SetupHardware(void)
{
  BSP_SDRAM_Init();
  MX_GPIO_Init ();
  MX_DMA_Init ();
  MX_ADC1_Init ();
  MX_ADC2_Init ();
  MX_CRC_Init ();
  MX_I2C3_Init ();
  MX_SPI5_Init ();
  MX_TIM2_Init ();
  MX_TIM3_Init ();
  MX_TIM4_Init ();
  MX_RTC_Init ();
}

/**
 * @brief  Period elapsed callback in non blocking mode
 * @note   This function is called  when TIM6 interrupt took place, inside
 * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
 * a global variable "uwTick" used as application time base.
 * @param  htim : TIM handle
 * @retval None
 */
void
HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{

  if (htim->Instance == TIM6)
  {
    HAL_IncTick ();
  }

}

/**
 * @brief  This function is executed in case of error occurrence.
 * @param  None
 * @retval None
 */
void
Error_Handler(void)
{
  /* User can add his own implementation to report the HAL error return state */
  HAL_GPIO_WritePin (GPIOG, LD4_Pin, GPIO_PIN_SET);
  asm volatile ("BKPT 0");
  while (1)
  {
  }
}

#ifdef USE_FULL_ASSERT

/**
 * @brief Reports the name of the source file and the source line number
 * where the assert_param error has occurred.
 * @param file: pointer to the source file name
 * @param line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
   ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
