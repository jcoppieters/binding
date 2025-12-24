#if !defined(AFX_DLGDIMMERPROP_H__1DEEAF48_6DCB_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_DLGDIMMERPROP_H__1DEEAF48_6DCB_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgDimmerProp.h : header file
//

#include "DlgUnitProp.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgDimmerProp dialog

class CDlgDimmerProp : public CDlgUnitProp
{
// Construction
public:
	CDlgDimmerProp(CWnd* pParent = NULL);   // standard constructor

	virtual void InitControls(void); // Sedert versie 0x0900 zonder params

// Dialog Data
	//{{AFX_DATA(CDlgDimmerProp)
	enum { IDD = IDD_DIALOG_PROP_UNIT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgDimmerProp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
private :
				
protected:	
	CStatic	m_StaticValue;
	CSliderCtrl	m_trackbar;
	BOOL fControlsCreated;

	virtual void ShowFullDialog(BOOL fShow);
	virtual void CleanUp(void);
	virtual void RequestRuntimeProperties(void);

	BYTE bPreviousDimValue;	
	CDlgDimmerChildProp *m_pDlgChild;
	
	void UpdateRuntimeProperties();
	void InitStaticControls();

	// Generated message map functions
	//{{AFX_MSG(CDlgDimmerProp)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	#define TIMERID_DIALOG_REQUEST_UPDATE			306
	#define TIMERID_DIALOG_SEND_VALUE				307	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGDIMMERPROP_H__1DEEAF48_6DCB_11D8_B865_0050BAC412B1__INCLUDED_)
