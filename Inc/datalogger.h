/*
 * datalogger.h
 *
 *  Created on: 22.10.2016
 *      Author: Kwarc
 */

#ifndef __DATALOGGER_H_
#define __DATALOGGER_H_

#include <stdbool.h>
#include "fatfs.h"
#include "rtc.h"
#include "usb_host.h"
#include "dsp_calculations.h"

/* FOR TEST ONLY */
volatile uint32_t write_time_ms;
volatile uint8_t timer_onoff;
/* ------------- */
struct datalogger_t
{
	FATFS USB_disk;        				/* File system object for USB disk logical drive */
	FIL file;							/* File objects */
	FRESULT res;						/* FatFs function common result code */
	char filename[13];					/* File name */
	volatile _Bool disk_connected;		/* Disc connected indicator flag */
	volatile _Bool print_screen;		/* Print screen indicator flag */
	uint8_t interval_div[4];			/* Divider used for timing in RTC interrupt */
	uint8_t interval_div_idx;			/* Index in interval_dic table */
	_Bool save_waveforms;
	_Bool log_now;
	char data_buffer[256];				/* 244 char's has total data */

}DL;

void DL_Init(void);
void DL_MountDisk(void);
void DL_UnmountDisk(void);
void DL_CreateFile(void);
void DL_CloseFile(void);
void DL_WriteTimeTest(void);
void DL_LogToFile(struct parameters_t *grid);
void DL_PrintScreen(void);
void DL_SaveWaveforms(void);
void DL_TestApplication(void);
void DL_WriteTimeTest(void);

#endif /* __DATALOGGER_H_ */
