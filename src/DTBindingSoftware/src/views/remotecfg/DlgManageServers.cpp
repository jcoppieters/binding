// DlgManageServers.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgManageServers.h"
#include "afxdialogex.h"

using namespace DUOTECNO;


// CDlgManageServers dialog

enum {
	WM_UPDATE_CONTROLS = WM_USER + 1
};


IMPLEMENT_DYNAMIC(CDlgManageServers, CDialog)

CDlgManageServers::CDlgManageServers(CWnd* pParent, DUOTECNO::IAppRemoteCfgApi& refRemoteCfg)
	: CDialog(IDD_DIALOG_SERVERS, pParent), m_refRemoteCfg(refRemoteCfg), m_mode(MODE_VIEW)
{

}

CDlgManageServers::~CDlgManageServers()
{
}

void CDlgManageServers::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_SERVER, m_comboServers);
	DDX_Control(pDX, IDC_EDIT_NAME, m_editName);
	DDX_Control(pDX, IDC_EDIT_HOSTNAME, m_editHostName);
	DDX_Control(pDX, IDC_EDIT_PORT, m_editPort);
	DDX_Control(pDX, IDC_CHECK_SECURED, m_checkSecured);
	DDX_Control(pDX, IDC_EDIT_PASSWORD2, m_editPassword);
	DDX_Control(pDX, IDC_STATIC_PWD, m_staticPassword);
	DDX_Control(pDX, IDC_BUTTON_ADJUST, m_btnAdjust);
	DDX_Control(pDX, IDC_BUTTON_ADD, m_btnAdd);
	DDX_Control(pDX, IDC_BUTTON_REMOVE, m_btnRemove);
}



BEGIN_MESSAGE_MAP(CDlgManageServers, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CDlgManageServers::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE, &CDlgManageServers::OnBnClickedButtonRemove)
	ON_BN_CLICKED(IDC_BUTTON_ADJUST, &CDlgManageServers::OnBnClickedButtonAdjust)
	ON_BN_CLICKED(IDC_CHECK_SECURED, &CDlgManageServers::OnBnClickedCheckSecured)
	ON_CBN_SELCHANGE(IDC_COMBO_SERVER, &CDlgManageServers::OnCbnSelchangeComboServer)
	ON_MESSAGE(WM_UPDATE_CONTROLS, onWmUpdateControls)
END_MESSAGE_MAP()


// CDlgManageServers message handlers
void CDlgManageServers::OnBnClickedButtonAdd()
{
	AppRemoteCfgServer serverInfo;
	if (getDataControls(&serverInfo)) {	
		int count = 0;
		if (!appremoteCfg_serversAdd(&m_refRemoteCfg,serverInfo,&count)) {
			count = 0;
		}	
		// Updaten van de combobox.
		if (m_currentSelected < count-1) {
			m_currentSelected++;
			m_comboServers.SetCurSel(m_currentSelected);
		}
		// Updaten van de combobox - tot hier
	}	
	m_mode = MODE_VIEW;
	this->PostMessage(WM_UPDATE_CONTROLS,1);				// wParam = Clear Combo list box : 0=false, 1=true
}

void CDlgManageServers::OnBnClickedButtonRemove()
{
	CString hostName;
	CString hostPort;

	m_editHostName.GetWindowText(hostName);
	m_editPort.GetWindowText(hostPort);

	if (hostName.IsEmpty() || hostPort.IsEmpty()) {
		AfxMessageBox("Host name & port cannot be empty");
		return;
	}
	int port = -1;
	if (sscanf((LPCSTR)hostPort, "%i", &port)<1) {
		port = -1;
	}
	if (port <= 0) {
		AfxMessageBox("Host port is invalid value");
		return;
	}

	// Updaten van de combobox.
	int count = 0;
	if (!appremoteCfg_serversRemove(&m_refRemoteCfg,hostName,port,&count)) {
		count = 0;
	}
	if (count <= 0) {
		m_currentSelected = -1;
	}
	else {
		m_currentSelected = count-1;
	}
	m_comboServers.SetCurSel(m_currentSelected);
	// Updaten van de combobox - tot hier...

	m_mode = MODE_VIEW;
	this->PostMessage(WM_UPDATE_CONTROLS,1);		// wParam = Clear Combo list box : 0=false, 1=true
}

void CDlgManageServers::OnBnClickedButtonAdjust()
{
	m_mode = MODE_EDIT;
	this->PostMessage(WM_UPDATE_CONTROLS,0);		// wParam = Clear Combo list box : 0=false, 1=true
}

void CDlgManageServers::OnCbnSelchangeComboServer() 
{
	m_currentSelected = m_comboServers.GetCurSel();
	this->PostMessage(WM_UPDATE_CONTROLS,0);		// wParam = Clear Combo list box : 0=false, 1=true
}

void CDlgManageServers::OnBnClickedCheckSecured()
{
	const bool show = (BST_CHECKED == m_checkSecured.GetCheck() ? true : false);
	m_staticPassword.ShowWindow(show);
	m_editPassword.ShowWindow(show);
}

