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
	uint8_t CB0, CB1, CB2, CB3, CB4;
	/* Multipage page */
	uint8_t MP;
} WM_ItemFlag;

WM_HWIN hMpage;
WM_HWIN hCB0, hCB1, hCB2,hCB_Uf, hCB_If;
WM_HWIN hB1;
WM_HWIN hMW, hMP0, hMP1, hMP2, hMP3;
WM_HWIN hText_URMS, hText_IRMS, hText_Freq, hText_PF, hText_DPF, hText_P, hText_Q, hText_S, hText_ZL;
WM_HWIN hText_Time, hText_Date, hText_Uf, hText_If, hText12;
WM_HWIN hGraph0, hGraph1;
WM_HWIN hDd1, hDd2, hDd_Uf, hDd_If;
WM_HWIN hDateTime;
WM_HWIN hCal;
WM_HWIN hHH, hMM;
CALENDAR_DATE EMWIN_date;

GRAPH_DATA_Handle hData_U, hData_I, hData_Uf, hData_If;
GRAPH_SCALE_Handle hScale0x, hScale0yU, hScale0yI, hScale1x, hScale1yU, hScale1yI;

/* Harmonic index in DropDown's on page 'Harmonics' */
uint8_t Uf_idx, If_idx;

WM_HWIN CreateMainWindow(void);
WM_HWIN CreateWaveforms(void);
WM_HWIN CreateHarmonics(void);
WM_HWIN CreateImpedances(void);
WM_HWIN CreateSettings(void);
WM_HWIN CreateSetDateTime(void);

#endif /* __WM_MYDEFINES_H_ */
