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
#define ID_DROPDOWN_0 (GUI_ID_USER + 0x04)
#define ID_DROPDOWN_1 (GUI_ID_USER + 0x05)
#define ID_TEXT_0 (GUI_ID_USER + 0x06)
#define ID_TEXT_1 (GUI_ID_USER + 0x07)
#define ID_TEXT_2 (GUI_ID_USER + 0x08)
#define ID_TEXT_3 (GUI_ID_USER + 0x09)
#define ID_TEXT_4 (GUI_ID_USER + 0x0A)
#define ID_TEXT_5 (GUI_ID_USER + 0x0B)
#define ID_TEXT_6 (GUI_ID_USER + 0x0C)


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
  { WINDOW_CreateIndirect, "Harmonics", ID_WINDOW_0, 0, 0, 222, 270, 0, 0x0, 0 },
  { GRAPH_CreateIndirect, "graph", ID_GRAPH_0, 16, 18, 194, 124, 0, 0x0, 0 },
  { DROPDOWN_CreateIndirect, "Dd_Uf", ID_DROPDOWN_0, 14, 193, 80, 21, 0, 0x0, 0 },
  { DROPDOWN_CreateIndirect, "Dd_If", ID_DROPDOWN_1, 14, 234, 80, 21, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "FFT", ID_TEXT_0, 14, 4, 20, 16, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "U(f)", ID_TEXT_1, 128, 193, 80, 20, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "I(f)", ID_TEXT_2, 128, 234, 80, 22, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Harmonic U(f):", ID_TEXT_3, 14, 176, 80, 16, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "Harmonic I(f):", ID_TEXT_4, 14, 218, 80, 20, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "THD(U):", ID_TEXT_5, 14, 152, 94, 20, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "THD(I):", ID_TEXT_6, 114, 152, 94, 20, 0, 0x64, 0 },
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
  int     NCode;
  int     Id;
  // USER START (Optionally insert additional variables)
  // USER END

  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
    //
    // Initialization of 'graph'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_GRAPH_0);
    GRAPH_SetBorder(hItem, 2, 2, 2, 2);
    hGraph1 = hItem;
    //
    // Initialization of 'THD(U):'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_5);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    TEXT_SetFont(hItem, GUI_FONT_13_1);
    hTextTHD_U = hItem;
    //
    // Initialization of 'THD(I):'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_6);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    TEXT_SetFont(hItem, GUI_FONT_13_1);
    hTextTHD_I = hItem;
    //
    // Initialization of 'Dd_Uf'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_0);
    DROPDOWN_SetFont(hItem, GUI_FONT_16B_1);
    DROPDOWN_SetUpMode(hItem,1);
    DROPDOWN_SetListHeight(hItem, 96 + 2);
    DROPDOWN_AddString(hItem, "DC");
    DROPDOWN_AddString(hItem, "50Hz");
    DROPDOWN_AddString(hItem, "100Hz");
    DROPDOWN_AddString(hItem, "150Hz");
    DROPDOWN_AddString(hItem, "200Hz");
    DROPDOWN_AddString(hItem, "250Hz");
    DROPDOWN_AddString(hItem, "300Hz");
    DROPDOWN_AddString(hItem, "350Hz");
    DROPDOWN_AddString(hItem, "400Hz");
    DROPDOWN_AddString(hItem, "450Hz");
    DROPDOWN_AddString(hItem, "500Hz");
    DROPDOWN_AddString(hItem, "550Hz");
    DROPDOWN_AddString(hItem, "600Hz");
    DROPDOWN_AddString(hItem, "650Hz");
    DROPDOWN_AddString(hItem, "700Hz");
    DROPDOWN_AddString(hItem, "750Hz");
    DROPDOWN_AddString(hItem, "800Hz");
    DROPDOWN_SetAutoScroll(hItem, 1);
    DROPDOWN_SetScrollbarWidth(hItem,19);
    DROPDOWN_SetSel(hItem,1);
    hDd_Uf = hItem;
    //
    // Initialization of 'Dd_If'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_1);
    DROPDOWN_SetFont(hItem, GUI_FONT_16B_1);
    DROPDOWN_SetUpMode(hItem,1);
    DROPDOWN_SetListHeight(hItem, 96 + 2);
    DROPDOWN_AddString(hItem, "DC");
    DROPDOWN_AddString(hItem, "50Hz");
    DROPDOWN_AddString(hItem, "100Hz");
    DROPDOWN_AddString(hItem, "150Hz");
    DROPDOWN_AddString(hItem, "200Hz");
    DROPDOWN_AddString(hItem, "250Hz");
    DROPDOWN_AddString(hItem, "300Hz");
    DROPDOWN_AddString(hItem, "350Hz");
    DROPDOWN_AddString(hItem, "400Hz");
    DROPDOWN_AddString(hItem, "450Hz");
    DROPDOWN_AddString(hItem, "500Hz");
    DROPDOWN_AddString(hItem, "550Hz");
    DROPDOWN_AddString(hItem, "600Hz");
    DROPDOWN_AddString(hItem, "650Hz");
    DROPDOWN_AddString(hItem, "700Hz");
    DROPDOWN_AddString(hItem, "750Hz");
    DROPDOWN_AddString(hItem, "800Hz");
    DROPDOWN_SetAutoScroll(hItem, 1);
    DROPDOWN_SetScrollbarWidth(hItem,19);
    DROPDOWN_SetSel(hItem,1);
    hDd_If = hItem;
    //
    // Initialization of 'U(f)'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
    TEXT_SetFont(hItem, GUI_FONT_20B_1);
    TEXT_SetText(hItem, "0.0 V");
    hText_Uf = hItem;
    //
    // Initialization of 'I(f)'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_2);
    TEXT_SetFont(hItem, GUI_FONT_20B_1);
    TEXT_SetText(hItem, "0.00 A");
    hText_If = hItem;
    // USER START (Optionally insert additional code for further widget initialization)
	Uf_idx = If_idx = 1;	// Set 50Hz in harmonics dropdown
    // USER END
    break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
    case ID_DROPDOWN_0: // Notifications sent by 'Dd_Uf'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_SEL_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
    	  Uf_idx = (uint8_t)DROPDOWN_GetSel(hDd_Uf);
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_DROPDOWN_1: // Notifications sent by 'Dd_If'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_SEL_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
    	  If_idx = (uint8_t)DROPDOWN_GetSel(hDd_If);
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    // USER START (Optionally insert additional code for further Ids)
    // USER END
    }
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
*       CreateHarmonics
*/
WM_HWIN CreateHarmonics(void) {
  WM_HWIN hWin;

  hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
  return hWin;
}

// USER START (Optionally insert additional public code)
// USER END

/*************************** End of file ****************************/
