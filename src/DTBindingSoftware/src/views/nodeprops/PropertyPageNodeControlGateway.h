#if !defined(AFX_PROPERTYPAGENODECONTROLGATEWAY_H__2E236E8C_3F36_4315_96B1_2D7F7E816705__INCLUDED_)
#define AFX_PROPERTYPAGENODECONTROLGATEWAY_H__2E236E8C_3F36_4315_96B1_2D7F7E816705__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertyPageNodeControlGateway.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPropertyPageNodeControlGateway dialog

#include "PropertyPageNodeControl.h"

class CPropertyPageNodeControlGateway : public CPropertyPageNode
{
	DECLARE_DYNCREATE(CPropertyPageNodeControlGateway)	

protected:
	BOOL fInitControls;
	#define TIMERID_UPDATE_OBJECT_TIMER		305
	#define UPDATE_OBJECT_TIMER				5000

// Construction
public:
	CPropertyPageNodeControlGateway();
	CPropertyPageNodeControlGateway(CCanNode *pNode);

	~CPropertyPageNodeControlGateway();

	virtual void UpdateRunTimeControls();

// Dialog Data
	//{{AFX_DATA(CPropertyPageNodeControlGateway)
	enum { IDD = IDD_DIALOG_NODE_CONTROL_GATEWAY };
	CEdit	m_EditState;
	CButton	m_BtnRequestState;
	CButton	m_BtnEraseDatabase;
	CButton	m_BtnClearEeprom;
	CButton	m_BtnResetFactory;
	CButton	m_BtnReset;
	CButton	m_CheckUnlock;
	CButton	m_CheckSerialize;
	CStatic m_staticSystemLog;
	CButton m_btnDownloadSystemLog;
	CStatic m_staticFirmware;
	CButton m_btnFirmware;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropertyPageNodeControlGateway)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPropertyPageNodeControlGateway)
	virtual BOOL OnInitDialog();	
	afx_msg void OnBtnClearEeprom();
	afx_msg void OnBtnReset();
	afx_msg void OnBtnResetFactory();
	afx_msg void OnCheckSerialize();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBtnRequest();
	afx_msg void OnBtnClear();
	afx_msg void OnBnClickedBtnDownloadSystemlog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void UpdateStaticControls(void);
	void RequestRuntimeStatussen(void);
public:
	afx_msg void OnBnClickedBtnFirmware();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYPAGENODECONTROLGATEWAY_H__2E236E8C_3F36_4315_96B1_2D7F7E816705__INCLUDED_)
