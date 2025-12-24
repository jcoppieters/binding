#ifndef INCLUDED_DUOTECNO_MFC_HELPER_TOOLTIPCTRL_H
#define INCLUDED_DUOTECNO_MFC_HELPER_TOOLTIPCTRL_H
/*************************************************************/
class CMFCToolTipCtrl {
public:
	static HWND Create(int toolID, HWND hDlg, UINT id);
#if(0)
	static HWND Create(int toolID, HWND hDlg, const CString& str);
#endif
};
/*************************************************************/
#endif 
/*************************************************************/