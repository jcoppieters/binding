// DlgMainApplicationTask.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgMainApplicationTask.h"

#include "ApplicationDlgAppearance.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
CDlgMainApplicationTask* CDlgMainApplicationTask::m_pDialog = 0;
CRect CDlgMainApplicationTask::rcWindowRestore[DIALOG_MAX_MODES] = { CRect(0,0,0,0),
																	 CRect(375,350,775,620),
																	 CRect(0,0,0,0)
};
/////////////////////////////////////////////////////////////////////////////
// CDlgMainApplicationTask dialog

CDlgMainApplicationTask* CDlgMainApplicationTask::GetInstance()
{
	if (m_pDialog != 0) 
	{
		ASSERT(m_pDialog->GetSafeHwnd() != 0);		
		if (m_pDialog->GetSafeHwnd() != 0) return m_pDialog;		
		else return 0;		
	}
	else
	{
		m_pDialog = new CDlgMainApplicationTask();
		ASSERT(m_pDialog);
		if (m_pDialog->Create() == TRUE) 
			return m_pDialog;
		ASSERT(0);
		return 0;
	}
}

void CDlgMainApplicationTask::Destroy()
{
	if ((m_pDialog != 0) && (m_pDialog->GetSafeHwnd() != 0))
	{
		m_pDialog->StoreWindowPosition();
 		m_pDialog->DestroyWindow();
	}
}

/////////////////////////////////////////////////////////////////////////////


CDlgMainApplicationTask::CDlgMainApplicationTask(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMainApplicationTask::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgMainApplicationTask)
	//}}AFX_DATA_INIT

	CurrentMode = DIALOG_MODE_UNKNOWN;	// DIALOG_MODE_STATUS;
	ulTimerInterval = 5000;  
}


void CDlgMainApplicationTask::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgMainApplicationTask)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgMainApplicationTask, CDialog)
	//{{AFX_MSG_MAP(CDlgMainApplicationTask)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP

	ON_MESSAGE(WM_UPDATE_CONTROLS,OnUpdateControls)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgMainApplicationTask message handlers

BOOL CDlgMainApplicationTask::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// MFC_GUI_REMOVE_CLOSEBUTTON.
	// onderstaande regele zorgt ervoor dat de 'close' button rechtsboven aan de dialoog
	// niet zichtbaar is.
	// De user moet de dialoog afsluiten via de cancel/ok (van de child dialoog.)
	ModifyStyle( WS_SYSMENU, 0 );		
		
	if (m_DialogChildAppStatus.GetSafeHwnd() == 0)
	{
		CRect rcChildFrame;		

		(GetDlgItem(IDC_CHILD_DIALOG1))->GetWindowRect(&rcChildFrame);
		ScreenToClient(&rcChildFrame);
		
		m_DialogChildAppStatus.Create(NULL,
									  NULL,
									  0,
									  rcChildFrame,
									  this,
									  IDD_DIALOG_CHILD_TASK_STATUS);
	}

	if (m_DialogChildAskUser.GetSafeHwnd() == 0)
	{
		CRect rcChildFrame;		

		(GetDlgItem(IDC_CHILD_DIALOG1))->GetWindowRect(&rcChildFrame);
		ScreenToClient(&rcChildFrame);
		
		m_DialogChildAskUser.Create(NULL,
									NULL,
									0,
									rcChildFrame,
									this,
									IDD_DIALOG_CHILD_TASK_INFO);
			
	}

	if (m_DialogChildAppLog.GetSafeHwnd() == 0)
	{
		CRect rcChildFrame;		
		GetClientRect(&rcChildFrame);

		rcChildFrame.bottom += 100;

		m_DialogChildAppLog.Create(NULL,
									NULL,
									0,
									rcChildFrame,
									this,
									IDD_DIALOG_CHILD_ERRORLOG);	
	}

	RefreshTimer();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgMainApplicationTask::PostNcDestroy() 
{
	if (GetSafeHwnd() != 0)
		KillTimer(WM_DIALOG_TIMER);

	this->CurrentMode = DIALOG_MODE_UNKNOWN;

	delete this;
	m_pDialog = 0;	
}

void CDlgMainApplicationTask::SetModeTaskStatus(CApplicationDlgAppearance* pDlgApp)
{
	ASSERT(GetSafeHwnd() != 0);
	CurrentMode = DIALOG_MODE_STATUS; 
	if (GetSafeHwnd() != 0)
	{
		SetWindowText(pDlgApp->GetTitle());
		m_DialogChildAppStatus.SetProperties(pDlgApp);
		PostMessage(WM_UPDATE_CONTROLS);	
	}
}

