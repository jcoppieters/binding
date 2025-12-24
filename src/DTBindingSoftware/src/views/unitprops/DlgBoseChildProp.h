#if !defined(AFX_DLGBOSECHILDPROP_H__660E6577_28D0_4D37_B639_DC0C479A9C5C__INCLUDED_)
#define AFX_DLGBOSECHILDPROP_H__660E6577_28D0_4D37_B639_DC0C479A9C5C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgBoseChildProp.h : header file
//
/////////////////////////////////////////////////////////////////////////////
#include "DlgUnitChildProp.h"

#include "App\Appbindings\appbindingproperty\BindingPropertyUnit.h"


/////////////////////////////////////////////////////////////////////////////
// CDlgBoseChildProp dialog

class CDlgBoseChildProp : public CDlgUnitChildProp
{
protected:
	static const char* szType[5];


// Construction
public:
	CDlgBoseChildProp(CWnd* pParent = NULL);   // standard constructor
	virtual void OnNotifyClose(void);
	void InitControls(		
		CBindingPropertyUnit *ptr);


// Dialog Data
	//{{AFX_DATA(CDlgBoseChildProp)
	enum { IDD = IDD_DIALOG_PROP_UNIT_CHILD };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgBoseChildProp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	virtual void InitStaticControls(void);
	virtual void InitControlProperties(void);	
	virtual void RestoreDefaults(void);

	void InitControlsToDefaults(void);
	void UpdateControls(void);

	CStatic m_staticInfo;

	// Generated message map functions
	//{{AFX_MSG(CDlgBoseChildProp)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:


	// Geen pointer naar de originele CBindingPropertyBoseRsUnit omdat we
	// de originele niet gaan wijzigen + omdat het een tijdelijke is 
	// die op de stack werd aangemaakt.

	CBindingPropertyBoseRsUnit m_bindingPropertyBoseRSUnit;		
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGBOSECHILDPROP_H__660E6577_28D0_4D37_B639_DC0C479A9C5C__INCLUDED_)
