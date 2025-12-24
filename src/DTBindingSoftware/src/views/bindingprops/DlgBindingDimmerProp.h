#if !defined(AFX_DLGBINDINGDIMMERPROP_H__EBC35E88_87A6_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_DLGBINDINGDIMMERPROP_H__EBC35E88_87A6_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgBindingDimmerProp.h : header file
//

/////////////////////////////////////////////////////////////////////////////

#include "DlgBindingUnitProp.h"
#include "BindingUIElement.h"

/////////////////////////////////////////////////////////////////////////////

// CDlgBindingDimmerProp dialog
class CBindingDimmerUnit;

class CDlgBindingDimmerProp : public CDlgBindingUnitProp
{
// Construction
public:
	CDlgBindingDimmerProp(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgBindingDimmerProp)
	enum { IDD = IDD_DIALOG_PROP_BINDINGUNIT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgBindingDimmerProp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CBindingDimmerUnit *m_pBindingDimUnit;
	const DUOTECNO::BINDINGS::UI::CStringEntries* m_pStringEntries;

	virtual void InitControls(void);
	virtual void OnCmbBoxDataChanged(void);
	virtual void OnSpin1DataChanged(int iStep);
	virtual void OnSpin2DataChanged(int iStep);
	virtual void OnSpin3DataChanged(int iStep);
	void UpdateControls(void);

	void SetDimmerValue(CEdit& Ctrl);
	void SetPIRDimmerValue(CEdit& ctrl);
	void SetPIRTimerValue(CEdit& ctrl);
	void SetSpeed(CEdit& ctrl,BYTE bSpeed);
	void SetDimCalcValue(CEdit& Ctrl);
	void setDimRangeValue(CEdit& ctrl,const CString& msg,int value);		// DIMMER_CHANGES_RANGE
	void setDimRangeMinValue(CEdit& ctrl);		// DIMMER_CHANGES_RANGE
	void setDimRangeMaxValue(CEdit& ctrl);		// DIMMER_CHANGES_RANGE


	BOOL RetrieveBindingUnit( );
	CBindingDimmerUnit* getBindingDimmerUnit( );	
	const DUOTECNO::BINDINGS::UI::CStringEntries* getStringEntries( ) const;

	// Generated message map functions
	//{{AFX_MSG(CDlgBindingDimmerProp)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



inline CBindingDimmerUnit* CDlgBindingDimmerProp::getBindingDimmerUnit( )
{
	ASSERT( m_pBindingDimUnit );
	return m_pBindingDimUnit;
}

inline const DUOTECNO::BINDINGS::UI::CStringEntries* CDlgBindingDimmerProp::getStringEntries( ) const
{
	ASSERT( m_pStringEntries );
	return m_pStringEntries;
}



//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGBINDINGDIMMERPROP_H__EBC35E88_87A6_11D8_B865_0050BAC412B1__INCLUDED_)
