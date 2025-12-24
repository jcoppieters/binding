#if !defined(AFX_DLGBINDINGAUDIOHC4UNIT_H__CE574857_1305_41E3_A388_C1D70221040B__INCLUDED_)
#define AFX_DLGBINDINGAUDIOHC4UNIT_H__CE574857_1305_41E3_A388_C1D70221040B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgBindingAudioHC4Unit.h : header file
//

#include "DlgBindingUnitProp.h"
#include "bindingUIelement.h"
class CBindingHC4AudioUnit;

/////////////////////////////////////////////////////////////////////////////
// CDlgBindingAudioHC4Unit dialog

class CDlgBindingAudioHC4Unit : public CDlgBindingUnitProp
{
private:
	CBindingHC4AudioUnit *m_pBindingHC4AudioUnit;
	const DUOTECNO::BINDINGS::UI::CStringEntries* m_pStringEntries;

// Construction
public:
	CDlgBindingAudioHC4Unit(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgBindingAudioHC4Unit)
	enum { IDD = IDD_DIALOG_PROP_BINDINGUNIT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgBindingAudioHC4Unit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void InitControls(void);
	virtual void OnCmbBoxDataChanged(void);
	virtual void OnSpin1DataChanged(int iStep);
	virtual void OnSpin2DataChanged(int iStep);	
	void UpdateControls(void);
	BOOL LoadStringEntryResources(int Key);
	BOOL RetrieveBindingUnit( );
	CBindingHC4AudioUnit* getBindingHC4AudioUnit( );
	const DUOTECNO::BINDINGS::UI::CStringEntries* getStringEntries( ) const;

	// Generated message map functions
	//{{AFX_MSG(CDlgBindingAudioHC4Unit)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


inline CBindingHC4AudioUnit* CDlgBindingAudioHC4Unit::getBindingHC4AudioUnit( )
{
	ASSERT( m_pBindingHC4AudioUnit );
	return m_pBindingHC4AudioUnit;
}

inline const DUOTECNO::BINDINGS::UI::CStringEntries* CDlgBindingAudioHC4Unit::getStringEntries( ) const
{
	ASSERT( m_pStringEntries );
	return m_pStringEntries;
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGBINDINGAUDIOHC4UNIT_H__CE574857_1305_41E3_A388_C1D70221040B__INCLUDED_)
