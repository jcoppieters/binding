#if !defined(AFX_DLGSENSPROPDLG_H__C0579A02_6E82_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_DLGSENSPROPDLG_H__C0579A02_6E82_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSensPropDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgSensPropDlg dialog
#include "DlgUnitProp.h"
#include "uicontrols/controls/MyEditCtrl.h"

class CDlgSensPropChild;

class CDlgSensPropDlg : public CDlgUnitProp
{
// Construction
public:
	CDlgSensPropDlg(CWnd* pParent = NULL);   // standard constructor

	virtual void InitControls(void); // Sedert versie 0x0900 zonder params

// Dialog Data
	//{{AFX_DATA(CDlgSensPropDlg)
	enum { IDD = IDD_DIALOG_PROP_UNIT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSensPropDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
private:
	void UpdateTempMode(BYTE bMode);
	CDlgSensPropChild *pDlgChild;

protected:

	enum {
		// controls.
		IDC_BUDDY_EDIT_SENSOR_SETPOINT = 1000,
		IDC_SPIN_UPDOWN_SENSOR_SETPOINT,

		// Timer id.
		TIMER_ID_SLIDER_REQUEST_SEND_VALUE = 0,
		TIMER_VALUE_SLIDER_DELAY_SEND_VALUE = 250,

		TIMER_ID_SPINCTRL_REQUEST_SEND_VALUE = 1,
		TIMER_VALUE_SPINCTRL_DELAY_SEND_VALUE = 500,
	};

	CMyEditCtrl m_editSetpoint;
	CSpinButtonCtrl m_spinSetpoint;
	CStatic	m_StaticMeasured;
	CSliderCtrl	m_trackbar;
	CButton m_btnModes[4];
	CButton m_CheckBtnOnOff;
	short m_valueSpinCtrl;				// Waarde voor de spin control
	BOOL fControlsCreated;	
	
	virtual void ShowFullDialog(BOOL fShow);
	virtual void CleanUp(void);
	virtual void RequestRuntimeProperties(void);

	void InitStaticControls(void);
	void UpdateRuntimeProperties(void);
	void CalculateDialogCoordinates();
	void setStaticSetpointValue(short sValue);
	void setStaticMeasuredValue(short sValue);
	void setTrackbarSetpointValue(short sValue);
	BYTE getBindingUnitType( ) const;
	short NormalizeSetpointValue(short value);
	BOOL sendSetpointValue(int value);

	// Generated message map functions
	//{{AFX_MSG(CDlgSensPropDlg)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBtnModesSun(void);
	afx_msg void OnBtnModesHalfSun(void);
	afx_msg void OnBtnModesMoon(void);
	afx_msg void OnBtnModesHalfMoon(void);
	afx_msg void OnBtnCheckOnOff(void);
	afx_msg void OnSpinSetpointUpDown(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSENSPROPDLG_H__C0579A02_6E82_11D8_B865_0050BAC412B1__INCLUDED_)
