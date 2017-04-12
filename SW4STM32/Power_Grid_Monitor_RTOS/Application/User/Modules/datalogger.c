/*
 * datalogger.c
 *
 *  Created on: 22.10.2016
 *      Author: Kwarc
 */

#include <stdio.h>
#include "datalogger.h"
#include "DIALOG.h"
#include "MESSAGEBOX.h"

extern void Error_Handler(void);
extern uint8_t pga_gain;

const char *fileHeader = "Time,Frequency,Power Factor,Apparent Power,Real Power,Reactive Power,"
										"U_FFT_R[0],U_FFT_R[1],U_FFT_R[2],U_FFT_R[3],U_FFT_R[4],"
										"U_FFT_R[5],U_FFT_R[6],U_FFT_R[7],U_FFT_R[8],"
										"U_FFT_R[9],U_FFT_R[10],U_FFT_R[11],U_FFT_R[12],"
										"U_FFT_R[13],U_FFT_R[14],U_FFT_R[15],U_FFT_R[16],"
										"U_FFT_I[0],U_FFT_I[1],U_FFT_I[2],U_FFT_I[3],U_FFT_I[4],"
										"U_FFT_I[5],U_FFT_I[6],U_FFT_I[7],U_FFT_I[8],"
										"U_FFT_I[9],U_FFT_I[10],U_FFT_I[11],U_FFT_I[12],"
										"U_FFT_I[13],U_FFT_I[14],U_FFT_I[15],U_FFT_I[16],"

										"I_FFT_R[0],I_FFT_R[1],I_FFT_R[2],I_FFT_R[3],I_FFT_R[4],"
										"I_FFT_R[5],I_FFT_R[6],I_FFT_R[7],I_FFT_R[8],"
										"I_FFT_R[9],I_FFT_R[10],I_FFT_R[11],I_FFT_R[12],"
										"I_FFT_R[13],I_FFT_R[14],I_FFT_R[15],I_FFT_R[16],"
										"I_FFT_I[0],I_FFT_I[1],I_FFT_I[2],I_FFT_I[3],I_FFT_I[4],"
										"I_FFT_I[5],I_FFT_I[6],I_FFT_I[7],I_FFT_I[8],"
										"I_FFT_I[9],I_FFT_I[10],I_FFT_I[11],I_FFT_I[12],"
										"I_FFT_I[13],I_FFT_I[14],I_FFT_I[15],I_FFT_I[16]\r\n";

const char *sprintfFormat = "%02d:%02d:%02d,%2.2f,%1.2f,%+4.0f,%+4.0f,%+4.0f,"
							"%3.1f,%3.1f,%3.1f,%3.1f,%3.1f,%3.1f,%3.1f,%3.1f,"
							"%3.1f,%3.1f,%3.1f,%3.1f,%3.1f,%3.1f,%3.1f,%3.1f,%3.1f,"
							"%3.1f,%3.1f,%3.1f,%3.1f,%3.1f,%3.1f,%3.1f,%3.1f,"
							"%3.1f,%3.1f,%3.1f,%3.1f,%3.1f,%3.1f,%3.1f,%3.1f,%3.1f,"

							"%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,"
							"%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,"
							"%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,"
							"%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f\r\n";

static void WriteByte2File(uint8_t data, void * file)
{
	UINT bWritten;
	/* Save data to file */
	f_write(file, &data, 1, &bWritten);
}

void DL_Init(void)
{
	/* Initialize variables */
	DL.disk_connected = false;
	DL.print_screen = false;
	DL.interval_div[0]=1;DL.interval_div[1]=60;DL.interval_div[2]=120;DL.interval_div[3]=240;
	DL.interval_div_idx = 1;
	DL.save_waveforms = false;
	DL.log_now = false;
	rtc_1sTick = 1;

	/* Initialize USB-host & FAT-FS */
	MX_USB_HOST_Init();
	MX_FATFS_Init();
}
void DL_MountDisk(void)
{
	/* Register the file system object to the FatFs module */
	if(f_mount(&DL.USB_disk, (TCHAR const*)USBH_Path, 1) != FR_OK)
	{
	/* FatFs Initialization Error */
	Error_Handler();
	}
	/* Create and get in directory for files created by device */
    f_mkdir("PGM");
    f_chdir("/PGM");
}

void DL_UnmountDisk(void)
{
	/* Register the file system object to the FatFs module */
	if(f_mount(NULL, (TCHAR const*)USBH_Path, 1) != FR_OK)
	{
	/* FatFs De-initialization Error */
	Error_Handler();
	}
}

