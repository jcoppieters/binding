#if !defined(AFX_DLGALARMPROP_H__01CB9872_AC50_4DC7_B0BA_2E9424511074__INCLUDED_)
#define AFX_DLGALARMPROP_H__01CB9872_AC50_4DC7_B0BA_2E9424511074__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgAlarmProp.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgAlarmProp dialog

#include "DlgUnitProp.h"
class CDlgAlarmChildProp;


class CDlgAlarmProp : public CDlgUnitProp
{
// Construction
public:
	CDlgAlarmProp(CWnd* pParent = NULL);   // standard constructor

	virtual void InitControls(void);	// Sedert versie 0x0900 zonder params

// Dialog Data
	//{{AFX_DATA(CDlgAlarmProp)
	enum { IDD = IDD_DIALOG_PROP_UNIT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgAlarmProp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	virtual void ShowFullDialog(BOOL fShow);
	virtual void CleanUp(void);
	virtual void RequestRuntimeProperties(void);

	CDlgAlarmChildProp *pDlgChild;
	
	BOOL fControlsCreated;

	// Generated message map functions
	//{{AFX_MSG(CDlgAlarmProp)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGALARMPROP_H__01CB9872_AC50_4DC7_B0BA_2E9424511074__INCLUDED_)
