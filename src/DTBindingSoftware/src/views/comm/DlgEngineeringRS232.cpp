// DlgEngineeringRS232.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgEngineeringRS232.h"

// ------------------------------------
#include "TMGlobals.h"
using namespace TMCommunication;
using namespace TMGlobalHSM;
using namespace TMGlobalSettings;
// ------------------------------------



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
CDlgEngineeringRS232* CDlgEngineeringRS232::m_pThis = NULL;
/////////////////////////////////////////////////////////////////////////////

// CDlgEngineeringRS232 dialog


CDlgEngineeringRS232::CDlgEngineeringRS232(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgEngineeringRS232::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgEngineeringRS232)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_nID = CDlgEngineeringRS232::IDD;
	m_comport = 1;
}


void CDlgEngineeringRS232::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgEngineeringRS232)
	DDX_Control(pDX, IDC_LIST_MESSAGES, m_ListBoxMessages);
	DDX_Control(pDX, IDC_CHECK_CONNECT, m_CheckOnline);
	DDX_Control(pDX, IDC_COMBO_PORT, m_CmbBoxPort);
	DDX_Control(pDX, IDC_LISTBOX, m_ListBox);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgEngineeringRS232, CDialog)
	//{{AFX_MSG_MAP(CDlgEngineeringRS232)
	ON_BN_CLICKED(IDC_CHECK_CONNECT, OnCheckConnect)
	ON_BN_CLICKED(IDC_BUTTON_RESET, OnButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_STATUS, OnButtonStatus)
	//}}AFX_MSG_MAP

	ON_MESSAGE(WM_UPDATE_CONTROLS,OnUpdateControls)
	ON_MESSAGE(WM_COMMUNICATION_MODEM_STATUS,OnCommunicationModemStatus)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgEngineeringRS232 message handlers
void CDlgEngineeringRS232::Show(void)
{
	if (m_pThis == NULL) {
		m_pThis = new CDlgEngineeringRS232();	
		if (m_pThis->Create() == TRUE) {
			m_pThis->ShowWindow(TRUE);
		}
	}
	else  {
		m_pThis->SetActiveWindow();
	}

}

void CDlgEngineeringRS232::Add(CString s)
{
	if (m_pThis != NULL)
	{
		m_pThis->AddMessage(s);
	}
}
void CDlgEngineeringRS232::AddInfo(CString s)
{
	if (m_pThis != NULL)
	{
		m_pThis->AddMessageInfo(s);
	}
}
///////////////////////////////////////////////////////////////////////////
void CDlgEngineeringRS232::AddMessage(CString szData)
{
   CString szMessage;

   CTime t = CTime::GetCurrentTime();
   szMessage = t.Format( "%d %b, %H:%M:%S " );  
   szMessage += szData;

   m_ListBox.InsertString(m_ListBox.GetCount(),szMessage);
   m_ListBox.SetTopIndex(m_ListBox.GetCount()-10);

   if (m_ListBox.GetCount() >= 10000)
	   m_ListBox.ResetContent();
}

void CDlgEngineeringRS232::AddMessageInfo(CString szData)
{ 
   m_ListBoxMessages.InsertString(m_ListBoxMessages.GetCount(),szData);
   m_ListBoxMessages.SetTopIndex(m_ListBoxMessages.GetCount()-10);

   if (m_ListBoxMessages.GetCount() >= 10000)
	   m_ListBoxMessages.ResetContent();
}

void CDlgEngineeringRS232::ClearInfo()
{
	if (m_pThis != NULL)
	{
		m_pThis->m_ListBoxMessages.ResetContent();
	}
}

void CDlgEngineeringRS232::OnCheckConnect() 
{	
	m_CheckOnline.SetCheck(FALSE);
	if (TMIsConnected() == FALSE) 
	{	
		int nComPort = 0; 
		const int index = m_CmbBoxPort.GetCurSel();		// CHANGES_MVS_2008
		if (index != CB_ERR) {
			nComPort = m_CmbBoxPort.GetItemData(index);
			nComPort++;
		}				
		if ((nComPort >0 ) && (nComPort <= 9)) {					
			m_comport = nComPort;
			fBusyConnecting = TRUE;
			TMGetCommunicationApp()->setSerialUsbConnect(m_comport);
		}	
	}
	else 
	{
		TMGetGlobalHSM()->EvOperationCommunicationOffline();
		fConnected = FALSE;
	}
	PostMessage( WM_UPDATE_CONTROLS );
	
}

