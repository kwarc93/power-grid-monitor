/*********************************************************************
*                                                                    *
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
*                                                                    *
**********************************************************************
*                                                                    *
* C-file generated by:                                               *
*                                                                    *
*        GUI_Builder for emWin version 5.32                          *
*        Compiled Oct  8 2015, 11:59:02                              *
*        (c) 2015 Segger Microcontroller GmbH & Co. KG               *
*                                                                    *
**********************************************************************
*                                                                    *
*        Internet: www.segger.com  Support: support@segger.com       *
*                                                                    *
**********************************************************************
*/

// USER START (Optionally insert additional includes)
#include "WM_MyDefines.h"
// USER END

#include "DIALOG.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_WINDOW_0 (GUI_ID_USER + 0x00)
#define ID_GRAPH_0 (GUI_ID_USER + 0x01)
#define ID_TEXT_0 (GUI_ID_USER + 0x02)
#define ID_TEXT_1 (GUI_ID_USER + 0x03)
#define ID_TEXT_2 (GUI_ID_USER + 0x04)
#define ID_TEXT_3 (GUI_ID_USER + 0x05)
#define ID_TEXT_4 (GUI_ID_USER + 0x06)
#define ID_TEXT_5 (GUI_ID_USER + 0x07)
#define ID_TEXT_6 (GUI_ID_USER + 0x08)
#define ID_TEXT_7 (GUI_ID_USER + 0x09)
#define ID_TEXT_8 (GUI_ID_USER + 0x0A)
#define ID_TEXT_9 (GUI_ID_USER + 0x0B)
#define ID_TEXT_10 (GUI_ID_USER + 0x0C)


// USER START (Optionally insert additional defines)
// USER END

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

// USER START (Optionally insert additional static data)
// USER END

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { WINDOW_CreateIndirect, "Waveforms", ID_WINDOW_0, 8, 42, 222, 270, 0, 0x0, 0 },
  { GRAPH_CreateIndirect, "graph", ID_GRAPH_0, 16, 18, 194, 124, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "U(t)", ID_TEXT_0, 24, 2, 25, 14, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "U RMS", ID_TEXT_1, 16, 152, 90, 24, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "I RMS", ID_TEXT_2, 116, 152, 90, 24, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "Freq", ID_TEXT_3, 16, 174, 90, 24, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "PF", ID_TEXT_4, 16, 221, 90, 24, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "P", ID_TEXT_5, 16, 199, 90, 24, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Q", ID_TEXT_6, 110, 199, 100, 20, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "S", ID_TEXT_7, 116, 174, 90, 24, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "ZL", ID_TEXT_8, 6, 246, 211, 20, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "I(t)", ID_TEXT_9, 182, 2, 21, 20, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "DPF", ID_TEXT_10, 116, 221, 90, 20, 0, 0x0, 0 },
  // USER START (Optionally insert additional widgets)
  // USER END
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/

// USER START (Optionally insert additional static code)
// USER END

/*********************************************************************
*
*       _cbDialog
*/
static void _cbDialog(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  // USER START (Optionally insert additional variables)
  // USER END

  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
    //
    // Initialization of 'waveforms'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_GRAPH_0);
    GRAPH_SetBorder(hItem, 2, 2, 2, 2);
    hGraph0 = hItem;
    //
    // Initialization of 'U RMS'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
    TEXT_SetFont(hItem, GUI_FONT_20B_1);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    hText_URMS = hItem;
    //
    // Initialization of 'I RMS'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_2);
    TEXT_SetFont(hItem, GUI_FONT_20B_1);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    hText_IRMS = hItem;
    //
    // Initialization of 'Freq'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_3);
    TEXT_SetFont(hItem, GUI_FONT_20B_1);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    hText_Freq = hItem;
    //
    // Initialization of 'PF'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_4);
    TEXT_SetFont(hItem, GUI_FONT_20B_1);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    hText_PF = hItem;
    //
    // Initialization of 'DPF'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_10);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    TEXT_SetFont(hItem, GUI_FONT_20B_1);
    hText_DPF = hItem;
    //
    // Initialization of 'P'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_5);
    TEXT_SetFont(hItem, GUI_FONT_20B_1);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    hText_P = hItem;
    //
    // Initialization of 'Q'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_6);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    TEXT_SetFont(hItem, GUI_FONT_20B_1);
    hText_Q = hItem;
    //
    // Initialization of 'S'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_7);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    TEXT_SetFont(hItem, GUI_FONT_20B_1);
    hText_S = hItem;
    //
    // Initialization of 'ZL'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_8);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    TEXT_SetFont(hItem, GUI_FONT_16B_1);
    hText_ZL = hItem;
    // USER START (Optionally insert additional code for further widget initialization)
    // USER END
    break;
  // USER START (Optionally insert additional message handling)
  // USER END
  default:
    WM_DefaultProc(pMsg);
    break;
  }
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       CreateVoltage
*/
WM_HWIN CreateWaveforms(void) {
  WM_HWIN hWin;

  hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
  return hWin;
}

// USER START (Optionally insert additional public code)
// USER END

/*************************** End of file ****************************/
