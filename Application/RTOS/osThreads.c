/*
 * osThreads.c
 *
 *  Created on: 06.04.2017
 *      Author: Kwarc
 */
#include <GUI/emwin_functions.h>
#include <Modules/datalogger.h>
#include <RTOS/cpu_utils.h>
#include <RTOS/osThreads.h>
#include "stm32f4xx_hal.h"
#include "main.h"
#include "DIALOG.h"
#include "MESSAGEBOX.h"

static void TimerCallback(void const *n)
{
  TouchScreen_Update();
}

void Default_Thread(void const *argument)
{
  (void) argument;

  uint32_t lastTick;
  osTimerId lcd_timer;

  /* Initialize USB-host & FAT-FS */
  USB.disk_connected = false;
  MX_USB_HOST_Init();
  MX_FATFS_Init();

  /* Initialize Necessary modules */
  EMWIN_Init();
  DSP_Init();
  DL_Init();
  DR_Init();

  /* Create Touch screen Timer */
  osTimerDef(TS_Timer, TimerCallback);
  lcd_timer =  osTimerCreate(osTimer(TS_Timer), osTimerPeriodic, (void *)0);
  /* Start the TS Timer */
  osTimerStart(lcd_timer, 40);

  osThreadResume(DSPThreadHandle);

  lastTick = osKernelSysTick();
  /* GUI background Task */
  for(;;)
  {
    GUI_Exec();
    osDelayUntil(&lastTick, 20);
  }
  /* Should never go here */
  osThreadTerminate(DEFThreadHandle);
}

void DSP_Thread(void const *argument)
{
  (void) argument;

  for(;;)
  {
    if(osSemaphoreWait(DSP_Semaphore, portMAX_DELAY) == osOK)
    {
      if(DSP_AutoselectBuffers() == true)	// If buffers are ready to calculations
      {

        // Filter both signals
        if(WM_ItemFlag.CB_FIR)
        {
          DSP_FIRFilter();
        }

        DSP_IncreaseADCBits();
        DSP_DeleteOffset();

        // Copy waveforms to display buffers
        arm_copy_f32(U.DSP_buffer, U.GRAPH_buffer, FFT_LENGTH);
        arm_copy_f32(I.DSP_buffer, I.GRAPH_buffer, FFT_LENGTH);

        if(DL.save_waveforms)
        {
          DL.save_waveforms = false;
          DL_SaveWaveforms();
        }

        DSP_CalcRMS();
        DSP_CalcFrequency();
        DSP_CalcFFT();
        DSP_CalcTHD();
        DSP_CalcPower();
        DSP_CalcPF();
        DSP_CalcDPF();
        DSP_CalcLoadImpedance();
        DSP_GetLoadCharacter();
        DSP_AverageValues(5);

        /* ----------------------------------------------------------------------
         ** Log calculated parameters
         ** ------------------------------------------------------------------- */
        if(DL.log_now && grid.data_averaged && WM_ItemFlag.CB_LOG)
        {
          DL.log_now = false;
          osSemaphoreRelease(DL_Semaphore);
        }

        osSemaphoreRelease(GUI_Semaphore);
      }
    }

  }
  /* Should never go here */
  osThreadTerminate(DSPThreadHandle);
}


void DL_Thread(void const *argument)
{
  (void) argument;

  for(;;)
  {
    if(osSemaphoreWait(DL_Semaphore, portMAX_DELAY) == osOK)
    {
      DL_LogToFile(&grid);
    }
  }
  /* Should never go here */
  osThreadTerminate(DLThreadHandle);
}

void DR_Thread(void const *argument)
{
  (void) argument;

  for(;;)
  {
    if(osSemaphoreWait(DR_Semaphore, portMAX_DELAY) == osOK)
    {
      FIL csv_file;
      FRESULT fr;

      fr = f_open(&csv_file, DR.filename, FA_READ);

      if(fr == FR_OK)
      {
    	WM_HWIN info = MESSAGEBOX_Create(" Please wait...", "Information", GUI_MESSAGEBOX_CF_MODAL);
        DR_ReadFFT(&csv_file, Zf_idx);
        DSP_CalcSourceImpedance();
        WM_DeleteWindow(info);
      }

      fr = f_close(&csv_file);
    }
  }
  /* Should never go here */
  osThreadTerminate(DRThreadHandle);
}

