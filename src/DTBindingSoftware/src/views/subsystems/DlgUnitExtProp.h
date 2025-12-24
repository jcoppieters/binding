#pragma once
#include "afxwin.h"


// CDlgUnitExtProp dialog

class CDlgUnitExtProp : public CDialog
{
	#define WM_UPDATE_CONTROLS	(WM_USER + 150)

	enum 
	{  
		WM_TIMER_UPDATE_PROPERTIES	= 1,
		WM_TIMER_RELOAD_VALUE		= 2000 
	};

	DECLARE_DYNAMIC(CDlgUnitExtProp)

public:
	CDlgUnitExtProp(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgUnitExtProp();

	virtual BOOL OnInitDialog();
	static CDlgUnitExtProp* getInstance( );
	static bool isActive( );
	void Show(void);

// Dialog Data
	enum { IDD = IDD_DIALOG_UNIT_EXT_PROP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	BOOL Create();

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedButtonRequest();
	afx_msg void OnBnClickedOk();

private:
	CComboBox m_cmbUnitType;
	CComboBox m_cmbUnitName;
	CStatic m_staticAddress;
	CStatic m_staticInfo;
	CButton m_btnRequest;
	static CDlgUnitExtProp* m_pInstance;
	int m_nID;

protected:
	void InitStaticControls(void);
	int AddUnitsToComboBox(BYTE bUnitType);
	void DisplayUnitProperty(BYTE bNodeAddress, BYTE bUnitAddress);
	void DisplayCurrentUnitProperties( );
	void ShowUnitPropertyControls(BOOL fShow);

	afx_msg void OnCbnSelendokComboUnittype();
	afx_msg void OnCbnSelendokComboUnitName();
	
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CStatic m_staticGroupBox;
};
