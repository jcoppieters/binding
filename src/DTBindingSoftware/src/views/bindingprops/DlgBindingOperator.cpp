// DlgBindingOperator.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgBindingOperator.h"
#include "MyWindowMessages.h"
#include "MainFrm.h"
#include "BindingConstants.h"
#include "BindingOperator.h"

#include "App\Appbindings\appbindingproperty\BindingPropertyStrings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// ---------------------------------------------------------------------------

CBindingTimerOperator* CDlgBindingOperator::m_pBindingOperator = NULL;
CDlgBindingOperator* CDlgBindingOperator::m_pBindingOperatorDlg = NULL;

// --------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// CDlgBindingOperator dialog


CDlgBindingOperator::CDlgBindingOperator(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgBindingOperator::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgBindingOperator)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	
	//m_pParent = pParent;

	m_nID = CDlgBindingOperator::IDD;
	m_pBindingOperator = NULL;
}


void CDlgBindingOperator::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgBindingOperator)
	DDX_Control(pDX, IDC_SPIN1, m_Spin1);
	DDX_Control(pDX, IDC_EDIT_SPIN1, m_EditSpin1);
	DDX_Control(pDX, IDC_COMBO_TIMER, m_CmbTimer);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgBindingOperator, CDialog)
	//{{AFX_MSG_MAP(CDlgBindingOperator)
	ON_CBN_SELENDOK(IDC_COMBO_TIMER, OnSelendokComboTimer)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN1, OnDeltaposSpin1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgBindingOperator message handlers
BOOL CDlgBindingOperator::Show(CBindingOperator *pBindingOperator,CPoint TopLeft)
{
	CRect rect;								
	ASSERT(m_pBindingOperatorDlg == NULL);

	m_pBindingOperatorDlg = new CDlgBindingOperator(AfxGetMainWnd());	//m_pMainWnd);

	if (m_pBindingOperatorDlg->Create() == TRUE) 
	{
		m_pBindingOperatorDlg->ShowWindow(TRUE);
		
		// Calculate positions ...
		m_pBindingOperatorDlg->GetClientRect(&rect);	
		if (TopLeft.y < rect.Height()) TopLeft.y = rect.Height();

		m_pBindingOperatorDlg->MoveWindow(TopLeft);		
		m_pBindingOperatorDlg->InitControls(pBindingOperator);
	}
	return TRUE;
}

BOOL CDlgBindingOperator::Destroy(void)
{
	if (m_pBindingOperatorDlg != NULL) 
	{
		((CDialog*)m_pBindingOperatorDlg)->DestroyWindow();
		m_pBindingOperatorDlg = NULL;		
	}

	ASSERT(m_pBindingOperatorDlg == NULL);

	return TRUE;
}

void CDlgBindingOperator::PostNcDestroy() 
{
	delete this;	
	m_pBindingOperatorDlg = NULL;	/* NEW !! */
}

void CDlgBindingOperator::MoveWindow(CPoint topleft)
{
	CRect rcMainWnd,rcWindow;

	//m_pMainWnd->GetWindowRect(&rcMainWnd);

	((CMainFrame*)AfxGetMainWnd())->GetWindowRect(&rcMainWnd);
	GetWindowRect(&rcWindow);
	
	const int xsize = rcWindow.Width();
	const int ysize = rcWindow.Height();	
	
	rcWindow.top = topleft.y + rcMainWnd.top;
	rcWindow.left = topleft.x + rcMainWnd.left;
	rcWindow.bottom = topleft.y + ysize + rcMainWnd.top;
	rcWindow.right = topleft.x + xsize + rcMainWnd.left;
	
	CWnd::MoveWindow(rcWindow);
}

void CDlgBindingOperator::InitControls(CBindingOperator *pOperator)
{
	CString szString;

	m_pBindingOperator = (CBindingTimerOperator*) pOperator;

	ASSERT(m_pBindingOperator);

	for (int i=0;i<MAX_PM_TIMERS;i++) 
	{
		szString.LoadString(TimerStrings[i]);
		m_CmbTimer.InsertString(i,szString);
	}

	UpdateControls();
}


BOOL CDlgBindingOperator::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_DELETE) 
	{	
		AfxGetMainWnd()->SendMessage(WM_ADDFLOW_REMOVE_CONTROL);
		AfxGetMainWnd()->SendMessage(WM_DESTROY_DIALOG_BINDINGUNIT);

		return TRUE;	// Coz Window is destroyed !!!!
	}	
	
	return CDialog::PreTranslateMessage(pMsg);
}


void CDlgBindingOperator::UpdateControls(void)
{
	CString szEdit1;
	TimerType type;
	BYTE bTimeValue;

	ASSERT(m_pBindingOperator);
	type = m_pBindingOperator->GetTimerType();
	m_CmbTimer.SetCurSel(type);

	bTimeValue = m_pBindingOperator->GetTimeValue();
	if (bTimeValue <=59) 
	{
		szEdit1.Format("%02d sec.",bTimeValue);
	}
	else 
	{
		szEdit1.Format("%02d min.",bTimeValue-59);
	}
	m_EditSpin1.SetWindowText(szEdit1);
}

void CDlgBindingOperator::OnSelendokComboTimer() 
{
	const int nCurrSel = m_CmbTimer.GetCurSel();
	if (nCurrSel < MAX_PM_TIMERS) 
	{
		m_pBindingOperator->SetTimerType((TimerType)nCurrSel);
	}
	else 
	{
		m_pBindingOperator->SetTimerType((TimerType) 0);
	}
}

void CDlgBindingOperator::OnDeltaposSpin1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Zie BUG-0137
	enum
	{
		OPERATOR_TIMER_VALUE_MIN			= 1,
		OPERATOR_TIMER_VALUE_MAX			= 119		// BUG-0137: 119 ipv 120
	};

	NM_UPDOWN* const pNMUpDown = (NM_UPDOWN*)pNMHDR;
	
	const int iStep = ((pNMUpDown->iDelta)*(-1));
	BYTE bTimeValue = m_pBindingOperator->GetTimeValue();
	bTimeValue += iStep;

	if (iStep >0) 
	{
		if (bTimeValue > OPERATOR_TIMER_VALUE_MAX) 
		{
			bTimeValue = OPERATOR_TIMER_VALUE_MAX;
		}
	}
	else 
	{
		if ((bTimeValue < OPERATOR_TIMER_VALUE_MIN) || 
			(bTimeValue > OPERATOR_TIMER_VALUE_MAX))
		{
			bTimeValue = OPERATOR_TIMER_VALUE_MIN;
		}
	}

	m_pBindingOperator->SetTimeValue(bTimeValue);
	UpdateControls();
	*pResult = 0;
}
