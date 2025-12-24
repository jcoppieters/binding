#if !defined(AFX_PROPERTYPAGENODECONTROL_H__B71B4786_A991_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_PROPERTYPAGENODECONTROL_H__B71B4786_A991_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertyPageNodeControl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPropertyPageNodeControl dialog
class CCanNode;
#include "PropertyPageNode.h"
#include "afxwin.h"


class CPropertyPageNodeControl : public CPropertyPageNode
{
	#define TIMERID_UPDATE_OBJECT_TIMER		305
	#define UPDATE_OBJECT_TIMER				5000	

	DECLARE_DYNCREATE(CPropertyPageNodeControl)
	BOOL fInitControls;

protected:
	//CButton* m_pBtnClearEeprom;

// Construction
public:
	CPropertyPageNodeControl();
	CPropertyPageNodeControl(CCanNode *pNode);
	CPropertyPageNodeControl(unsigned short nID,CCanNode *pNode);
	~CPropertyPageNodeControl();

	virtual void UpdateRunTimeControls();


// Dialog Data
	//{{AFX_DATA(CPropertyPageNodeControl)
	enum { IDD = IDD_DIALOG_NODE_CONTROL };
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
	//{{AFX_VIRTUAL(CPropertyPageNodeControl)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPropertyPageNodeControl)
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnClearEeprom();
	afx_msg void OnBtnReset();
	afx_msg void OnBtnResetFactory();
	afx_msg void OnCheckSerialize();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBnClickedBtnDownloadSystemlog();
	afx_msg void OnBnClickedBtnFirmware();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void UpdateStaticControls(void);
	void RequestRuntimeStatussen(void);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYPAGENODECONTROL_H__B71B4786_A991_11D8_B865_0050BAC412B1__INCLUDED_)
