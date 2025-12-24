#if !defined(AFX_PROPERTYWIZARDPAGE1_H__7D85C303_30E6_4AC0_912D_E7C04C993312__INCLUDED_)
#define AFX_PROPERTYWIZARDPAGE1_H__7D85C303_30E6_4AC0_912D_E7C04C993312__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertyWizardPage1.h : header file
//

#include "PropertyWizardPageBase.h"

/////////////////////////////////////////////////////////////////////////////
// CPropertyWizardPage1 dialog


typedef enum 
{
	ENUM_RADIOBUTTON_PNP				= 0,
	ENUM_RADIOBUTTON_REPLACE			= 1,
	ENUM_RADIOBUTTON_PROGFILE			= 2,		// restore addresses as in database
	ENUM_RADIOBUTTON_MANUALASSIGN		= 3,
	ENUM_RADIOBUTTON_ASSIGNMASTER		= 4,		// sedert V16.44: Mogelijkheid om via de CAN-bus een master toe te kennen.
	ENUM_RADIOBUTTON_INIT				= 5,
	ENUM_RADIOBUTTON_NODESADDED			= 6,
	ENUM_RADIOBUTTON_NODESINCOMPATIBLE	= 7,
	ENUM_RADIOBUTTON_SENDPROGRAMFILE	= 8,
	ENUM_RADIOBUTTON_COPYNAMES			= 9,
	ENUM_RADIOBUTTON_ACCESSCODE			= 10,

}ENUM_RADIOBUTTONS;


class CPropertyWizardPage1 : public CPropertyWizardPageBase		
{
	DECLARE_DYNCREATE(CPropertyWizardPage1)

// Construction
public:	
	CPropertyWizardPage1();
	~CPropertyWizardPage1();

// Dialog Data
	//{{AFX_DATA(CPropertyWizardPage1)
	enum { IDD = IDD_DIALOG_WIZARDPAGE1 };
	int		m_RadioGroup;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropertyWizardPage1)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPropertyWizardPage1)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
	void EnableControls(ENUM_RADIOBUTTONS button,BOOL fState);

	void setTextRadioButton(int buttonIdx,UINT resourceID);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYWIZARDPAGE1_H__7D85C303_30E6_4AC0_912D_E7C04C993312__INCLUDED_)
