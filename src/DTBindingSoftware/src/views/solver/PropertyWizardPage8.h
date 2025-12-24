#if !defined(AFX_PROPERTYWIZARDPAGE8_H__8060E130_82B4_4659_89EB_8DDF2BCB9C8D__INCLUDED_)
#define AFX_PROPERTYWIZARDPAGE8_H__8060E130_82B4_4659_89EB_8DDF2BCB9C8D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertyWizardPage8.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPropertyWizardPage8 dialog

#include "PropertyWizardPageBase.h"

class CCanNode;

class CPropertyWizardPage8 : public CPropertyWizardPageBase	
{
	DECLARE_DYNCREATE(CPropertyWizardPage8)

	#define WM_UPDATE_CONTROLS	(WM_USER + 150)

// Construction
public:
	CPropertyWizardPage8();
	~CPropertyWizardPage8();

	void AddNodeToComboBox(int index,const CCanNode& Node);
	void ShowDocNodeInfo(const CCanNode* Node);
	void ShowRTNodeInfo(const CCanNode* Node);

// Dialog Data
	//{{AFX_DATA(CPropertyWizardPage8)
	enum { IDD = IDD_DIALOG_WIZARDPAGE8 };
	CButton	m_btnUpgrade;
	CMyEditCtrl	m_EditPhysicalRT;
	CMyEditCtrl	m_EditPhysicalDoc;
	CMyEditCtrl	m_EditUnitsRT;
	CMyEditCtrl	m_EditUnitsDoc;
	CMyEditCtrl	m_EditNameRt;
	CMyEditCtrl	m_EditNameDoc;
	CComboBox	m_CmbLogical;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropertyWizardPage8)
	public:
	virtual LRESULT OnWizardBack();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BYTE GetNodeAddress(void);
	// Generated message map functions
	//{{AFX_MSG(CPropertyWizardPage8)
	virtual BOOL OnInitDialog();
	afx_msg void OnCloseupComboLogicalAddress();
	afx_msg void OnCbnSelchangeComboLogicalAddress();
	afx_msg void OnButtonUpgrade();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	LONG OnUpdateControls(WPARAM wParam,LPARAM lParam);
	// void UpdateControls(void);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYWIZARDPAGE8_H__8060E130_82B4_4659_89EB_8DDF2BCB9C8D__INCLUDED_)
