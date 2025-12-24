#if !defined(AFX_PROPERTYWIZARDPAGE10_H__91B24DB3_CE8A_4552_A2F5_AB1192F11C2C__INCLUDED_)
#define AFX_PROPERTYWIZARDPAGE10_H__91B24DB3_CE8A_4552_A2F5_AB1192F11C2C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertyWizardPage10.h : header file
//


#if(0)

#include "PropertyWizardPageBase.h"

struct STRUCT_UNIT;
class CCanNode;

/////////////////////////////////////////////////////////////////////////////
// CPropertyWizardPage10 dialog

class CPropertyWizardPage10 : public CPropertyWizardPageBase		
{
	DECLARE_DYNCREATE(CPropertyWizardPage10)

// Construction
public:
	CPropertyWizardPage10();
	~CPropertyWizardPage10();

// Dialog Data
	//{{AFX_DATA(CPropertyWizardPage10)
	enum { IDD = IDD_DIALOG_WIZARDPAGE10 };
	CComboBox	m_CmbNetworkNodes;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropertyWizardPage10)
	public:
	virtual LRESULT OnWizardBack();
	virtual BOOL OnWizardFinish();
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPropertyWizardPage10)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonCopyall();
	afx_msg void OnButtonCopynode();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void AddNodeToComboBox(CCanNode *pDocNode);
	void DeleteNodeFromComboBox(BYTE bNodeAddress);
	BYTE GetAddressOfCurrSel(void);
	BOOL CompareNodeUnitNames(CCanNode *pNode1,CCanNode *pNode2);
	BOOL IsUnitNameDifferent(STRUCT_UNIT *pUnit1,STRUCT_UNIT *pUnit2);	
	BOOL CopyNodeNames(CCanNode *pDest,CCanNode *pSource);
	BOOL CopyUnitNames(STRUCT_UNIT *pUnit1,STRUCT_UNIT *pUnit2);
	BOOL CopyUnitNames(CCanNode *pDest,CCanNode *pSource);
	void UpdateControls(void);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


#endif

#endif // !defined(AFX_PROPERTYWIZARDPAGE10_H__91B24DB3_CE8A_4552_A2F5_AB1192F11C2C__INCLUDED_)
