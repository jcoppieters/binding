// DlgModemStatus.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgModemStatus.h"
#include "ComApplication.h"

// ----------------------------
#include "TMGlobals.h"
using namespace TMCommunication;
// ----------------------------

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
CDlgModemStatus* CDlgModemStatus::m_pDlg = NULL;
/////////////////////////////////////////////////////////////////////////////
// CDlgModemStatus dialog

void CDlgModemStatus::Show(void)
{
	if (m_pDlg == NULL) 
	{
		m_pDlg = new CDlgModemStatus();
	
		if (m_pDlg->Create() == TRUE)
		{
			m_pDlg->ShowWindow(TRUE);
		}
	}
	else 
	{
		m_pDlg->SetActiveWindow();
	}
}

void CDlgModemStatus::Update()
{
	if (m_pDlg != NULL)	
		m_pDlg->PostMessage(WM_UPDATE_CONTROLS);	
}
/////////////////////////////////////////////////////////////////////////////
CDlgModemStatus::CDlgModemStatus(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgModemStatus::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgModemStatus)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_nID = CDlgModemStatus::IDD;
}


void CDlgModemStatus::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgModemStatus)
	DDX_Control(pDX, IDC_EDIT_ADDRESS, m_EditAddress);
	DDX_Control(pDX, IDC_EDIT_V24STATE, m_EditV24State);
	DDX_Control(pDX, IDC_EDIT_CURRENTSTATE, m_EditCurrentState);
	DDX_Control(pDX, IDC_EDIT_VERSION, m_EditVersion);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgModemStatus, CDialog)
	//{{AFX_MSG_MAP(CDlgModemStatus)
	ON_BN_CLICKED(IDC_BUTTON_RESET, OnButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_STATUS, OnButtonStatus)
	ON_BN_CLICKED(IDC_BUTTON_ADDRESS, OnButtonAddress)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_VERSION, OnButtonVersion)
	//}}AFX_MSG_MAP

	ON_MESSAGE(WM_UPDATE_CONTROLS,OnUpdateControls)
	ON_MESSAGE(WM_CLEAR_CONTROLS,OnClearControls)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgModemStatus message handlers

void CDlgModemStatus::PostNcDestroy() 
{	
	delete this;
	m_pDlg = NULL;
}

void CDlgModemStatus::OnOK() 
{
	DestroyWindow();	
}


void CDlgModemStatus::OnButtonReset() 
{
	CString szMessage("Are you sure you wan't to reset the modem");
	
	if (AfxMessageBox(szMessage, MB_ICONQUESTION | MB_YESNO) == IDYES)
	{	
		TMGetCommunicationApp()->ResetCommunication();
		PostMessage(WM_CLEAR_CONTROLS);	
	}
}

void CDlgModemStatus::OnButtonStatus() 
{
	TMGetCommunicationApp()->RequestModemStatus();	
	PostMessage(WM_CLEAR_CONTROLS);
}

void CDlgModemStatus::OnButtonVersion() 
{
	TMGetCommunicationApp()->RequestModemVersion( );	/** @since 0x0562 */
}

BOOL CDlgModemStatus::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	TMGetCommunicationApp()->RequestModemStatus();
	PostMessage(WM_CLEAR_CONTROLS);

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


LRESULT CDlgModemStatus::OnUpdateControls(WPARAM wParam,LPARAM lParam)
{		
	const CSerialCanModemErrorStatus *const pModemErrorStatus = TMGetModemErrorStatus();				
	if (pModemErrorStatus != 0)
	{
		if (pModemErrorStatus->GetFatalErrorFlag())
		{
			PostMessage(WM_CLEAR_CONTROLS);
		}
		else
		{				
			const CCommunicationServerInfo& serverInfo = TMCommunicationGetStatus().getServerInfo();
			const CCommunicationServerInfo::type_t type = serverInfo.getCommunicationType();			
			CString s("----");
			if (CCommunicationServerInfo::SERIAL == type) {										
				const BYTE bSoftwareVersion = (serverInfo.getSoftwareVersion() & 0xFF00) >> 8;
				const BYTE bSoftwareVersionRev = (serverInfo.getSoftwareVersion() & 0xFF);								
				if (0xFF != bSoftwareVersion) {						
					if (0xFF != bSoftwareVersionRev) {
						s.Format("V%02x.%02x",bSoftwareVersion,bSoftwareVersionRev);
					}
					else {
						s.Format("V%02x",bSoftwareVersion);									 
					}
				}														
			}
			m_EditVersion.SetWindowText(s);	

			const CSerialCanModem *const pCanModem = TMGetModemInfo();		
			if (pCanModem != NULL)
			{
				// Display current status ...
				pCanModem->GetStatus(s);
				m_EditCurrentState.SetWindowText(s);

				// Display v24 status ...
				pCanModem->GetV24Status(s);
				m_EditV24State.SetWindowText(s);


				const BYTE bNodeAddress = serverInfo.getNodeAddress();
				if (0xFF != bNodeAddress) {
					s.Format("0x%02x",bNodeAddress);
				}
				else {
					s = CString("---");
				}				
				m_EditAddress.SetWindowText(s);	
			}						
		}
	}					
	TRACE("Update Controls");	
	return TRUE;
}

LONG CDlgModemStatus::OnClearControls(WPARAM wParam,LPARAM lParam)
{
	CString s("-----");
	
	m_EditVersion.SetWindowText(s);		
	m_EditCurrentState.SetWindowText(s);
	m_EditV24State.SetWindowText(s);
	m_EditAddress.SetWindowText(s);

	return TRUE;
}

void CDlgModemStatus::OnButtonAddress() 
{
	TMGetCommunicationApp()->RequestModemAddress();	
}

void CDlgModemStatus::OnClose() 
{
	DestroyWindow();	
}


