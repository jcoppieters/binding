// DlgAppBindingsVerify.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgAppBindingsVerify.h"

// -----------------------------------
#include "TMGlobals.h"

using namespace TMBindingDatabase;
using namespace TMNodeDatabase;
// -----------------------------------

#include "mainfrm.h"

#include "app\appbindings\api\bindingconv2nodes_api.h"

#include "app\appbindings\appbindingconv2nodes\bindingconversiontonodes_utils.h"
#include "app\appfilelog\appfilelogapi.h"




enum 
{
	WM_FSM_EVENT			= (WM_USER + 100),

	WM_TIMER_START			= 1,

	
	// Waarde
	TIMER_START_VALUE		= 100	// Versie 0x0903: Waarde aangepast van 10 naar 100 msec.
};


// CDlgAppBindingsVerify dialog

IMPLEMENT_DYNAMIC(CDlgAppBindingsVerify, CDialog)

CDlgAppBindingsVerify::CDlgAppBindingsVerify(CWnd* pParent,const CStringArray* pStringArray)
	: CDialog(CDlgAppBindingsVerify::IDD, pParent),
	  m_pStrArrErrorsBindingFiles(pStringArray),
	  m_fPaintHandler(FALSE)
{
	m_currState = STATE_INIT;
}

CDlgAppBindingsVerify::~CDlgAppBindingsVerify()
{
}

void CDlgAppBindingsVerify::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_YES, m_btnYes);
	DDX_Control(pDX, IDC_BUTTON_NO, m_btnNo);
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Control(pDX, IDC_STATIC_INFO, m_staticInfo);
	DDX_Control(pDX, IDC_PROGRESS_INFO, m_progressInfo);
	DDX_Control(pDX, IDC_STATIC_ICON2, m_staticIcon);
}


BEGIN_MESSAGE_MAP(CDlgAppBindingsVerify, CDialog)
		ON_MESSAGE(WM_FSM_EVENT, OnFsmEvent)
		ON_BN_CLICKED(IDC_BUTTON_YES, &CDlgAppBindingsVerify::OnBnClickedButtonYes)
		ON_BN_CLICKED(IDC_BUTTON_NO, &CDlgAppBindingsVerify::OnBnClickedButtonNo)
		ON_BN_CLICKED(IDOK, &CDlgAppBindingsVerify::OnBnClickedOk)
		ON_WM_ENTERIDLE()
		ON_WM_ENABLE()
		ON_WM_ACTIVATE()
		ON_WM_TIMER()
		ON_WM_PAINT()
		ON_WM_CLOSE()
END_MESSAGE_MAP()


// CDlgAppBindingsVerify message handlers

