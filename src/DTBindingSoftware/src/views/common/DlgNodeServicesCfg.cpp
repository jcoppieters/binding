// DlgNodeServicesCfg.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgNodeServicesCfg.h"
#include "cannode.h"

#include "utils/mfc/controls_util.h"
using namespace DUOTECNO::MFC_HELPER;


// CDlgNodeServicesCfg dialog

IMPLEMENT_DYNAMIC(CDlgNodeServicesCfg, CDialog)

CDlgNodeServicesCfg::CDlgNodeServicesCfg(CWnd* pParent,IAppDlgNodeServicesCfg* pHost, CCanNode* pCanNode /*=NULL */)
	: CDialog(CDlgNodeServicesCfg::IDD, pParent), m_pHost(pHost) , m_pCanNode(pCanNode)
{

}

CDlgNodeServicesCfg::~CDlgNodeServicesCfg()
{
}

void CDlgNodeServicesCfg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_NODENAME, m_editNodeName);
	DDX_Control(pDX, IDC_EDIT_NODE_PWD, m_editPassword);
	DDX_Control(pDX, IDC_EDIT_LOCALIP, m_CtrlLocalIPAddress);
	DDX_Control(pDX, IDC_EDIT_LOCALPORT, m_editLocalPort);
	DDX_Control(pDX, IDC_EDIT_LOCALPORT2, m_editRemoteDns);
	DDX_Control(pDX, IDC_EDIT_REMOTEPORT, m_editRemotePort);
	DDX_Control(pDX, IDOK, m_btnOk);
}


BEGIN_MESSAGE_MAP(CDlgNodeServicesCfg, CDialog)
	ON_BN_CLICKED(IDCANCEL, &CDlgNodeServicesCfg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CDlgNodeServicesCfg::OnBnClickedOk)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDlgNodeServicesCfg message handlers
BOOL CDlgNodeServicesCfg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_btnOk.EnableWindow(FALSE);

	if ((0 != m_pCanNode) && (0 != m_pHost)) {
	
		const int nodeAddress = m_pCanNode->getNodeAddress();

		{
			// Updaten van de node name...
			const CString nodeName = m_pCanNode->getNodeName();
			m_editNodeName.SetWindowTextA(nodeName);
		}
	
		{
			// Updaten van het password...
			CString password;
			if (!m_pHost->getNodePassword(nodeAddress,&password)) {
				password = CString();
			}
			m_editPassword.SetWindowText(password);
		}

		{
			// Updaten lokale ip-settings.
			CString hostNameLocal;
			int portLocal = -1;
			int values[4] = { -1,-1,-1,-1 };
			if (m_pHost->getLocalIPSettings(nodeAddress,&hostNameLocal,&portLocal)) {				
				if (!hostNameLocal.IsEmpty()) {
					if (4 != sscanf(hostNameLocal.GetBuffer(0),"%d.%d.%d.%d",&values[0],&values[1],&values[2],&values[3])) {
						values[0] = -1;
						values[1] = -1;
						values[2] = -1;
						values[3] = -1;
					}
				}						
			}

			if ((values[0]>=0 && values[0]<=255) && 
				(values[1]>=0 && values[1]<=255) &&
				(values[2]>=0 && values[2]<=255) &&
				(values[3]>=0 && values[3]<=255)) 
			{
				m_CtrlLocalIPAddress.SetAddress(values[0],values[1],values[2],values[3]);	
			}
			else {
				m_CtrlLocalIPAddress.ClearAddress();
			}

			if (portLocal>=0) {
				CString s;
				s.Format("%d",portLocal);
				m_editLocalPort.SetWindowText(s);
			}
		}
	
		{
			// Updaten remote settings...
			CString hostNameRemote;		
			int portRemote = -1;

			if (!m_pHost->getRemoteIPSettings(nodeAddress,&hostNameRemote,&portRemote)) {			
				portRemote = -1;
				hostNameRemote = CString();
			}

			// Sedert V16.28: 
			// Indien we geen remote hostName gevonden hebben voor deze node dan nog eens zoeken in de lijst bij de andere nodes.
			// Normaal is er per installatie 1 dynDns ingesteld.
			if (hostNameRemote.IsEmpty()) {
				if (!m_pHost->searchRemoteDns(nodeAddress,&hostNameRemote)) {
					hostNameRemote = CString();
				}
			}
			
			m_editRemoteDns.SetWindowText(hostNameRemote);
			if (portRemote>=0) {
				CString s;
				s.Format("%d",portRemote);
				m_editRemotePort.SetWindowText(s);
			}
		}
	}

	SetTimer(WM_TIMER_UPDATE_BTN_SAVE,100,(NULL));
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}



