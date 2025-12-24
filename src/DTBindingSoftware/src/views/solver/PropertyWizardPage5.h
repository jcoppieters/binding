#if !defined(AFX_PROPERTYWIZARDPAGE5_H__0C472BCF_26A1_4867_86DD_787E943E9DEF__INCLUDED_)
#define AFX_PROPERTYWIZARDPAGE5_H__0C472BCF_26A1_4867_86DD_787E943E9DEF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertyWizardPage5.h : header file
//

#include "PropertyWizardPageBase.h"
/////////////////////////////////////////////////////////////////////////////
// CPropertyWizardPage5 dialog


// TODO:
// Wanneer er een replace bezig is de controls even disablen + timer starten.
// Wanneer niet gelukt een melding geven.

class CPropertyWizardPage5 : public CPropertyWizardPageBase		
{
	DECLARE_DYNCREATE(CPropertyWizardPage5)

// Construction
public:
	CPropertyWizardPage5();
	~CPropertyWizardPage5();

// Dialog Data
	//{{AFX_DATA(CPropertyWizardPage5)
	enum { IDD = IDD_DIALOG_WIZARDPAGE5 };
	CComboBox	m_ComboNew;
	CComboBox	m_ComboOld;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropertyWizardPage5)
	public:
	virtual LRESULT OnWizardBack();
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

// Implementation
protected:
	unsigned long GetOldPhysicalAddress(void);
	unsigned long GetNewPhysicalAddress(void);
	void UpdateControls(void);
	// Generated message map functions
	//{{AFX_MSG(CPropertyWizardPage5)
	afx_msg void OnButtonReplace();
	virtual BOOL OnInitDialog();
	afx_msg void OnCloseupComboNew();
	afx_msg void OnCloseupComboOld();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// Virtuele functie van de basis 
	virtual void OnNotifyAssignedLogicalAddress(int nodeAddress);
	LONG OnUpdateWizardControls(WPARAM wParam, LPARAM lParam);


	enum {
		WM_UPDATE_WIZARD_CONTROLS = (WM_USER + 150) 
	};

private:
	enum { TIMER_ID_ASSIGN_BUSY = 1};

	typedef enum {
		STATE_IDLE,
		STATE_REPLACE_BUSY
	} State_t;

	State_t m_state;							// De toestand.
	int m_nodeAddress;							// Node address die toegekend werd.
	unsigned long m_ulNewPhysicalAddress;		// Fysisch address van de nieuwe module.
	unsigned long m_ulOldPhysicalAddress;		// Fysisch address van de oude module.
	DWORD m_styleWizardButtons;					// Sedert V17.01: De navigatie buttons vorige toestand. Tijdens de replace worden deze tijdelijk disabled.
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYWIZARDPAGE5_H__0C472BCF_26A1_4867_86DD_787E943E9DEF__INCLUDED_)
