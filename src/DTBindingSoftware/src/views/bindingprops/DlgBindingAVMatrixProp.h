#if !defined(AFX_DLGBINDINGAVMATRIXPROP_H__345F3F1F_7B08_480A_9993_769AA15DE406__INCLUDED_)
#define AFX_DLGBINDINGAVMATRIXPROP_H__345F3F1F_7B08_480A_9993_769AA15DE406__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgBindingAVMatrixProp.h : header file
//


#include "DlgBindingUnitProp.h"
#include "bindingUIElement.h"
class CBindingAVMatrixUnit;

/////////////////////////////////////////////////////////////////////////////
// CDlgBindingAVMatrixProp dialog

class CDlgBindingAVMatrixProp : public CDlgBindingUnitProp
{
// Construction
protected:
	CBindingAVMatrixUnit *m_pBindingAVMatrixUnit;
	const DUOTECNO::BINDINGS::UI::CStringEntries* m_pStringEntries;

public:
	CDlgBindingAVMatrixProp(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgBindingAVMatrixProp)
	enum { IDD = IDD_DIALOG_PROP_BINDINGUNIT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgBindingAVMatrixProp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void InitControls(void);
	virtual void OnCmbBoxDataChanged(void);
	virtual void OnSpin1DataChanged(int iStep);
	virtual void OnSpin2DataChanged(int iStep);
	virtual void OnSpin3DataChanged(int iStep);
	void UpdateControls(void);
	BOOL LoadStringEntryResources(int Key);
	BOOL RetrieveBindingUnit( );
	CBindingAVMatrixUnit* getBindingAvMatrixUnit( );
	const DUOTECNO::BINDINGS::UI::CStringEntries* getStringEntries( ) const;

	// Generated message map functions
	//{{AFX_MSG(CDlgBindingAVMatrixProp)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

inline CBindingAVMatrixUnit* CDlgBindingAVMatrixProp::getBindingAvMatrixUnit( )
{
	ASSERT( m_pBindingAVMatrixUnit );
	return m_pBindingAVMatrixUnit;
}

inline const DUOTECNO::BINDINGS::UI::CStringEntries* CDlgBindingAVMatrixProp::getStringEntries( ) const
{
	ASSERT( m_pStringEntries );
	return m_pStringEntries;
}


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGBINDINGAVMATRIXPROP_H__345F3F1F_7B08_480A_9993_769AA15DE406__INCLUDED_)