// Btn ok enkel enablen wannneer:
// - Password ingevuld.
// - Lokale of remote settings ingevuld.
void CDlgNodeServicesCfg::OnTimer(UINT nIDEvent) {

	KillTimer(nIDEvent);

	if (WM_TIMER_UPDATE_BTN_SAVE == nIDEvent) {
		
		BOOL enable = TRUE;
		CString pwd;
		if (!CControls_Utils::getPassword(m_editPassword,&pwd)) {
			enable = FALSE;
		}
		else {
			BOOL localSettings = FALSE;		
			{						
				int port;
				if (CControls_Utils::getNwkIPPort(m_editLocalPort,&port)) {
					CString ipAddress;
					if (CControls_Utils::getNwkHostAddress(m_CtrlLocalIPAddress,&ipAddress)) {
						localSettings = TRUE;	
					}
				}
			}
			BOOL remoteSettings = FALSE;
			if (!localSettings) {
				int port;
				if (CControls_Utils::getNwkIPPort(m_editRemotePort,&port)) {
					CString ipAddress;
					if (CControls_Utils::getNwkHostAddress(m_editRemoteDns,&ipAddress)) {
						remoteSettings = TRUE;	
					}
				}
			}
			enable = (localSettings || remoteSettings ? TRUE : FALSE);
		}		
		m_btnOk.EnableWindow(enable);
		SetTimer(WM_TIMER_UPDATE_BTN_SAVE,300,(NULL));
	}
}


void CDlgNodeServicesCfg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

void CDlgNodeServicesCfg::OnBnClickedOk()
{
	if ((0 != m_pCanNode) && (0 != m_pHost)) {	
		const int nodeAddress = m_pCanNode->getNodeAddress();
	
		CString pwd;
		CString ipAddress;
		CString dns;
		int portLocal = -1;
		int portRemote = -1;

		// Opvragen van het password.
		if (!CControls_Utils::getPassword(m_editPassword,&pwd)) {
			pwd.Empty();
		}

		// Opvragen van de locale ip-settings.
		BOOL localSettings = FALSE;											
		if (CControls_Utils::getNwkIPPort(m_editLocalPort,&portLocal)) {	
			if (CControls_Utils::getNwkHostAddress(m_CtrlLocalIPAddress,&ipAddress)) {
				localSettings = TRUE;	
			}
		}	

		// Opvragen van de dns remote ip-settings.
		BOOL remoteSettings = FALSE;	
		if (CControls_Utils::getNwkIPPort(m_editRemotePort,&portRemote)) {	
			if (CControls_Utils::getNwkHostAddress(m_editRemoteDns,&dns)) {
				remoteSettings = TRUE;	
			}
		}
		
		// Updaten van de node services...
		if (!pwd.IsEmpty()) {			
			if (!localSettings) {
				portLocal = -1;
				ipAddress = CString();
			}
			if (!remoteSettings) {
				portRemote = -1;
				dns = CString();
			}			
			m_pHost->updateNodeSettings(nodeAddress,pwd,ipAddress,portLocal,dns,portRemote);
		}	
	}
	OnOK();
}