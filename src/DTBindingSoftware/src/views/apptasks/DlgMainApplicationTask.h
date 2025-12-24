#if !defined(AFX_DLGMAINAPPLICATIONTASK_H__E3391B5A_94BF_43F0_8279_A12BF28753A8__INCLUDED_)
#define AFX_DLGMAINAPPLICATIONTASK_H__E3391B5A_94BF_43F0_8279_A12BF28753A8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgMainApplicationTask.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgMainApplicationTask dialog
#include "DlgChildAppTaskStatus.h"
#include "DlgChildAppTaskInfo.h"
#include "DlgChildAppTaskLog.h"

#include "ApplicationDlgAppearance.h"


/**
 * CDlgMainApplicationTask
 */
class CDlgMainApplicationTask : public CDialog
{
public:
	static CDlgMainApplicationTask* GetInstance();
	static void Destroy();

protected:
	
	#define WM_DIALOG_TIMER			1

	typedef enum
	{
		DIALOG_MODE_STATUS		= 0,	// Wordt getoond bij het uitvoeren van een generic task.
		DIALOG_MODE_ERROR_LOG	= 1,
		DIALOG_MODE_ASK_USER	= 2,
		DIALOG_MAX_MODES,
		DIALOG_MODE_UNKNOWN		= 0xFF	// Sedert V16.04: zorgt ervoor dat bringToFront() niet aangeroepen wordt...
	
	} ENUM_DIALOG_MODE;

	static CDlgMainApplicationTask* m_pDialog;
	static CRect rcWindowRestore[DIALOG_MAX_MODES];

	CDlgChildAppTaskStatus	m_DialogChildAppStatus;		// wordt getoond in de mode DIALOG_MODE_STATUS
	CDlgChildAppTaskInfo	m_DialogChildAskUser;		// wordt getoond in de mode DIALOG_MODE_ASK_USER
	CDlgChildAppTaskLog		m_DialogChildAppLog;		// wordt getoond in de mode DIALOG_MODE_ERROR_LOG

	unsigned long ulTimerInterval;

	ENUM_DIALOG_MODE CurrentMode;


// Construction
public:
	CDlgMainApplicationTask(CWnd* pParent = NULL);   // standard constructor

	void SetModeTaskStatus(CApplicationDlgAppearance* pDlgApp);
	void SetModeTaskAskUser(CApplicationDlgInfoAppearance* pDlgApp);
	void SetModeTaskErrorLog(CString szMessage);
	void ShowProgressStepUp();

// Dialog Data
	//{{AFX_DATA(CDlgMainApplicationTask)
	enum { IDD = IDD_DIALOG_TASK_MAIN };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgMainApplicationTask)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	
	BOOL Create()
	{ 
		return CDialog::Create(IDD_DIALOG_TASK_MAIN,
							   AfxGetMainWnd()); 
	}

	void StoreWindowPosition();
	void RestoreWindowPosition();
	void RefreshTimer();

	// Generated message map functions
	//{{AFX_MSG(CDlgMainApplicationTask)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	LRESULT OnUpdateControls(WPARAM wParam,LPARAM lParam);

	// MFC_GUI_HANDLE_CLOSEBUTTON
	// Oppassen: Deze overridden om de 'x' (=Close button) handler.
	// de WM_CLOSE werkt niet op deze en de child dialogen...
	// Wordt niet meer gebruikt zie macro MFC_GUI_REMOVE_CLOSEBUTTON
	// virtual void OnCancel();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGMAINAPPLICATIONTASK_H__E3391B5A_94BF_43F0_8279_A12BF28753A8__INCLUDED_)
