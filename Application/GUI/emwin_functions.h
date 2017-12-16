/*
 * emwin_functions.h
 *
 *  Created on: 11.12.2016
 *      Author: Kwarc
 */

#ifndef __EMWIN_FUNCTIONS_H_
#define __EMWIN_FUNCTIONS_H_

#include <GUI/WM_MyDefines.h>
#include <Peripherals/pga.h>
#include "GUI.h"
#include "stm32f429i_discovery_ts.h"
#include "stm32f429i_discovery_sdram.h"

#define LINE(x) ((x) * (GUI_GetFontSizeY()))
#define OMEGA "\u03a9"
#define TIME_SCALE_TICK 32
#define FREQ_SCALE_TICK 8
#define VERTICAL_SCALE_TICK 20
#define GRAPH_YT_EMPTY_DATA 0x7FFF
uint8_t GUI_Initialized;
uint8_t active_graph;
uint8_t page;

void EMWIN_Init(void);
void TouchScreen_Update(void);

#endif /* __EMWIN_FUNCTIONS_H_ */
