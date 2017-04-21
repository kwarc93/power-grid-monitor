/*
 * gui_functions.c
 *
 *  Created on: 11.12.2016
 *      Author: Kwarc
 */

#include "emwin_functions.h"
#include "dsp_calculations.h"
GUI_BITMAP bmagh_r;

static void _UserDrawGraph(WM_HWIN hWin, int Stage)
{
	int16_t y_value_U, y_value_I, x_value;
	float32_t* pU = DSP_GetBufferPointer(voltageFFT);
	float32_t* pI = DSP_GetBufferPointer(currentFFT);

	switch (Stage) {
	case GRAPH_DRAW_FIRST:
		break;
	case GRAPH_DRAW_AFTER_BORDER:

		for(uint16_t idx = 0; idx < FFT_LENGTH/4 + 4*RFFT_50HZ_BIN; idx+=RFFT_50HZ_BIN)
		{
			x_value = 4*idx/5;

			/* Draw Voltage FFT */
			GUI_SetColor(GUI_LIGHTCYAN);
			y_value_U = 98 - (int16_t)pU[idx]/3U;
			GUI_DrawVLine(x_value + 33,y_value_U,98);
			GUI_DrawVLine(x_value + 34,y_value_U,98);
			/* Draw Current FFT */
			GUI_SetColor(GUI_YELLOW);
			y_value_I = (int16_t)(98 - 3.75f*pga_gain*pI[idx]);
			GUI_DrawVLine(x_value + 35,y_value_I,98);
			GUI_DrawVLine(x_value + 36,y_value_I,98);
		}

		break;
	case GRAPH_DRAW_LAST:
		break;
	}
}