BOOL CDlgAppBindingsVerify::OnInitDialog()
{
	DWORD dwSttle = 0;

	CDialog::OnInitDialog();

	CString szMessage(_T("Chevk loaded databases, please wait."));
	m_staticInfo.SetWindowText(szMessage);

	iconShow(IDI_INFORMATION);
	buttonsHideAll();
	m_progressInfo.ShowWindow(FALSE);

	// Oppassen:
	// Moeten hier een timer starten omdat anders de dialoog niet altijd aktief is.
	SetTimer(WM_TIMER_START,TIMER_START_VALUE,(TIMERPROC)NULL);

	ASSERT(this->IsWindowVisible()==FALSE);
	ASSERT(this->m_fPaintHandler == FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CDlgAppBindingsVerify::OnFsmEvent(WPARAM wParam,LPARAM lParam)
{
	LRESULT res = TRUE;

	switch(m_currState)
	{
		case STATE_INIT:
		{
			const Event_t ev = (Event_t) wParam;

			switch(ev)
			{
				case EVENT_START:
				{
					ASSERT(this->GetSafeHwnd()!= 0);
					ASSERT(this->m_fPaintHandler==TRUE);

					CString szMessage(_T("Verify bindings busy, please wait."));
					m_staticInfo.SetWindowText(szMessage);

					iconShow(IDI_INFORMATION);

					buttonsHideAll();
					m_progressInfo.ShowWindow(FALSE);

					PostEvent(EVENT_NEXT_STEP);
					SetState(STATE_STRINGARRAY_CHECK);
				} break;

				default:
				{
					ASSERT(0);
				} break;
			}

		} break;

		case STATE_STRINGARRAY_CHECK:
		{
			// Controleren of er fouten waren bij het inladen van de binding bestanden.
			// Vragen of de gebruiker de logbestanden wilt zien.

			const Event_t ev = (Event_t) wParam;

			switch(ev)
			{
				case EVENT_NEXT_STEP:
				{
					if(0!=m_pStrArrErrorsBindingFiles)
					{
						if (!m_pStrArrErrorsBindingFiles->IsEmpty())
						{
							CString szMessage(_T("Problems happened while loading the bindingfiles.\r\n"
												 "Would you like to see the logfiles ?"));
							
							m_staticInfo.SetWindowText(szMessage);

							iconShow(IDI_QUESTION);
							m_btnOk.ShowWindow(FALSE);
							m_btnNo.ShowWindow(TRUE);
							m_btnYes.ShowWindow(TRUE);
							m_progressInfo.ShowWindow(FALSE);					
							// Oppassen: In deze toestand blijven
						}
						else
						{
							buttonsHideAll();
							progressbarStart();

							PostEvent(EVENT_NEXT_STEP);
							SetState(STATE_BINDINGS_VERIFY);
						}
					}
					else
					{
						buttonsHideAll();
						progressbarStart();

						PostEvent(EVENT_NEXT_STEP);
						SetState(STATE_BINDINGS_VERIFY);
					}
				} break;

				case EVENT_BTN_YES:		
				{
					CString szMessage(_T("Problems happened while loading the bindingfiles.\r\n"
										 "See logfile.\r\n"
										 "Press Next button to verify the binding databases.\r\n"));							
					m_staticInfo.SetWindowText(szMessage);

					iconShow(IDI_INFORMATION);
					m_btnOk.ShowWindow(TRUE);
					m_btnOk.SetWindowText(_T("Next"));
					m_btnNo.ShowWindow(FALSE);
					m_btnYes.ShowWindow(FALSE);
					m_progressInfo.ShowWindow(FALSE);	

					CAppFileLogAPI::showLogFileErrors(APPFILELOG_BINDINGFILES_LOAD_EC);

					SetState(STATE_STRINGARRAY_SHOW);
				} break;

				case EVENT_BTN_NO:
				{
					buttonsHideAll();
					progressbarStart();

					PostEvent(EVENT_NEXT_STEP);
					SetState(STATE_BINDINGS_VERIFY);
				} break;			
			}
		} break;

		case STATE_STRINGARRAY_SHOW:
		{
			const Event_t ev = (Event_t) wParam;

			switch(ev)
			{
				case EVENT_BTN_CONTINUE:
				{
					CString szMessage(_T("Not all bindings in the databases could be found "
										 "in the binding files.\r\n"
										 "Would you like to do a restore to correct the problem ?"));
					m_staticInfo.SetWindowText(szMessage);

					buttonsHideAll();
					m_btnOk.SetWindowText(_T("Ok"));
					
					iconShow(IDI_INFORMATION);
					progressbarStart();

					PostEvent(EVENT_NEXT_STEP);
					SetState(STATE_BINDINGS_VERIFY);

				} break;
			}
		} break;

		case STATE_BINDINGS_VERIFY:
		{
			const Event_t ev = (Event_t) wParam;

			switch(ev)
			{
				case EVENT_NEXT_STEP:
				{				
					CStringArray strArray;

					buttonsHideAll();

					if (TRUE==CBindingConv2Nodes_Api::SearchBindingEntriesInBindingFiles(
								&strArray,
								(void*)this,
								progressCallback))
					{
						SetState(STATE_MANUALBINDINGS_SEARCH);	
						PostEvent(EVENT_NEXT_STEP);	
					}
					else
					{
						CString szMessage(_T("Some bindings in the databases could not be found "
											 "in the binding files. "
											 "For details view the logfiles in the \"logfiles\" project subdirectory.\r\n"
											 "Would you like to do a restore to correct the problem ?"));
						m_staticInfo.SetWindowText(szMessage);

						iconShow(IDI_QUESTION);
						m_progressInfo.ShowWindow(FALSE);
						m_btnOk.ShowWindow(FALSE);
						m_btnNo.ShowWindow(TRUE);
						m_btnYes.ShowWindow(TRUE);
					}												
				} break;

				case EVENT_BTN_YES:		
				{					
					PostEvent(EVENT_NEXT_STEP);
					SetState(STATE_BINDINGS_RESTORE);
				} break;

				case EVENT_BTN_NO:
				{
					PostEvent(EVENT_NEXT_STEP);	
					SetState(STATE_MANUALBINDINGS_SEARCH);	
				} break;	
			}		
		} break;

		case STATE_BINDINGS_RESTORE:
		{
			const Event_t ev = (Event_t) wParam;
			switch(ev)
			{
				case EVENT_NEXT_STEP:
				{			
					int countBindings=0;
					int countPropertyUnits=0;
					CStringArray strArray;
					CStringArray strArray2;

					CString szMessage(_T("Start restoring bindings from database to the bindingfiles\r\n"
										 "Please wait..."));
					m_staticInfo.SetWindowText(szMessage);


					iconShow(IDI_INFORMATION);				
					buttonsHideAll();

					progressbarStart();

					countBindings=CBindingConv2Nodes_Api::AddBindingEntriesToBindingFiles(
							&strArray,
							(void*) this,
							&progressCallback);

					progressbarStart();

					countPropertyUnits=CBindingConv2Nodes_Api::AddBindingPropertyToBindingFiles(		
							&strArray2,
							(void*) this,
							&progressCallback);
					
					if ((strArray.IsEmpty())&&(strArray2.IsEmpty()))
					{
						szMessage.Format(_T("Added %d bindings to the nodes\r\n"
										    "Added %d property bindings to the nodes\r\n"),
											countBindings,
											countPropertyUnits);

						m_staticInfo.SetWindowText(szMessage);
					}
					else
					{						
						szMessage.Format(_T("Added %d bindings to the nodes\r\n"
										    "Added %d property bindings to the nodes\r\n"
											"Errors happened while adding the bindings.\r\n"
											"For details view the logfiles in the \"logfiles\" project subdirectory."),
											countBindings,
											countPropertyUnits);

						m_staticInfo.SetWindowText(szMessage);

						iconShow(IDI_ERROR);
					}					

					progessbarEnd();

					m_btnOk.ShowWindow(TRUE);
					m_btnOk.SetWindowText(_T("Next"));

				} break;

				case EVENT_BTN_CONTINUE:
				{
					buttonsHideAll();

					SetState(STATE_MANUALBINDINGS_SEARCH);
					PostEvent(EVENT_NEXT_STEP);
				} break;
			}
		} break;

		case STATE_MANUALBINDINGS_SEARCH:
		{
			const Event_t ev = (Event_t) wParam;
			switch(ev)
			{
				case EVENT_NEXT_STEP:			
				{
					CStringArray stringArray;
					int count;

					CString szMessage(_T("Start searching for manual bindings.\r\n"
										 "Please wait..."));
					m_staticInfo.SetWindowText(szMessage);

					iconShow(IDI_INFORMATION);				
					buttonsHideAll();
					progressbarStart();
					count=CBindingConv2Nodes_Api::SearchManualBindings(&stringArray,(void*) this,&progressCallback);		
					progessbarEnd();

					if(count>0)
					{
						CString szMessage;
						
						szMessage.Format(_T("Found %d manual binding strings.\r\n"
											 "Would you like to see the binding strings ?"),
											 count);
						m_staticInfo.SetWindowText(szMessage);

						iconShow(IDI_QUESTION);				
						m_btnNo.ShowWindow(TRUE);
						m_btnYes.ShowWindow(TRUE);
					}
					else
					{		
						PostEvent(EVENT_NEXT_STEP);
						SetState(STATE_SHUTDOWN);	
					}
				} break;

				case EVENT_BTN_YES:
				{
					CAppFileLogAPI::showLogFileErrors(APPFILELOG_BINDINGFILES_MANUALBINDINGS);

					PostEvent(EVENT_NEXT_STEP);
					SetState(STATE_SHUTDOWN);	
				} break;

				case EVENT_BTN_NO:
				{
					PostEvent(EVENT_NEXT_STEP);
					SetState(STATE_SHUTDOWN);	

				} break;
			}
		} break;

		case STATE_SHUTDOWN:
		{
			const Event_t ev = (Event_t) wParam;
			switch(ev)
			{
				case EVENT_NEXT_STEP:
				{
					PostMessage(WM_CLOSE);	
				} break;
			}
		} break;

		default:
		{
			ASSERT(0);
		} break;
	}
	return res;
}

void CDlgAppBindingsVerify::iconShow(LPSTR strIcon)
{
	HICON icon = AfxGetApp()->LoadStandardIcon(strIcon);
	if (icon !=0)
	{
		m_staticIcon.ShowWindow(TRUE);
		m_staticIcon.SetIcon(icon);
	}
}

void CDlgAppBindingsVerify::progressbarStart(int pos,int max)
{
	m_progressInfo.ShowWindow(TRUE);
	m_progressInfo.SetRange(0,max);
	m_progressInfo.SetPos(pos);	
}

void CDlgAppBindingsVerify::progessbarEnd(int pos)
{
	m_progressInfo.SetPos(pos);	
}

void CDlgAppBindingsVerify::buttonsHideAll(void)
{
	m_btnYes.ShowWindow(FALSE);
	m_btnNo.ShowWindow(FALSE);
	m_btnOk.ShowWindow(FALSE);
}

void CDlgAppBindingsVerify::progressCallback(
	void* pvUserData,
	int step,
	int max)
{
	CDlgAppBindingsVerify* const pDlg = (CDlgAppBindingsVerify*) pvUserData;

	if (0!=pDlg)
	{
		CString szMessage;
		int pos = 0;
		int delay=-1;

		pos = ((step*100)/(max));
		ASSERT(pos>=0 && pos<=100);

		switch(pDlg->m_currState)
		{
			case STATE_BINDINGS_VERIFY:
			{				
				szMessage.Format(_T("Busy verifying binding %d/%d.\r\n"
									"Please wait..."),
									step,
									max);		

				delay = 0;

			} break;

			case STATE_BINDINGS_RESTORE:
			{				
				szMessage.Format(_T("Busy converting binding %d/%d.\r\n"
									"Please wait..."),
									step,
									max);								
											
			} break; /* STATE_BINDINGS_RESTORE */

			case STATE_MANUALBINDINGS_SEARCH:
			{
				szMessage.Format(_T("Busy searching for manual bindings.\r\n"
									"Checking bindingfile %d/%d.\r\n"
									"Plaese wait..."));
			} break;

		} /* switch(m_currState) */

		if(!szMessage.IsEmpty())
		{
			pDlg->m_staticInfo.SetWindowText(szMessage);	
		}

		pDlg->m_progressInfo.SetPos(pos);

		// Indien nodig een kleine delay invoegen zodat de UI-Controls 
		// beter afgebeeld worden...

		if (delay>0)
		{			
			::Sleep(delay);
		}
	}
}

void CDlgAppBindingsVerify::PostEvent(Event_t ev)
{
	PostMessage(WM_FSM_EVENT,ev,0);	
}

void CDlgAppBindingsVerify::SetState(State_t state)
{
	m_currState = state;
}
void CDlgAppBindingsVerify::OnBnClickedButtonYes()
{	
	PostEvent(EVENT_BTN_YES);
}

void CDlgAppBindingsVerify::OnBnClickedButtonNo()
{
	PostEvent(EVENT_BTN_NO);
}

void CDlgAppBindingsVerify::OnBnClickedOk()
{
	switch(m_currState)
	{
		case STATE_STRINGARRAY_SHOW:
		case STATE_BINDINGS_RESTORE:
		{
			PostEvent(EVENT_BTN_CONTINUE);
		} break;

		default:
		{
			OnOK();		
		} break;
	}
}

void CDlgAppBindingsVerify::OnEnterIdle(UINT nWhy, CWnd* pWho)
{
	CDialog::OnEnterIdle(nWhy, pWho);
}

void CDlgAppBindingsVerify::OnEnable(BOOL bEnable)
{
	CDialog::OnEnable(bEnable);
}

void CDlgAppBindingsVerify::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);
}

void CDlgAppBindingsVerify::OnTimer(UINT_PTR nIDEvent)
{
	if (STATE_INIT==this->m_currState)
	{	
		if (this->m_fPaintHandler==TRUE)
		{	
			PostEvent(EVENT_START);
		}
		else
		{
			SetTimer(WM_TIMER_START,TIMER_START_VALUE,(TIMERPROC)NULL);
		}
	}	

	CDialog::OnTimer(nIDEvent);
}

void CDlgAppBindingsVerify::OnPaint()
{
	m_fPaintHandler = TRUE;

	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages
}

void CDlgAppBindingsVerify::OnClose()
{
	if (this->m_currState==STATE_SHUTDOWN)
	{
		CDialog::OnClose();
	}
}
