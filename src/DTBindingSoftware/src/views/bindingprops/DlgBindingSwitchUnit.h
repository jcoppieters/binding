#if !defined(AFX_DLGBINDINGSWITCHUNIT_H__86C38121_87CB_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_DLGBINDINGSWITCHUNIT_H__86C38121_87CB_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgBindingSwitchUnit.h : header file
//


#include "DlgBindingUnitProp.h"
#include "BindingUIElement.h"

class CBindingRelaisUnit;

/////////////////////////////////////////////////////////////////////////////
// CDlgBindingSwitchUnit dialog

class CDlgBindingSwitchUnit : public CDlgBindingUnitProp
{
protected:
	CBindingRelaisUnit *m_pBindingSwitchUnit;
	const DUOTECNO::BINDINGS::UI::CStringEntries* m_pStringEntries;

public:
	CDlgBindingSwitchUnit(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgBindingSwitchUnit)
	enum { IDD = IDD_DIALOG_PROP_BINDINGUNIT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgBindingSwitchUnit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void InitControls(void);
	virtual void OnCmbBoxDataChanged(void);
	virtual void OnSpin1DataChanged(int iStep);
	void UpdateControls(void);

	BOOL RetrieveBindingUnit( );
	CBindingRelaisUnit* getBindingSwitchUnit( );	
	const DUOTECNO::BINDINGS::UI::CStringEntries* getStringEntries( ) const;

	// Generated message map functions
	//{{AFX_MSG(CDlgBindingSwitchUnit)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

inline CBindingRelaisUnit* CDlgBindingSwitchUnit::getBindingSwitchUnit( )
{
	ASSERT( m_pBindingSwitchUnit );
	return m_pBindingSwitchUnit;
}

inline const DUOTECNO::BINDINGS::UI::CStringEntries* CDlgBindingSwitchUnit::getStringEntries( ) const
{
	ASSERT( m_pStringEntries );
	return m_pStringEntries;
}



//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGBINDINGSWITCHUNIT_H__86C38121_87CB_11D8_B865_0050BAC412B1__INCLUDED_)
