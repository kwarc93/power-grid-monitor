/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
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
#include <RTOS/osThreads.h>

/* Variables -----------------------------------------------------------------*/

/* Function prototypes -------------------------------------------------------*/
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */

/* Init FreeRTOS */

void MX_FREERTOS_Init(void) {

  /* Create binary semaphore for DSP thread */
  osSemaphoreDef(DSP_Event);
  DSP_Semaphore = osSemaphoreCreate(osSemaphore(DSP_Event), 1U);
  osSemaphoreWait(DSP_Semaphore, 0U);

  /* Create binary semaphore for DL thread */
  osSemaphoreDef(DL_Event);
  DL_Semaphore = osSemaphoreCreate(osSemaphore(DL_Event), 1U);
  osSemaphoreWait(DL_Semaphore, 0U);

  /* Create binary semaphore for DR thread */
  osSemaphoreDef(DR_Event);
  DR_Semaphore = osSemaphoreCreate(osSemaphore(DR_Event), 1U);
  osSemaphoreWait(DR_Semaphore, 0U);

  /* Create binary semaphore for GUI thread */
  osSemaphoreDef(GUI_Event);
  GUI_Semaphore = osSemaphoreCreate(osSemaphore(GUI_Event), 1U);
  osSemaphoreWait(GUI_Semaphore, 0U);

  /* Create & start Default thread */
  osThreadDef(DEFThread, Default_Thread, osPriorityBelowNormal, 0, configMINIMAL_STACK_SIZE*8);
  DEFThreadHandle = osThreadCreate (osThread(DEFThread), NULL);

  /* Create & start DSP thread */
  osThreadDef(DSPThread, DSP_Thread, osPriorityRealtime, 0, configMINIMAL_STACK_SIZE*8);
  DSPThreadHandle = osThreadCreate (osThread(DSPThread), NULL);
  osThreadSuspend(DSPThreadHandle);

  /* Create & start GUI thread */
  osThreadDef(GUIThread, GUI_Thread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE*8);
  GUIThreadHandle = osThreadCreate (osThread(GUIThread), NULL);

  /* Create & start DL thread */
  osThreadDef(DLThread, DL_Thread, osPriorityHigh, 0, configMINIMAL_STACK_SIZE*8);
  DLThreadHandle = osThreadCreate (osThread(DLThread), NULL);

  /* Create & start DR thread */
  osThreadDef(DRThread, DR_Thread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE*8);
  DRThreadHandle = osThreadCreate (osThread(DRThread), NULL);

}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
