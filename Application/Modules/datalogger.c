/*
 * datalogger.c
 *
 *  Created on: 22.10.2016
 *      Author: Kwarc
 */

#include <Modules/datalogger.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "DIALOG.h"
#include "MESSAGEBOX.h"
#include "tlsf/tlsf.h"

extern void Error_Handler(void);
extern uint8_t pga_gain;

struct datareader_t DR;
struct datalogger_t DL;
struct disk_t USB;

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

const char *sprintfFormat = "%02d:%02d:%02d,%2.2f,%1.2f,%4.0f,%4.0f,%4.0f,"
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
/* ----------------------------------------------------------------------------
 * Datalogger functions
 * ----------------------------------------------------------------------------
 */
void DL_Init(void)
{
  /* Initialize variables */
  DL.print_screen = false;
  DL.interval_div[0]=1;DL.interval_div[1]=60;DL.interval_div[2]=120;DL.interval_div[3]=240;
  DL.interval_div_idx = 1;
  DL.save_waveforms = false;
  DL.log_now = false;
  rtc_1sTick = 1;
}
void DL_MountDisk(void)
{
  /* Register the file system object to the FatFs module */
  if(f_mount(&USB.disk, (TCHAR const*)USBH_Path, 1) != FR_OK)
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

  cnt++;


}

void DL_CloseFile(void)
{
  /* Close the opened file */
  if(f_close(&DL.file) != FR_OK)
  {
    Error_Handler();
  }
  /* Show information on success */
  MESSAGEBOX_Create(" Log saved!", "Information", GUI_MESSAGEBOX_CF_MODAL);
}

