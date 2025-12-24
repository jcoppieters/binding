#pragma once

class CDebugDialogLayout
{
public:
	CDebugDialogLayout( );
	static void dumpControlLayout(CWnd* pDlg,const CString& strName = CString());

protected:
	static BOOL verifyPreconditions(CWnd* pDlg);
	static void dumpInfoDialog(CWnd* pDlg);
	static void dumpInfoChildControls(CWnd* pDlg);
};