void EMWIN_Init(void)
{

	/* Initialize the Touch screen */
	BSP_TS_Init(240, 320);
	/* Initializes the SDRAM device */
	BSP_SDRAM_Init();

	/* Init the STemWin GUI Library */
	GUI_Init();
	GUI_X_InitOS();
	GUI_Initialized = 1;

	/* Activate the use of memory device feature */
	WM_SetCreateFlags(WM_CF_MEMDEV);

	/* Show welcome screen */
	GUI_SetBkColor(GUI_WHITE);
	GUI_Clear();
	GUI_DrawBitmap(&bmagh_r, (240-bmagh_r.XSize)/2, (320-bmagh_r.YSize)/2);

	/* Window Manager Widgets Init */
	// MAIN WINDOW
	hMW = CreateMainWindow();
	// WAVEFORMS TAB
	hMP0 = CreateWaveforms();
	// HARMONICS TAB
	hMP1 = CreateHarmonics();
	// IMPEDANCES TAB
	hMP2 = CreateImpedances();
	// SETTINGS TAB
	hMP3 = CreateSettings();

	MULTIPAGE_AttachWindow(hMpage,0,hMP0);
	MULTIPAGE_AttachWindow(hMpage,1,hMP1);
	MULTIPAGE_AttachWindow(hMpage,2,hMP2);
	MULTIPAGE_AttachWindow(hMpage,3,hMP3);

/* ------------------------------------ Waveforms Graph Initialization ------------------------------------- */
	/* Preapare data(U(t)) for Graph0 */
	hData_U = GRAPH_DATA_YT_Create(GUI_LIGHTCYAN, FFT_LENGTH/4 + 32, 0, 0);
	GRAPH_AttachData(hGraph0, hData_U);
	GRAPH_DATA_YT_SetOffY(hData_U, 60);
	GRAPH_DATA_YT_SetAlign(hData_U,GRAPH_ALIGN_LEFT);

	/* Preapare data(I(t)) for Graph0 */
	hData_I = GRAPH_DATA_YT_Create(GUI_YELLOW, FFT_LENGTH/4 + 32, 0, 0);
	GRAPH_AttachData(hGraph0, hData_I);
	GRAPH_DATA_YT_SetOffY(hData_I, 60);
	GRAPH_DATA_YT_SetAlign(hData_I,GRAPH_ALIGN_LEFT);

	/* Prepare time-scale to Graph0 */
	hScale0x =  GRAPH_SCALE_Create(114, GUI_TA_HCENTER | GUI_TA_VCENTER, GRAPH_SCALE_CF_HORIZONTAL, TIME_SCALE_TICK);
	GRAPH_AttachScale(hGraph0, hScale0x);
	GRAPH_SCALE_SetFactor(hScale0x,0.390625f);
	GRAPH_SCALE_SetTextColor(hScale0x, GUI_WHITE);
	GRAPH_SCALE_SetOff(hScale0x,TIME_SCALE_TICK);
	GRAPH_SetGridDistX(hGraph0,TIME_SCALE_TICK);
	GRAPH_SCALE_SetTickDist(hScale0x,TIME_SCALE_TICK*2);
//	GRAPH_SetLineStyleV(hGraph0, GUI_LS_DOT);
	/* Prepare voltage-scale to Graph0 */
	hScale0yU =  GRAPH_SCALE_Create(30, GUI_TA_RIGHT, GRAPH_SCALE_CF_VERTICAL, VERTICAL_SCALE_TICK);
	GRAPH_AttachScale(hGraph0, hScale0yU);
	GRAPH_SCALE_SetOff(hScale0yU,59);
	GRAPH_SCALE_SetFactor(hScale0yU,7.5f);
	GRAPH_SCALE_SetTextColor(hScale0yU, GUI_LIGHTCYAN);
	GRAPH_SetGridOffY(hGraph0,VERTICAL_SCALE_TICK);
	GRAPH_SetGridDistY(hGraph0, VERTICAL_SCALE_TICK);
//	GRAPH_SetLineStyleH(hGraph0, GUI_LS_DOT);
	/* Prepare current-scale to Graph0 */
	hScale0yI =  GRAPH_SCALE_Create(164, GUI_TA_LEFT, GRAPH_SCALE_CF_VERTICAL, VERTICAL_SCALE_TICK);
	GRAPH_AttachScale(hGraph0, hScale0yI);
	GRAPH_SCALE_SetOff(hScale0yI,59);
	GRAPH_SCALE_SetFactor(hScale0yI,0.632f);
	GRAPH_SCALE_SetTextColor(hScale0yI, GUI_YELLOW);
	/* Set scales visible */
	GRAPH_SetGridVis(hGraph0,1);
/* ------------------------------------------------------------------------------------------------------- */
/* ------------------------------------ Harmonics Graph Initialization ------------------------------------- */

	/* Prepare freq-scale to Graph1 */
	hScale1x =  GRAPH_SCALE_Create(114, GUI_TA_HCENTER | GUI_TA_VCENTER, GRAPH_SCALE_CF_HORIZONTAL, FREQ_SCALE_TICK);
	GRAPH_AttachScale(hGraph1, hScale1x);
	GRAPH_SCALE_SetFactor(hScale1x,0.128f);
	GRAPH_SCALE_SetTextColor(hScale1x, GUI_WHITE);
	GRAPH_SCALE_SetOff(hScale1x,4*FREQ_SCALE_TICK);
	GRAPH_SetGridDistX(hGraph1,FREQ_SCALE_TICK);
	GRAPH_SCALE_SetTickDist(hScale1x, 4*FREQ_SCALE_TICK);
//	GRAPH_SetLineStyleV(hGraph0, GUI_LS_DOT);
	/* Prepare voltage-scale to graph1 */
	hScale1yU =  GRAPH_SCALE_Create(28, GUI_TA_RIGHT, GRAPH_SCALE_CF_VERTICAL, VERTICAL_SCALE_TICK);
	GRAPH_AttachScale(hGraph1, hScale1yU);
	GRAPH_SCALE_SetFactor(hScale1yU,2.875f);
	GRAPH_SCALE_SetTextColor(hScale1yU, GUI_LIGHTCYAN);
	GRAPH_SCALE_SetOff(hScale1yU,VERTICAL_SCALE_TICK-1);
	GRAPH_SetGridOffY(hGraph1,VERTICAL_SCALE_TICK+1);
	GRAPH_SetGridDistY(hGraph1, VERTICAL_SCALE_TICK);
//	GRAPH_SetLineStyleH(hGraph0, GUI_LS_DOT);
	/* Prepare current-scale to graph1 */
	hScale1yI =  GRAPH_SCALE_Create(166, GUI_TA_LEFT, GRAPH_SCALE_CF_VERTICAL, VERTICAL_SCALE_TICK);
	GRAPH_AttachScale(hGraph1, hScale1yI);
	GRAPH_SCALE_SetFactor(hScale1yI,0.25f);
	GRAPH_SCALE_SetTextColor(hScale1yI, GUI_YELLOW);
	GRAPH_SCALE_SetOff(hScale1yI,VERTICAL_SCALE_TICK-1);
	GRAPH_SCALE_SetNumDecs(hScale1yI,1);
	/* Set scales visible */
	GRAPH_SetGridVis(hGraph1,1);
	GRAPH_SetUserDraw(hGraph1, _UserDrawGraph); // Enable user draw
/* ------------------------------------------------------------------------------------------------------- */

	FRAMEWIN_SetText(hMW, "Power Grid Monitor");

	active_graph = 0;
	HAL_Delay(1000);		// Delay 1s to show init screen
	MULTIPAGE_SelectPage(hMpage, page = 0);

}

void TouchScreen_Update(void)
{
	GUI_PID_STATE TS_State;
	static TS_StateTypeDef prev_state;
	TS_StateTypeDef  ts;
	uint16_t xDiff, yDiff;

	BSP_TS_GetState(&ts);

	TS_State.Pressed = ts.TouchDetected;

	xDiff = (prev_state.X > ts.X) ? (prev_state.X - ts.X) : (ts.X - prev_state.X);
	yDiff = (prev_state.Y > ts.Y) ? (prev_state.Y - ts.Y) : (ts.Y - prev_state.Y);

	if((prev_state.TouchDetected != ts.TouchDetected )||
			(xDiff > 3 )||
			(yDiff > 3))
	{
		prev_state.TouchDetected = ts.TouchDetected;

		if((ts.X != 0) &&  (ts.Y != 0))
		{
			prev_state.X = ts.X;
			prev_state.Y = ts.Y;
		}

		TS_State.Layer = 0;
		TS_State.x = prev_state.X;
		TS_State.y = prev_state.Y;

		if(GUI_Initialized)	GUI_TOUCH_StoreStateEx(&TS_State);
	}
}