void GUI_Thread(void const *argument)
{
  (void) argument;

  uint8_t active_graph = 0;
  uint16_t i;
  char string[32];

  for(;;)
  {
    if(osSemaphoreWait(GUI_Semaphore, portMAX_DELAY ) == osOK)
    {
      active_graph ^= 1;
      page = MULTIPAGE_GetSelection(hMpage);

      if(DL.print_screen)
      {
        DL.print_screen = false;
        DL_PrintScreen();
      }

      /* ----------------------------------------------------------------------
       ** Draw signals in time domain
       ** ------------------------------------------------------------------- */
      if(page == 0)
      {
        if(active_graph == 0)
        {
          float32_t* p = DSP_GetBufferPointer(voltage);
          for(i=0;i<32;i++)
          {
            GRAPH_DATA_YT_AddValue(hData_U,GRAPH_YT_EMPTY_DATA);
          }
          for(;i<FFT_LENGTH+32;i++)
          {
            GRAPH_DATA_YT_AddValue(hData_U,(int16_t)p[i-32]/164);
          }
        }
        else if(active_graph == 1)
        {
          float32_t* p = DSP_GetBufferPointer(current);
          for(i=0;i<32;i++)
          {
            GRAPH_DATA_YT_AddValue(hData_I,GRAPH_YT_EMPTY_DATA);
          }
          for(;i<FFT_LENGTH+32;i++)
          {
            GRAPH_DATA_YT_AddValue(hData_I,(int16_t)p[i-32]/170);
          }
        }
      }

      /* ----------------------------------------------------------------------
       ** Draw signals in frequency domain
       ** ------------------------------------------------------------------- */
      if(page == 1)
      {
        WM_Paint(hGraph1);
        if(grid.data_averaged)
        {
          sprintf(string, "THD(U): %2.1f%%", grid.THD_voltage);
          TEXT_SetText(hTextTHD_U, string);
          sprintf(string, "THD(I): %2.1f%%", grid.THD_current);
          TEXT_SetText(hTextTHD_I, string);

          sprintf(string, "%3.1f V", U.FFT_out_real[GET_HARMONIC(Uf_idx)]);
          TEXT_SetText(hText_Uf, string);
          sprintf(string, "%2.2f A", I.FFT_out_real[GET_HARMONIC(If_idx)]);
          TEXT_SetText(hText_If, string);
        }
      }
      /* ----------------------------------------------------------------------
       ** Display calculated parameters
       ** ------------------------------------------------------------------- */
      if(page == 0 && grid.data_averaged)
      {

        sprintf(string, "%3.1f V", grid.RMS_voltage);
        TEXT_SetText(hText_URMS, string);
        sprintf(string, "%2.2f A", grid.RMS_current);
        TEXT_SetText(hText_IRMS, string);
        sprintf(string, "%2.2f Hz", grid.frequency);
        TEXT_SetText(hText_Freq, string);
        sprintf(string, "%.0f VA", grid.S);
        TEXT_SetText(hText_S, string);
        sprintf(string, "%.0f W", grid.P);
        TEXT_SetText(hText_P, string);
        sprintf(string, "%.0f VAR", grid.Q);
        TEXT_SetText(hText_Q, string);
        sprintf(string, "PF: %.2f", grid.PF);
        TEXT_SetText(hText_PF, string);
        sprintf(string, "DPF: %.2f", grid.DPF);
        TEXT_SetText(hText_DPF, string);

        /* Display load character */
        switch(grid.load_type)
        {
          case ind_load:
            TEXT_SetText(hText_Ztype,"inductive load (-)");
            break;
          case ind_generator:
            TEXT_SetText(hText_Ztype,"inductive load (+)");
            break;
          case cap_generator:
            TEXT_SetText(hText_Ztype,"capacitive load (+)");
            break;
          case cap_load:
            TEXT_SetText(hText_Ztype,"capacitive load (-)");
            break;
        }

      }

      if(page == 2 && grid.data_averaged)
      {
        sprintf(string, "%.2f %+.2fi Ohm", grid.src_impedance[0], grid.src_impedance[1]);
        TEXT_SetText(hText_ZS, string);

        sprintf(string, "%.2f %+.2fi Ohm", grid.load_impedance[0], grid.load_impedance[1]);
        TEXT_SetText(hText_ZL, string);
      }

      if(page == 3 && rtc_1sTick)
      {
        /* Get time Format : hh:mm:ss */
        sprintf(string, "%02d:%02d:%02d", rtc_time.Hours, rtc_time.Minutes, rtc_time.Seconds);
        /* Display time */
        TEXT_SetText(hText_Time, string);
        /* Get date Format : mm-dd-yy */
        sprintf(string, "%02d-%02d-%4d", rtc_date.Date, rtc_date.Month, 2000 + rtc_date.Year);
        /* Display date */
        TEXT_SetText(hText_Date, string);
        rtc_1sTick = 0;
      }

      sprintf(string, "Power Grid Monitor | CPU: %d%%", osGetCPUUsage());
      FRAMEWIN_SetText(hMW, string);
    }

  }
  /* Should never go here */
  osThreadTerminate(GUIThreadHandle);

}
