#include "stdafx.h"
#include "DebugDialogLayout.h"
#include "DialogLayoutHelper.h"
#include <afxwin.h>
#include <afxstr.h>


CString GetWindowClassName(CWnd* pWnd)
{
    CString className;
    if (pWnd && ::IsWindow(pWnd->GetSafeHwnd()))
    {
        TCHAR szClass[256] = { 0 };
        ::GetClassName(pWnd->GetSafeHwnd(), szClass, 255);
        className = szClass;
    }
    return className;
}



CDebugDialogLayout::CDebugDialogLayout( ) { }

void CDebugDialogLayout::dumpControlLayout(CWnd* pDlg,const CString& strName) {	
	if (FALSE == verifyPreconditions(pDlg)) {
		return;
	}

	CString strInfo = strName;
	if (strInfo.IsEmpty()) {
		strInfo = AfxGetAppName();
	}

	TRACE(_T("\n=== Layout dump van dialoog: %s ===\n"), strInfo);
	dumpInfoDialog(pDlg);
	dumpInfoChildControls(pDlg);
    TRACE(_T("=== Einde layout dump ===\n\n"));
}

void CDebugDialogLayout::dumpInfoDialog(CWnd* pDlg) {
	
	CString dlgClassName = GetWindowClassName(pDlg);
   
    CString dlgCaption;
    pDlg->GetWindowText(dlgCaption);

    UINT dlgID = (UINT)pDlg->GetDlgCtrlID();


	CDialogLayoutHelper dlgLayoutHelper(pDlg);

	// Scherm positie.
    CRect rectWindow = dlgLayoutHelper.getScreenWindowPos();
   
	// Topleft positie op het scherm.
	CPoint dlgTopLeft = dlgLayoutHelper.getScreenWindowPosTopLeft();

	// Bottom right positie op het scherm.
	CPoint dlgBottomRight = dlgLayoutHelper.getScreenWindowPosBottomRight();

	// Geeft de groote van de borders terug.
	const CRect rectBorders = dlgLayoutHelper.getWindowBorders();

	// Geeft de rectangle van de client weer.
    const CRect rectClient = dlgLayoutHelper.getClientAreaRectangle();


	TRACE(_T("Caption			: \"%s\"\n"), dlgCaption);
    TRACE(_T("Class				: %s\n"), dlgClassName);
    TRACE(_T("Dialog ID			: %u (0x%04X)\n"), dlgID, dlgID);
    TRACE(_T("Venster Pos		: (%d, %d)\n"), rectWindow.left, rectWindow.top);
    TRACE(_T("Venster Grootte	: %d x %d\n"), rectWindow.Width(), rectWindow.Height());
    TRACE(_T("Client Grootte	: %d x %d\n"), rectClient.Width(), rectClient.Height());
    TRACE(_T("Header Hoogte		: %d px\n"), rectBorders.top);
	TRACE(_T("Borders			: top=%d px, bottom=%d px, left =%d px, right=%d px\n"), rectBorders.top, rectBorders.bottom, rectBorders.left, rectBorders.right);
	TRACE(_T("Schermpositie		: Top-Left (%d,%d), Bottom-Right (%d,%d)\n"), dlgTopLeft.x, dlgTopLeft.y, dlgBottomRight.x, dlgBottomRight.y);
	TRACE(_T("---------------------------------------------------------\n"));
}

void CDebugDialogLayout::dumpInfoChildControls(CWnd* pDlg) {
	CWnd* pChild = pDlg->GetWindow(GW_CHILD);
    while (pChild)
    {
        CString className;
        ::GetClassName(pChild->GetSafeHwnd(), className.GetBuffer(256), 256);
        className.ReleaseBuffer();

        CRect rect;
        pChild->GetWindowRect(&rect);
        pDlg->ScreenToClient(&rect);

        UINT ctrlID = pChild->GetDlgCtrlID();

        CString ctrlName;
        ctrlName.Format(_T("ID: 0x%04X"), ctrlID);

        CString text;
        pChild->GetWindowText(text);

        TRACE(_T("[%-18s] (%4d,%4d)-(%4d,%4d)  Size: %3dx%3d  Text: \"%s\"  Class: %s\n"),
            ctrlName,
            rect.left, rect.top,
            rect.right, rect.bottom,
            rect.Width(), rect.Height(),
            text,
            className);
        
        pChild = pChild->GetNextWindow();
    }
}

BOOL CDebugDialogLayout::verifyPreconditions(CWnd* pDlg) {
	if ((NULL == pDlg) || (FALSE == ::IsWindow(pDlg->GetSafeHwnd()))) {
		ASSERT(NULL != pDlg);
		TRACE(_T("[LayoutDebugger] Ongeldig dialoogvenster\n"));
        return FALSE;
	}
	return TRUE;
}

