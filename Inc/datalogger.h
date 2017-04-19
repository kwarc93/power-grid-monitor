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

/* ------------- */
struct disk_t
{
	FATFS disk;        						/* File system object for USB disk logical drive */
	volatile _Bool disk_connected;			/* Disc connected indicator flag */
}USB;
/* ------------- */
struct datalogger_t
{
	FIL file;							/* File objects */
	FRESULT res;						/* FatFs function common result code */
	char filename[13];					/* File name */
	volatile _Bool print_screen;		/* Print screen indicator flag */
	uint8_t interval_div[4];			/* Divider used for timing in RTC interrupt */
	uint8_t interval_div_idx;			/* Index in interval_dic table */
	_Bool save_waveforms;
	_Bool log_now;
	char data_buffer[512];

}DL;
/* ---------- */
#define START_COLUMN	(6)
#define OFFSET			(17)
struct datareader_t
{
	FIL file;							/* File objects */
	FRESULT res;						/* FatFs function common result code */
	char filename[13];					/* File name */
	char data_buffer[512];
	_Bool data_alloc;
	char delimiter;
	uint32_t lines_nr;
	float32_t *FFT_U, *FFT_I;			/* Arrays of complex numbers (real, imag, real, imag, ...) */
}DR;
/* ---------- */

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

void DR_Init(void);
void DR_GetNumberOfLines(FIL *file, uint32_t *n);
void DR_ReadFFT(FIL *file, uint8_t harmonic);
#endif /* __DATALOGGER_H_ */
