#if !defined(AFX_PROPERTYWIZARDPAGE2_H__0AA08A69_2AFC_4A76_9E6B_9E1C34B8A657__INCLUDED_)
#define AFX_PROPERTYWIZARDPAGE2_H__0AA08A69_2AFC_4A76_9E6B_9E1C34B8A657__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertyWizardPage2.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPropertyWizardPage2 dialog

#include "PropertyWizardPageBase.h"

////////////////////////////////////////////////////////////////////
// Restore devices addresses as in database.
////////////////////////////////////////////////////////////////////

class CPropertyWizardPage2 : public CPropertyWizardPageBase		
{	
	DECLARE_DYNCREATE(CPropertyWizardPage2)

	#define WM_UPDATE_CONTROLS	(WM_USER + 150)

// Construction
public:
	CPropertyWizardPage2();
	~CPropertyWizardPage2();

// Dialog Data
	//{{AFX_DATA(CPropertyWizardPage2)
	enum { IDD = IDD_DIALOG_WIZARDPAGE2 };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropertyWizardPage2)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardBack();
	virtual LRESULT OnWizardNext();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPropertyWizardPage2)
	afx_msg void OnButtonRestore();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	LONG OnUpdateControls(WPARAM wParam,LPARAM lParam);

	// Virtuele functie van de basis 
	virtual void OnNotifyAssignedLogicalAddress(int nodeAddress);

private:

	enum { TIMER_ID_ASSIGN_BUSY = 0 };

	typedef enum {
		STATE_INIT = 0,
		STATE_NEXT,						// Next button tonen.
		STATE_FINISH_IDLE,				// Finish button disablen - Moeten nog de taak uitvoeren.
		STATE_FINISH_BUSY,				// Bezig met het algoritme.
		STATE_FINISH_DONE				// Finish button enablen - Moeten nog de taak uitvoeren.
	} State_t;

	State_t m_state;					// de toestand van de wizard buttons +gui
	int m_countNrOfAddresses;			// Het aantal nodes waarbij een adres toegekend wordt.
	CList<int> m_addresses;				// Lijst van addressen die momenteel een 0xFE adres hebben.
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYWIZARDPAGE2_H__0AA08A69_2AFC_4A76_9E6B_9E1C34B8A657__INCLUDED_)
