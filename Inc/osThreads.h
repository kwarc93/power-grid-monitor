/*
 * osThreads.h
 *
 *  Created on: 06.04.2017
 *      Author: Kwarc
 */

#ifndef _OSTHREADS_H_
#define _OSTHREADS_H_

#include "cmsis_os.h"

#define DSP_SIGNAL	(int32_t)(1)
#define GUI_SIGNAL	(int32_t)(2)
#define DL_SIGNAL	(int32_t)(3)

osThreadId LED1ThreadHandle, LED2ThreadHandle;
osThreadId DEFThreadHandle, DSPThreadHandle, GUIThreadHandle, DLThreadHandle;
osSemaphoreId DSP_Semaphore, DL_Semaphore, GUI_Semaphore;

void LED1_Thread(void const *argument);
void LED2_Thread(void const *argument);
void Default_Thread(void const *argument);
void DSP_Thread(void const *argument);
void GUI_Thread(void const *argument);
void DL_Thread(void const *argument);

#endif /* APPLICATION_USER_RTOS_OSTHREADS_H_ */
