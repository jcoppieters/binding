#if !defined(AFX_PROPERTYWIZARDPAGE0_H__05FE4EF6_9013_4CEB_A0C2_2CE4530E26F6__INCLUDED_)
#define AFX_PROPERTYWIZARDPAGE0_H__05FE4EF6_9013_4CEB_A0C2_2CE4530E26F6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertyWizardPage0.h : header file
//

#include "PropertyWizardPageBase.h"

/////////////////////////////////////////////////////////////////////////////
// CPropertyWizardPage0 dialog
//
// Weergave van de fouten.
//

class CPropertyWizardPage0 : public CPropertyWizardPageBase	
{
	DECLARE_DYNCREATE(CPropertyWizardPage0)

	typedef enum 
	{
		LISTBOX_INFO_DIFFERENTNAMES = 0,
		LISTBOX_INFO_PROGRAMFILEDIFFERENCES,
		LISTBOX_INFO_NODESREMOVED,
		LISTBOX_INFO_NODESADDED,
		LISTBOX_INFO_DUPLICATES,
		LISTBOX_INFO_FACTORYADDRESSES

	}ENUM_LISTBOX_INFO;

// Construction
public:
	CPropertyWizardPage0();
	~CPropertyWizardPage0();

	void AddToListBox(CString szText);

// Dialog Data
	//{{AFX_DATA(CPropertyWizardPage0)
	enum { IDD = IDD_DIALOG_WIZARDPAGE0 };
	CMyEditCtrl	m_EditText1;
	CListBox	m_ListBox;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropertyWizardPage0)
	public:
	virtual LRESULT OnWizardNext();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPropertyWizardPage0)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void AddToListBox(ENUM_LISTBOX_INFO info,BOOL fClear = TRUE);
	void showHorizontalScrollBar( );
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYWIZARDPAGE0_H__05FE4EF6_9013_4CEB_A0C2_2CE4530E26F6__INCLUDED_)