void DL_LogToFile(struct parameters_t *grid)
{
  const uint8_t harmonic_r = CFFT_50HZ_BIN_R;
  const uint8_t harmonic_i = CFFT_50HZ_BIN_I;

  /* Reset buffer */
  memset(DL.data_buffer, 0, sizeof(DL.data_buffer));
  /* Convert numeric data to string */
  sprintf(DL.data_buffer, sprintfFormat,rtc_time.Hours, rtc_time.Minutes, rtc_time.Seconds,
          grid->frequency,grid->PF,grid->S,grid->P,grid->Q,
          /* Real part of FFT voltage harmonics */
          U.FFT_out_cmplx[harmonic_r*0],U.FFT_out_cmplx[harmonic_r*1],
          U.FFT_out_cmplx[harmonic_r*2],U.FFT_out_cmplx[harmonic_r*3],
          U.FFT_out_cmplx[harmonic_r*4],U.FFT_out_cmplx[harmonic_r*5],
          U.FFT_out_cmplx[harmonic_r*6],U.FFT_out_cmplx[harmonic_r*7],
          U.FFT_out_cmplx[harmonic_r*8],U.FFT_out_cmplx[harmonic_r*9],
          U.FFT_out_cmplx[harmonic_r*10],U.FFT_out_cmplx[harmonic_r*11],
          U.FFT_out_cmplx[harmonic_r*12],U.FFT_out_cmplx[harmonic_r*13],
          U.FFT_out_cmplx[harmonic_r*14],U.FFT_out_cmplx[harmonic_r*15],
          U.FFT_out_cmplx[harmonic_r*16],
          /* Imaginary part of FFT voltage harmonics */
          U.FFT_out_cmplx[harmonic_i*0],U.FFT_out_cmplx[harmonic_i*1],
          U.FFT_out_cmplx[harmonic_i*2],U.FFT_out_cmplx[harmonic_i*3],
          U.FFT_out_cmplx[harmonic_i*4],U.FFT_out_cmplx[harmonic_i*5],
          U.FFT_out_cmplx[harmonic_i*6],U.FFT_out_cmplx[harmonic_i*7],
          U.FFT_out_cmplx[harmonic_i*8],U.FFT_out_cmplx[harmonic_i*9],
          U.FFT_out_cmplx[harmonic_i*10],U.FFT_out_cmplx[harmonic_i*11],
          U.FFT_out_cmplx[harmonic_i*12],U.FFT_out_cmplx[harmonic_i*13],
          U.FFT_out_cmplx[harmonic_i*14],U.FFT_out_cmplx[harmonic_i*15],
          U.FFT_out_cmplx[harmonic_i*16],
          /* Real part of FFT current harmonics */
          I.FFT_out_cmplx[harmonic_r*0],I.FFT_out_cmplx[harmonic_r*1],
          I.FFT_out_cmplx[harmonic_r*2],I.FFT_out_cmplx[harmonic_r*3],
          I.FFT_out_cmplx[harmonic_r*4],I.FFT_out_cmplx[harmonic_r*5],
          I.FFT_out_cmplx[harmonic_r*6],I.FFT_out_cmplx[harmonic_r*7],
          I.FFT_out_cmplx[harmonic_r*8],I.FFT_out_cmplx[harmonic_r*9],
          I.FFT_out_cmplx[harmonic_r*10],I.FFT_out_cmplx[harmonic_r*11],
          I.FFT_out_cmplx[harmonic_r*12],I.FFT_out_cmplx[harmonic_r*13],
          I.FFT_out_cmplx[harmonic_r*14],I.FFT_out_cmplx[harmonic_r*15],
          I.FFT_out_cmplx[harmonic_r*16],
          /* Imaginary part of FFT current harmonics */
          I.FFT_out_cmplx[harmonic_i*0],I.FFT_out_cmplx[harmonic_i*1],
          I.FFT_out_cmplx[harmonic_i*2],I.FFT_out_cmplx[harmonic_i*3],
          I.FFT_out_cmplx[harmonic_i*4],I.FFT_out_cmplx[harmonic_i*5],
          I.FFT_out_cmplx[harmonic_i*6],I.FFT_out_cmplx[harmonic_i*7],
          I.FFT_out_cmplx[harmonic_i*8],I.FFT_out_cmplx[harmonic_i*9],
          I.FFT_out_cmplx[harmonic_i*10],I.FFT_out_cmplx[harmonic_i*11],
          I.FFT_out_cmplx[harmonic_i*12],I.FFT_out_cmplx[harmonic_i*13],
          I.FFT_out_cmplx[harmonic_i*14],I.FFT_out_cmplx[harmonic_i*15],
          I.FFT_out_cmplx[harmonic_i*16]);
  /* Save data to file */
  f_write(&DL.file, DL.data_buffer, strlen(DL.data_buffer), NULL);
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
    if(USB.disk_connected)
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
/* ----------------------------------------------------------------------------
 * Datareader functions
 * ----------------------------------------------------------------------------
 */
void DR_Init(void)
{
  DR.delimiter = ',';
  DR.data_alloc = false;
  DR.scan_files = false;
  DR.lines_nr = 0;
}

void DR_GetNumberOfLines(FIL *file, uint32_t *lines_count)
{
  char c;
  UINT br;
  uint32_t lines = 0;

  f_lseek(file,0);

  do
  {
    f_read(file, &c, 1U, &br);
    if(c == '\n')  lines++;

  } while (!f_eof(file));

  if(c != '\n' && lines != 0)   lines++;

  *lines_count = lines;

}


void DR_ReadFFT(FIL *file, uint8_t harmonic)
{
  char *line_pointer;
  char *record;
  uint8_t start_col;

  // ------------------- free data --------------------- //
  if(DR.data_alloc == true)
  {
    tlsf_free(DR.FFT_U);
    DR.FFT_U = NULL;
    tlsf_free(DR.FFT_I);
    DR.FFT_I = NULL;
    DR.data_alloc = false;
  }
  // --------------------------------------------------- //

  DR_GetNumberOfLines(file, &DR.lines_nr);
  DR.lines_nr -= 1; // Substract header line

  // ------------------ alloc data --------------------- //
  if(DR.data_alloc == false)
  {
    DR.FFT_U=(float32_t*)tlsf_malloc(2*DR.lines_nr*sizeof(float32_t));
    if (DR.FFT_U == NULL) {
      return;
    }
    DR.FFT_I=(float32_t*)tlsf_malloc(2*DR.lines_nr*sizeof(float32_t));
    if (DR.FFT_I == NULL) {
      return;
    }
    DR.data_alloc = true;
  }
  // --------------------------------------------------- //
  // Skip first line (header is longer than 512 bytes)
  // --------------------------------------------------- //
  f_lseek(file,0);
  char c;
  UINT br;
  do
  {
    f_read(file, &c, 1U, &br);
  }
  while (c != '\n');
  // ------------------------------------------ //

  for(uint32_t row = 0; row < 2*DR.lines_nr; row+=2)
  {
    line_pointer=f_gets(DR.data_buffer,sizeof(DR.data_buffer),file);
    record = strtok(line_pointer, &DR.delimiter);

    // ----------------- VOLTAGE ----------------- //
    // If you want to start parsing from specified n-th column, call this 2 lines n-times
    start_col = START_COLUMN+harmonic;
    while(start_col--)record = strtok(NULL, &DR.delimiter);
    // Real part
    DR.FFT_U[row] = atof(record);
    start_col = OFFSET;
    while(start_col--)record = strtok(NULL, &DR.delimiter);
    // Imaginary part
    DR.FFT_U[row+1] = atof(record);

    // ------------------ CURRENT --------------- //
    start_col = OFFSET;
    while(start_col--)record = strtok(NULL, &DR.delimiter);
    // Real part
    DR.FFT_I[row] = atof(record);
    start_col = OFFSET;
    while(start_col--)record = strtok(NULL, &DR.delimiter);
    // Imaginary part
    DR.FFT_I[row+1] = atof(record);
    // ------------------------------------------ //
  }
}

static FRESULT GetCSVFilesCount(char* path, uint8_t* files_count)
{
  FRESULT res;
  DIR dir;
  FILINFO fno;
  uint8_t fcount = 0;

  /* Open the directory */
  res = f_opendir(&dir, path);

  if (res == FR_OK)
  {
    for (;;)
    {
      /* Read a directory item */
      res = f_readdir(&dir, &fno);
      /* Break on error or end of dir */
      if (res != FR_OK || fno.fname[0] == 0) break;
      /* If file name contains 'LOG' */
      if(strstr(fno.fname, "LOG") != NULL) fcount += 1;
    }
  }
  *files_count = fcount;
  res = f_closedir(&dir);
  return res;
}

static FRESULT DR_ListFiles(char* path, FILINFO* file)
{
  FRESULT res;
  DIR dir;
  FILINFO fno;
  uint8_t idx = 0;

  /* Open the directory */
  res = f_opendir(&dir, path);

  if (res == FR_OK)
  {
    for (;;)
    {
      /* Read a directory item */
      res = f_readdir(&dir, &fno);
      /* Break on error or end of dir */
      if (res != FR_OK || fno.fname[0] == 0) break;
      /* If file name contains 'LOG' */
      if(strstr(fno.fname, "LOG") != NULL) file[idx++] = fno;
    }
  }
  res = f_closedir(&dir);
  return res;
}

FRESULT DR_ScanFilesOnDisk(void)
{

  // ------------------- free data --------------------- //
  if(DR.scan_files == true)
  {
    tlsf_free(DR.LOG_files);
    DR.LOG_files = NULL;
    DR.scan_files = false;
  }
  // --------------------------------------------------- //

  /* Get number of LOG-xx.csv files in current directory */
  FRESULT  fr;
  fr = GetCSVFilesCount(".", &DR.files_count);
  if(fr != FR_OK) return fr;

  // ------------------ alloc data --------------------- //
  if(DR.scan_files == false)
  {
    DR.LOG_files = (FILINFO *) tlsf_malloc(DR.files_count * sizeof(FILINFO));
    if(DR.LOG_files == NULL)
    {
      Error_Handler();
    }
    DR.scan_files = true;
  }
  // --------------------------------------------------- //

  /* Save info of scanned LOG-xx.csv files in array */
  fr = DR_ListFiles(".", DR.LOG_files);

  return fr;
}