void CDlgMainApplicationTask::SetModeTaskAskUser(CApplicationDlgInfoAppearance* pDlgApp)
{
	ASSERT(GetSafeHwnd() != 0);
	CurrentMode = DIALOG_MODE_ASK_USER;
	if (GetSafeHwnd() != 0)
	{
		SetWindowText(pDlgApp->GetTitle());
		m_DialogChildAskUser.SetProperties(pDlgApp);
		PostMessage(WM_UPDATE_CONTROLS);
	}
}

void CDlgMainApplicationTask::SetModeTaskErrorLog(CString szMessage)
{
	ASSERT(GetSafeHwnd() != 0);
	CurrentMode = DIALOG_MODE_ERROR_LOG;
	if (GetSafeHwnd() != 0)
	{
		SetWindowText("Warnings happened !!");
		m_DialogChildAppLog.Set(szMessage);
		PostMessage(WM_UPDATE_CONTROLS);
	}
}

void CDlgMainApplicationTask::ShowProgressStepUp()
{
	if (CurrentMode == DIALOG_MODE_STATUS)
	{
		m_DialogChildAppStatus.ShowProgressStepUp();
	}
}


void CDlgMainApplicationTask::StoreWindowPosition()
{
	if (CurrentMode < DIALOG_MAX_MODES)
	{
		GetWindowRect(&rcWindowRestore[CurrentMode]);
		switch (CurrentMode)
		{
			case DIALOG_MODE_STATUS:
			{
				rcWindowRestore[DIALOG_MODE_ASK_USER] = rcWindowRestore[DIALOG_MODE_STATUS];
			} break;

			case DIALOG_MODE_ASK_USER:
			{
				rcWindowRestore[DIALOG_MODE_STATUS] = rcWindowRestore[DIALOG_MODE_ASK_USER];
			} break;
		}
	}
}

void CDlgMainApplicationTask::RestoreWindowPosition()
{
	if ((CurrentMode < DIALOG_MAX_MODES) &&
		(rcWindowRestore[CurrentMode].IsRectEmpty() == FALSE))
	{
		MoveWindow(rcWindowRestore[CurrentMode]);
	}
}

LRESULT CDlgMainApplicationTask::OnUpdateControls(WPARAM wParam,LPARAM lParam)
{
	RestoreWindowPosition();

	switch (CurrentMode)
	{
		case DIALOG_MODE_STATUS:
		{						
			ulTimerInterval = 5000;
			m_DialogChildAppStatus.ShowWindow(TRUE);
			m_DialogChildAskUser.ShowWindow(FALSE);
			m_DialogChildAppLog.ShowWindow(FALSE);
			ShowWindow(TRUE);
		} break;

		case DIALOG_MODE_ERROR_LOG:
		{	
			ulTimerInterval = 1500; 
			m_DialogChildAppStatus.ShowWindow(FALSE);
			m_DialogChildAskUser.ShowWindow(FALSE);
			m_DialogChildAppLog.ShowWindow(TRUE);
			ShowWindow(TRUE);
		} break;

		case DIALOG_MODE_ASK_USER:
		{		
			ulTimerInterval = 1500; 
			m_DialogChildAppStatus.ShowWindow(FALSE);
			m_DialogChildAskUser.ShowWindow(TRUE);
			m_DialogChildAppLog.ShowWindow(FALSE);
			ShowWindow(TRUE);
		} break;
	}
	SetActiveWindow();
	return TRUE;
}


void CDlgMainApplicationTask::OnTimer(UINT nIDEvent) 
{
	static BOOL fFlashWindow = FALSE;

	if ((nIDEvent == WM_DIALOG_TIMER) && (DIALOG_MODE_UNKNOWN != CurrentMode))
	{
		if ((CurrentMode == DIALOG_MODE_ERROR_LOG) ||
			(CurrentMode == DIALOG_MODE_ASK_USER))
		{						
			if (fFlashWindow == FALSE) 
				fFlashWindow = TRUE;
			else 
				fFlashWindow = FALSE;

			FlashWindow(fFlashWindow);
		}

		if (GetActiveWindow() != this)			
			BringWindowToTop();

		RefreshTimer();	
	}
	
	CDialog::OnTimer(nIDEvent);
}


void CDlgMainApplicationTask::RefreshTimer()
{
	SetTimer(WM_DIALOG_TIMER,ulTimerInterval,TIMERPROC (NULL));
}

// MFC_GUI_HANDLE_CLOSEBUTTON
// Deze virtuele functie overschrijven is de handler van de 'close'-button rechtsboven aan de 
// dialoog. De normale WM_CLOSE (=onClose) werkt niet (ook niet in de child dialogen.)
//
// Wordt niet meer gebruikt zie macro MFC_GUI_REMOVE_CLOSEBUTTON
#if(0)
void CDlgMainApplicationTask::OnCancel() 
{

	if (GetSafeHwnd() != 0)
		KillTimer(WM_DIALOG_TIMER);

	this->CDialog::OnCancel();
}
#endif 