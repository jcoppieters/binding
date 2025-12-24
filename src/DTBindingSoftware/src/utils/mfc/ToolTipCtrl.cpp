#include "stdafx.h"
#include "ToolTipCtrl.h"
/*************************************************************/

/*************************************************************/
/*************************************************************/
/*************************************************************/
HWND CMFCToolTipCtrl::Create(int toolID, HWND hDlg, UINT id)
{
    if (!toolID || !hDlg || !id)
    {
        return FALSE;
    }

    CString strTTText;
    strTTText.LoadString( id );

    // Get the window handle of the control to attach the TT to.
    HWND hwndTool = ::GetDlgItem(hDlg, toolID);

    // Create the tooltip window
    HWND hwndTip = CreateWindowEx(NULL, TOOLTIPS_CLASS, NULL,
                              WS_POPUP |TTS_ALWAYSTIP | TTS_BALLOON,
                              CW_USEDEFAULT, CW_USEDEFAULT,
                              CW_USEDEFAULT, CW_USEDEFAULT,
                              hDlg, NULL, 
                              AfxGetInstanceHandle() , NULL);

   if (!hwndTool || !hwndTip)
   {
       return (HWND)NULL;
   }                              

    // Associate the tooltip with the tool.
    TOOLINFO toolInfo = { 0 };
    toolInfo.cbSize = sizeof(toolInfo);
    toolInfo.hwnd = hDlg;
    toolInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
    toolInfo.uId = (UINT_PTR)hwndTool;
    toolInfo.lpszText = (char*)(LPCTSTR)strTTText;
    ::SendMessage(hwndTip, TTM_ADDTOOL, 0, (LPARAM)&toolInfo);
    ::SendMessageA(hwndTip, TTM_SETMAXTIPWIDTH, 0, 400); // force multi-line

    return hwndTip;
}

#if(0)
HWND CMFCToolTipCtrl::Create(int toolID, HWND hDlg, const CString& str) {

	if (!toolID || !hDlg || str.IsEmpty())
    {
        return FALSE;
    }

    

    // Get the window handle of the control to attach the TT to.
    HWND hwndTool = ::GetDlgItem(hDlg, toolID);

    // Create the tooltip window
    HWND hwndTip = CreateWindowEx(NULL, TOOLTIPS_CLASS, NULL,
                              WS_POPUP |TTS_ALWAYSTIP | TTS_BALLOON,
                              CW_USEDEFAULT, CW_USEDEFAULT,
                              CW_USEDEFAULT, CW_USEDEFAULT,
                              hDlg, NULL, 
                              AfxGetInstanceHandle() , NULL);

   if (!hwndTool || !hwndTip)
   {
       return (HWND)NULL;
   }                              

    // Associate the tooltip with the tool.
    TOOLINFO toolInfo = { 0 };
    toolInfo.cbSize = sizeof(toolInfo);
    toolInfo.hwnd = hDlg;
    toolInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
    toolInfo.uId = (UINT_PTR)hwndTool;
    toolInfo.lpszText = (char*)(LPCTSTR)str;
    ::SendMessage(hwndTip, TTM_ADDTOOL, 0, (LPARAM)&toolInfo);
    ::SendMessageA(hwndTip, TTM_SETMAXTIPWIDTH, 0, 400); // force multi-line

    return hwndTip;
}
#endif

/*************************************************************/