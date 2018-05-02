/*
 * FramewinDLG.h
 *
 *  Created on: 21 maj 2016
 *      Author: Kwarc
 */

#ifndef __WM_MYDEFINES_H_
#define __WM_MYDEFINES_H_

#include <stdio.h>
#include "DIALOG.h"

struct {
  /* Checkboxes states */
  uint8_t CB_FIR, CB_LOG, CB_WIN;
  /* Multipage page */
  uint8_t MP;
} WM_ItemFlag;

WM_HWIN hMW, hMP0, hMP1, hMP2, hMP3;
WM_HWIN hDateTime;
WM_HWIN hChooseFile;
WM_HWIN hCal;

MULTIPAGE_Handle hMpage;
CHECKBOX_Handle hCB_FIR, hCB_LOG, hCB_WIN;
BUTTON_Handle hB1;

TEXT_Handle hText_URMS, hText_IRMS, hText_Freq, hText_PF, hText_DPF, hText_P, hText_Q, hText_S, hText_Ztype;
TEXT_Handle hText_Time, hText_Date, hText_Uf, hText_If, hText12, hTextTHD_U, hTextTHD_I, hText_ZS, hText_ZL;

GRAPH_Handle hGraph0, hGraph1;
DROPDOWN_Handle hDd1, hDd2, hDd_Uf, hDd_If, hDd_Zf;

TREEVIEW_Handle hTreeView;
TREEVIEW_ITEM_Handle hMainDir;
TEXT_Handle hFileName;

SPINBOX_Handle hHH, hMM;
CALENDAR_DATE EMWIN_date;

GRAPH_DATA_Handle hData_U, hData_I, hData_Uf, hData_If;
GRAPH_SCALE_Handle hScale0x, hScale0yU, hScale0yI, hScale1x, hScale1yU, hScale1yI;

/* Harmonic index in DropDown's on page 'Harmonics' */
uint8_t Uf_idx, If_idx;

/* Harmonic index in DropDown's on page 'Impedances' */
uint8_t Zf_idx;

WM_HWIN CreateMainWindow(void);
WM_HWIN CreateWaveforms(void);
WM_HWIN CreateHarmonics(void);
WM_HWIN CreateImpedances(void);
WM_HWIN CreateSettings(void);
WM_HWIN CreateSetDateTime(void);
WM_HWIN CreateChooseFile(void);

#endif /* __WM_MYDEFINES_H_ */
