#pragma once
#include "afxwin.h"

namespace DUOTECNO {
	class CDlgHelperFont;
};

// CDlgUDPLANSettings dialog

class CDlgUDPLANSettings : public CDialog
{
	DECLARE_DYNAMIC(CDlgUDPLANSettings)

public:
	CDlgUDPLANSettings(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgUDPLANSettings();

	CString getNetworkAdapterAddress(void);

// Dialog Data
	enum { IDD = IDD_DIALOG_UDPCFGSETTINGS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	
	LONG OnUpdateDialog(WPARAM wParam,LPARAM lParam);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnCbnSelendokComboNetworkadapter();

public:
	DUOTECNO::CDlgHelperFont* m_pDlgFontHelper;
	CString m_szNetworkAdapterAddress;
	CComboBox m_cmbNetworkAdapters;
	CStatic m_staticIPAddress;
	CStatic m_staticNetmask;
};