void DL_CreateFile(void)
{
	static uint8_t cnt = 0;

	/* Convert actual date to filename string */
	sprintf(DL.filename, "log-%02d.csv", cnt);

    /* Create and Open a new .csv file object with write access */
    if(f_open(&DL.file, DL.filename, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
    {
      /* file open for write Error */
      Error_Handler();
    }

	/* Create header file for CSV */
	f_puts(fileHeader, &DL.file);

	/* Go to the end of file for future writing */
	f_lseek(&DL.file, f_size(&DL.file));

	cnt++;


}

void DL_CloseFile(void)
{
    /* Close the opened file */
    if(f_close(&DL.file) != FR_OK)
    {
    	Error_Handler();
    }
}

void DL_LogToFile(struct parameters_t *grid)
{

	/* Reset buffer */
	memset(DL.data_buffer, 0, sizeof(DL.data_buffer));
	/* Convert numeric data to string */
	sprintf(DL.data_buffer, sprintfFormat,rtc_time.Hours, rtc_time.Minutes, rtc_time.Seconds,
										grid->frequency,grid->PF,grid->S,grid->P,grid->Q,
										/* Real part of FFT voltage harmonics */
										U.FFT_out_cmplx[GET_HARMONIC_R(0)],U.FFT_out_cmplx[GET_HARMONIC_R(1)],
										U.FFT_out_cmplx[GET_HARMONIC_R(2)],U.FFT_out_cmplx[GET_HARMONIC_R(3)],
										U.FFT_out_cmplx[GET_HARMONIC_R(4)],U.FFT_out_cmplx[GET_HARMONIC_R(5)],
										U.FFT_out_cmplx[GET_HARMONIC_R(6)],U.FFT_out_cmplx[GET_HARMONIC_R(7)],
										U.FFT_out_cmplx[GET_HARMONIC_R(8)],U.FFT_out_cmplx[GET_HARMONIC_R(9)],
										U.FFT_out_cmplx[GET_HARMONIC_R(10)],U.FFT_out_cmplx[GET_HARMONIC_R(11)],
										U.FFT_out_cmplx[GET_HARMONIC_R(12)],U.FFT_out_cmplx[GET_HARMONIC_R(13)],
										U.FFT_out_cmplx[GET_HARMONIC_R(14)],U.FFT_out_cmplx[GET_HARMONIC_R(15)],
										U.FFT_out_cmplx[GET_HARMONIC_R(16)],
										/* Imaginary part of FFT voltage harmonics */
										U.FFT_out_cmplx[GET_HARMONIC_I(0)],U.FFT_out_cmplx[GET_HARMONIC_I(1)],
										U.FFT_out_cmplx[GET_HARMONIC_I(2)],U.FFT_out_cmplx[GET_HARMONIC_I(3)],
										U.FFT_out_cmplx[GET_HARMONIC_I(4)],U.FFT_out_cmplx[GET_HARMONIC_I(5)],
										U.FFT_out_cmplx[GET_HARMONIC_I(6)],U.FFT_out_cmplx[GET_HARMONIC_I(7)],
										U.FFT_out_cmplx[GET_HARMONIC_I(8)],U.FFT_out_cmplx[GET_HARMONIC_I(9)],
										U.FFT_out_cmplx[GET_HARMONIC_I(10)],U.FFT_out_cmplx[GET_HARMONIC_I(11)],
										U.FFT_out_cmplx[GET_HARMONIC_I(12)],U.FFT_out_cmplx[GET_HARMONIC_I(13)],
										U.FFT_out_cmplx[GET_HARMONIC_I(14)],U.FFT_out_cmplx[GET_HARMONIC_I(15)],
										U.FFT_out_cmplx[GET_HARMONIC_I(16)],
										/* Real part of FFT current harmonics */
										I.FFT_out_cmplx[GET_HARMONIC_R(0)],I.FFT_out_cmplx[GET_HARMONIC_R(1)],
										I.FFT_out_cmplx[GET_HARMONIC_R(2)],I.FFT_out_cmplx[GET_HARMONIC_R(3)],
										I.FFT_out_cmplx[GET_HARMONIC_R(4)],I.FFT_out_cmplx[GET_HARMONIC_R(5)],
										I.FFT_out_cmplx[GET_HARMONIC_R(6)],I.FFT_out_cmplx[GET_HARMONIC_R(7)],
										I.FFT_out_cmplx[GET_HARMONIC_R(8)],I.FFT_out_cmplx[GET_HARMONIC_R(9)],
										I.FFT_out_cmplx[GET_HARMONIC_R(10)],I.FFT_out_cmplx[GET_HARMONIC_R(11)],
										I.FFT_out_cmplx[GET_HARMONIC_R(12)],I.FFT_out_cmplx[GET_HARMONIC_R(13)],
										I.FFT_out_cmplx[GET_HARMONIC_R(14)],I.FFT_out_cmplx[GET_HARMONIC_R(15)],
										I.FFT_out_cmplx[GET_HARMONIC_R(16)],
										/* Imaginary part of FFT current harmonics */
										I.FFT_out_cmplx[GET_HARMONIC_I(0)],I.FFT_out_cmplx[GET_HARMONIC_I(1)],
										I.FFT_out_cmplx[GET_HARMONIC_I(2)],I.FFT_out_cmplx[GET_HARMONIC_I(3)],
										I.FFT_out_cmplx[GET_HARMONIC_I(4)],I.FFT_out_cmplx[GET_HARMONIC_I(5)],
										I.FFT_out_cmplx[GET_HARMONIC_I(6)],I.FFT_out_cmplx[GET_HARMONIC_I(7)],
										I.FFT_out_cmplx[GET_HARMONIC_I(8)],I.FFT_out_cmplx[GET_HARMONIC_I(9)],
										I.FFT_out_cmplx[GET_HARMONIC_I(10)],I.FFT_out_cmplx[GET_HARMONIC_I(11)],
										I.FFT_out_cmplx[GET_HARMONIC_I(12)],I.FFT_out_cmplx[GET_HARMONIC_I(13)],
										I.FFT_out_cmplx[GET_HARMONIC_I(14)],I.FFT_out_cmplx[GET_HARMONIC_I(15)],
										I.FFT_out_cmplx[GET_HARMONIC_I(16)]);
	/* Save data to file */
	f_write(&DL.file, DL.data_buffer, sizeof(DL.data_buffer), NULL);
	if(rtc_1sTick)	f_sync(&DL.file);

}

void DL_PrintScreen(void)
{
	FIL bmpFile;
	char filname[13];
	static uint8_t cnt = 0;

	/* Create filename with counter */
	sprintf(filname, "screen%02d.bmp", cnt);
    /* Create and Open a new .bmp file object with write access */
    if(f_open(&bmpFile, filname, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
    {
      /* file open for write Error */
      Error_Handler();
    }

	GUI_BMP_Serialize(WriteByte2File, &bmpFile);

    /* Close the opened file */
    if(f_close(&bmpFile) != FR_OK)
    {
    	Error_Handler();
    }
    MESSAGEBOX_Create(" Screenshot saved!", "Information", GUI_MESSAGEBOX_CF_MODAL);
    cnt++;
}

void DL_SaveWaveforms(void)
{
	FIL file;
	char buffer[16];
	static uint8_t cnt = 0;

	/* Create filename with counter */
	sprintf(buffer, "wf%02d.csv", cnt);
	/* Create and Open a new .bmp file object with write access */
	if(f_open(&file, buffer, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
	{
		/* file open for write Error */
		Error_Handler();
	}

	/* Create header file for csv */
	f_puts("Voltage,Current\r\n", &file);
	/* Go to the end of file for future writing */
	f_lseek(&file, f_size(&file));

	float32_t* p1 = DSP_GetBufferPointer(voltage);
	float32_t* p2 = DSP_GetBufferPointer(current);

	for(uint16_t i=0;i<FFT_LENGTH;i++)
	{
		sprintf(buffer, "%3.1f,%2.2f\r\n", (p1[i]*PEAK_VOLTAGE_RANGE)/(float32_t)ADC_RESOLUTION,
										   (p2[i]*PEAK_CURRENT_RANGE)/(float32_t)(ADC_RESOLUTION*pga_gain));
		f_puts(buffer,&file);
	}

	/* Close the opened file */
	if(f_close(&file) != FR_OK)
	{
		Error_Handler();
	}
    MESSAGEBOX_Create(" Waveforms saved!", "Information", GUI_MESSAGEBOX_CF_MODAL);
	cnt++;

}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == B1_Pin)
	{
		if(DL.disk_connected)
		{
			DL.print_screen = true;
		}
	}
}

void DL_TestApplication(void)
{
	FIL MyFile;                   						/* File object */
	FRESULT res;                                          /* FatFs function common result code */
	uint32_t byteswritten, bytesread;                     /* File write/read counts */
	uint8_t wtext[] = "This is STM32 working with FatFs"; /* File write buffer */
	uint8_t rtext[100];                                   /* File read buffer */


	/* Create and Open a new text file object with write access */
	if(f_open(&MyFile, "STM32.TXT", FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
	{
		/* 'STM32.TXT' file Open for write Error */
		Error_Handler();
	}
	else
	{
		/* Write data to the text file */
		res = f_write(&MyFile, wtext, sizeof(wtext), (void *)&byteswritten);

		if((byteswritten == 0) || (res != FR_OK))
		{
			/* 'STM32.TXT' file Write or EOF Error */
			Error_Handler();
		}
		else
		{
			/* Close the open text file */
			f_close(&MyFile);

			/* Open the text file object with read access */
			if(f_open(&MyFile, "STM32.TXT", FA_READ) != FR_OK)
			{
				/* 'STM32.TXT' file Open for read Error */
				Error_Handler();
			}
			else
			{
				/* Read data from the text file */
				res = f_read(&MyFile, rtext, sizeof(rtext), (void *)&bytesread);

				if((bytesread == 0) || (res != FR_OK))
				{
					/* 'STM32.TXT' file Read or EOF Error */
					Error_Handler();
				}
				else
				{
					/* Close the open text file */
					f_close(&MyFile);

					/* Compare read data with the expected data */
					if((bytesread != byteswritten))
					{
						/* Read data is different from the expected data */
						Error_Handler();
					}
					else
					{
						/* Success of the demo: no error occurrence */

					}
				}
			}
		}
	}
}