void CDlgEngineeringRS232::OnOK() 
{	
	DestroyWindow();
}

void CDlgEngineeringRS232::PostNcDestroy() 
{
	delete this;
	m_pThis = NULL;
}

// Solved BUG-0143
BOOL CDlgEngineeringRS232::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	enum {MAX_COM_PORTS=255};

	UINT nComPort = 1;	
	BOOL fInit = FALSE;

	m_CmbBoxPort.ResetContent();

	if (TMGetCommunicationSettings() != 0) {
		// De load is hier niet nodig want is alg gebeurd.
		int nCount = 0;
		BYTE bComports[MAX_COM_PORTS];	
		if (TMGetCommunicationSettings()->GetAvailableComPorts(&bComports[0],&nCount)== TRUE) {
			if (nCount>0) {
				for (int i=0;i<nCount;i++) {
					CString s;
					int index;
					s.Format("COM%d",bComports[i]+1);
					if ((index = m_CmbBoxPort.AddString(s)) != CB_ERR) {
						m_CmbBoxPort.SetItemData(index,bComports[i]);
					}
				}			
				fInit = TRUE;			
			}
		}
	}	

	/*******************************************************************
	 * If no COM Ports can be found then show default: COM1 - COM10
	 * When attaching the USB-Device the List is updated and only the
	 * USB port will be displayed.
	 *******************************************************************/
	if (fInit == FALSE) {
		CString s;
		int index;
		for (int i=0;i<MAX_COM_PORTS-1;i++) {
			s.Format("PORT%d",i+1);
			if ((index = m_CmbBoxPort.AddString(s)) != CB_ERR)
			{
				m_CmbBoxPort.SetItemData(index,i);
			}		
		}		
	}

	if (TMGetCommunicationSettings() != NULL) {
		if (TMGetCommunicationSettings()->GetDefaultComPort() > 0) {
			nComPort = TMGetCommunicationSettings()->GetDefaultComPort();			
		}
	}	
	
	fConnected = TMIsConnected();
	if (fConnected == FALSE) {		
		m_comport = nComPort;
	}

	m_CmbBoxPort.SetCurSel(0);
	for (int i=0;i<m_CmbBoxPort.GetCount();i++) {
		if (m_CmbBoxPort.GetItemData(i) == m_comport-1) {
			m_CmbBoxPort.SetCurSel(i);
			break;
		}
	}
	
	fBusyConnecting = FALSE;
	PostMessage(WM_UPDATE_CONTROLS);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LONG CDlgEngineeringRS232::OnUpdateControls(WPARAM wParam,LPARAM lParam)
{
	if (fBusyConnecting == TRUE)
	{
		m_CheckOnline.EnableWindow(FALSE);
		m_CmbBoxPort.EnableWindow(FALSE);	
	}
	else
	{
		m_CheckOnline.EnableWindow(TRUE);
		m_CheckOnline.SetCheck(fConnected);

		if (fConnected == TRUE) {	
			m_CmbBoxPort.EnableWindow(FALSE);
		}
		else {
			m_CmbBoxPort.EnableWindow(TRUE);
		}
	}
	return TRUE;
}

LONG CDlgEngineeringRS232::OnCommunicationModemStatus(WPARAM wParam,LPARAM lParam)
{
	fBusyConnecting = FALSE;
	fConnected = TMIsConnected();
	PostMessage( WM_UPDATE_CONTROLS );
	return TRUE;
}

void CDlgEngineeringRS232::OnButtonReset() 
{
	TMGetCommunicationApp()->ResetCommunication();
}

void CDlgEngineeringRS232::OnButtonStatus() 
{	
	TMGetCommunicationApp()->RequestModemStatus();
}