BOOL CDlgManageServers::OnInitDialog() {

	m_currentSelected = -1;
	m_mode = MODE_EDIT;

	int count = 0;
	if (!appremotecfg_serversGetCount(&m_refRemoteCfg,&count)) {
		count = 0;
	}
	if (count>0) {
		m_currentSelected = 0;
		m_mode = MODE_VIEW;
	}	
	this->PostMessage(WM_UPDATE_CONTROLS,1);	
	return this->CDialog::OnInitDialog();
}

// Data ophalen vanuit de controls...
BOOL CDlgManageServers::getDataControls(AppRemoteCfgServer* p) {

	CString friendlyName;
	CString hostName;
	CString password;
	CString hostPort;
	BOOL fSecured = FALSE;
	
	m_editName.GetWindowText(friendlyName);
	m_editHostName.GetWindowText(hostName);
	m_editPassword.GetWindowText(password);
	m_editPort.GetWindowText(hostPort);
	if (BST_CHECKED == m_checkSecured.GetCheck()) {
		fSecured = TRUE;
	}

	if ((friendlyName.IsEmpty() || hostName.IsEmpty() || hostPort.IsEmpty()) || (fSecured && password.IsEmpty()))
	{
		AfxMessageBox("Fields are empty");
		return FALSE;
	}

	int port = -1;
	if (sscanf((LPCSTR)hostPort,"%i",&port)<1) {
		port = -1;
	}
	if (port<=0) {
		AfxMessageBox("Host port is invalid value");
		return FALSE;
	}

	p->setFriendlyName(friendlyName);
	p->setHostName(hostName);
	p->setPassword(password);
	p->setSecured(fSecured == TRUE ? true : false);
	p->setHostPort(port);
	return TRUE;
}

// Controls aanpassen met de data...
BOOL CDlgManageServers::updateDataControls(const DUOTECNO::AppRemoteCfgServer* pServer) {

	bool fSecured = false;

	m_btnAdjust.ShowWindow(m_mode == MODE_VIEW);
	m_btnAdd.ShowWindow(m_mode == MODE_EDIT);
	m_btnRemove.ShowWindow(m_mode == MODE_EDIT);

	m_editName.EnableWindow(m_mode == MODE_EDIT);
	m_editHostName.EnableWindow(m_mode == MODE_EDIT);
	m_editPassword.EnableWindow(m_mode == MODE_EDIT);
	m_checkSecured.EnableWindow(m_mode == MODE_EDIT);
	m_editPassword.EnableWindow(m_mode == MODE_EDIT);
	m_editPort.EnableWindow(m_mode == MODE_EDIT);
	m_comboServers.EnableWindow(m_mode == MODE_VIEW);

	if (0 != pServer) {
	
		m_editName.SetWindowText(pServer->getFriendlyName());
		m_editHostName.SetWindowText(pServer->getHostName());
		m_editPassword.SetWindowText(pServer->getPassword());

		CString strPort;
		strPort.Format("%d", pServer->getHostPort());
		m_editPort.SetWindowText(strPort);

		fSecured = pServer->getSecured();
		m_checkSecured.SetCheck(fSecured == true ? BST_CHECKED : BST_UNCHECKED);

		m_editPassword.ShowWindow(fSecured == true ? 1 : 0);
		m_staticPassword.ShowWindow(fSecured == true ? 1 : 0);
	}
	else {
		m_editName.SetWindowText("");
		m_editHostName.SetWindowText("");
		m_editPassword.SetWindowText("");
		m_editPassword.SetWindowText("");
		m_editPort.SetWindowText("");

		fSecured = m_checkSecured.GetCheck() == BST_CHECKED ? true : false;
	}

	m_editPassword.ShowWindow(fSecured == true ? 1 : 0);
	m_staticPassword.ShowWindow(fSecured == true ? 1 : 0);
	return TRUE;
}

// Oppassen: wParam bepaalt of de combobox items moeten gewist worden...
LONG CDlgManageServers::onWmUpdateControls(WPARAM wParam, LPARAM lParam) {

	if (1 == wParam ) {
		helper_ComboBoxloadItems();
	}

	int count = 0;
	if (!appremotecfg_serversGetCount(&m_refRemoteCfg,&count)) {
		count = 0;
	}

	if (count <= 0) {
		m_currentSelected = -1;
	}
	else {
		if (m_currentSelected>=count) {
			m_currentSelected = 0;			
		}	
	}

	// Weergave van selected...
	if (m_currentSelected >=0) {
		AppRemoteCfgServer* pServerInfo = 0;
		if (appremoteCfg_serversGetIndex(&m_refRemoteCfg,m_currentSelected,&pServerInfo)) {
			updateDataControls(pServerInfo);		
			m_comboServers.SetCurSel(m_currentSelected);
		}
		else {
			m_currentSelected = -1;
		}
	}
	else {
		updateDataControls(0);
	}
	return TRUE;
}


void CDlgManageServers::helper_ComboBoxloadItems() {

	m_comboServers.ResetContent();

	CStringList stringList;
	if (appremotecfg_serversGetFriendlyNames(&m_refRemoteCfg,&stringList)) {
		int index = 0;
		if (stringList.GetSize()>0) {
			POSITION pos = stringList.GetHeadPosition();
			while(pos!=NULL) {
				CString& name = stringList.GetNext(pos);
				m_comboServers.InsertString(index,name);
				++index;
			}
		}
	}
}

