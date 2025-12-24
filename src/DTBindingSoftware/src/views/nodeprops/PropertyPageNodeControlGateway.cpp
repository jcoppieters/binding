// PropertyPageNodeControlGateway.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "PropertyPageNodeControlGateway.h"

#include "NodeDatabase.h"
#include "CanNode.h"
#include "nodegenestructs.h"
#include "TXNodeMessage.h"
#include "MyWindowTimers.h"
#include "NodeRuntimeStatus.h"
#include "TMAlgoritmesCfg.h"
#include "app\appCfg\AppCfginfo\cfginfonode.h"	
#include "app\appipc\appipccfgtool\ipluginconfigtoolapp.h"		

/////////////////////////////////////////////////////////////////////////////
#include "TMGlobals.h"
using namespace TMCommunication;
/////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropertyPageNodeControlGateway property page

IMPLEMENT_DYNCREATE(CPropertyPageNodeControlGateway, CPropertyPage)		// CHANGES_MVS_2008 : Afleiden van CPropertyPage

CPropertyPageNodeControlGateway::CPropertyPageNodeControlGateway() : 
CPropertyPageNode(CPropertyPageNodeControlGateway::IDD)
{
	//{{AFX_DATA_INIT(CPropertyPageNodeControlGateway)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	ASSERT( m_pSelNode );
}

CPropertyPageNodeControlGateway::CPropertyPageNodeControlGateway(CCanNode *pNode):
CPropertyPageNode(CPropertyPageNodeControlGateway::IDD,pNode),
fInitControls(FALSE)
{			
	ASSERT(m_pSelNode);
	ASSERT(fInitControls == FALSE);
}

CPropertyPageNodeControlGateway::~CPropertyPageNodeControlGateway()
{
}

void CPropertyPageNodeControlGateway::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropertyPageNodeControlGateway)
	DDX_Control(pDX, IDC_EDIT_STATE, m_EditState);
	DDX_Control(pDX, IDC_BTN_REQUEST, m_BtnRequestState);
	DDX_Control(pDX, IDC_BTN_CLEAR, m_BtnEraseDatabase);
	DDX_Control(pDX, IDC_BTN_CLEAR_EEPROM, m_BtnClearEeprom);
	DDX_Control(pDX, IDC_BTN_RESET_FACTORY, m_BtnResetFactory);
	DDX_Control(pDX, IDC_BTN_RESET, m_BtnReset);
	DDX_Control(pDX, IDC_CHECK_UNLOCK, m_CheckUnlock);
	DDX_Control(pDX, IDC_CHECK_SERIALIZE, m_CheckSerialize);
	DDX_Control(pDX, IDC_STATIC_SYSTEMLOG, m_staticSystemLog);
	DDX_Control(pDX, IDC_BTN_DOWNLOAD_SYSTEMLOG, m_btnDownloadSystemLog);
	DDX_Control(pDX, IDC_STATIC_FIRMWARE, m_staticFirmware);
	DDX_Control(pDX, IDC_BTN_FIRMWARE, m_btnFirmware);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropertyPageNodeControlGateway, CPropertyPage)
	//{{AFX_MSG_MAP(CPropertyPageNodeControlGateway)
	ON_BN_CLICKED(IDC_BTN_DOWNLOAD_SYSTEMLOG, &CPropertyPageNodeControlGateway::OnBnClickedBtnDownloadSystemlog)
	ON_BN_CLICKED(IDC_BTN_CLEAR_EEPROM, OnBtnClearEeprom)
	ON_BN_CLICKED(IDC_BTN_RESET, OnBtnReset)
	ON_BN_CLICKED(IDC_BTN_RESET_FACTORY, OnBtnResetFactory)
	ON_BN_CLICKED(IDC_CHECK_SERIALIZE, OnCheckSerialize)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_REQUEST, OnBtnRequest)
	ON_BN_CLICKED(IDC_BTN_CLEAR, OnBtnClear)
	ON_BN_CLICKED(IDC_BTN_FIRMWARE, &CPropertyPageNodeControlGateway::OnBnClickedBtnFirmware)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyPageNodeControlGateway message handlers
void CPropertyPageNodeControlGateway::UpdateStaticControls(void)
{	
	m_BtnClearEeprom.EnableWindow(FALSE);
	m_BtnResetFactory.EnableWindow(FALSE);
	m_CheckUnlock.EnableWindow(FALSE);
	m_BtnRequestState.EnableWindow(TRUE);
	m_BtnEraseDatabase.EnableWindow(TRUE);

	if (1)
	{
		BOOL visible = FALSE;
		if (TRUE == TMIsConnected()) {
			if (CNodeSystemLog_Util::isSupported(m_pSelNode,CNodeSystemLog_Util::FILE_ID_SYSTEMLOG)) {
				visible = TRUE;
			}
		}

		m_staticSystemLog.EnableWindow(visible);
		m_btnDownloadSystemLog.EnableWindow(visible);
	}

	if (1)	// Firmware upgrade.
	{
		BOOL visible = FALSE;
		if (CNodeFirmware_Util::isSupported(m_pSelNode)) {
			visible = TRUE;
		}
		m_staticFirmware.ShowWindow(visible);
		m_btnFirmware.ShowWindow(visible);
	}
}

void CPropertyPageNodeControlGateway::UpdateRunTimeControls(void)
{
	ASSERT(m_pSelNode);

	if (fInitControls == TRUE) {
	
		if (m_pSelNode->bDLLAccessControl & APPL_UNLOCK) 
		{
			m_CheckUnlock.SetCheck(1);
		}
		else 
		{
			m_CheckUnlock.SetCheck(0);
		}

		if (m_pSelNode->bDLLAccessControl & APPL_SERIALIZE) 
		{
			m_CheckSerialize.SetCheck(1);
		}
		else 
		{
			m_CheckSerialize.SetCheck(0);	
		}

		if (m_pSelNode->GetRuntimeStatus() != 0 && 
			m_pSelNode->bNodeType == NODE_PIN_GATEWAY)
		{
			CString szTemp;
			static_cast <CNodeGatewayRuntimeStatus*> 
				(m_pSelNode->GetRuntimeStatus())->ToString(szTemp);
			
			m_EditState.SetWindowText(szTemp);
		}
	}
}

void CPropertyPageNodeControlGateway::RequestRuntimeStatussen(void)
{
	ASSERT(m_pSelNode);

	CTXBrowsePnpMessages::getInstance()->
		RequestNodeConfig(m_pSelNode->bLogicalAddress);

	CTXNodeGatewayMessages::getInstance()->
		RequestState(m_pSelNode->bLogicalAddress);
}


BOOL CPropertyPageNodeControlGateway::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();		// CHANGES_MVS_2008 : Afleiden van CPropertyPage

	ASSERT(m_pSelNode->GetRuntimeStatus());
	ASSERT(m_pSelNode->bNodeType == NODE_PIN_GATEWAY);
	
	fInitControls = TRUE;

	UpdateStaticControls();
	UpdateRunTimeControls();

	RequestRuntimeStatussen();	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPropertyPageNodeControlGateway::OnBtnClearEeprom() 
{
	CString szMessage;
	szMessage.Format(_T("Are you sure you want to clear the EEPROM of node '%s'?\n"
						"This actions will clear all names, all bindings, all nodeflags...\n"
						"The node will lose his logicalAddress 0x%02x !"),
						m_pSelNode->szNodeName,
						m_pSelNode->bLogicalAddress);

	if (AfxMessageBox(szMessage,MB_OKCANCEL | MB_ICONQUESTION) == IDOK) 
	{
		CTXNodeMessage::getInstance()->
			FactoryResetNode(m_pSelNode->bLogicalAddress);

		// BE CAREFULLY ...
		// --> SHOULD WE REMOVE NODE FROM DATABASE ...
	}
}

void CPropertyPageNodeControlGateway::OnBtnReset() 
{
	CString szMessage;
	szMessage.Format(_T("Reset Node '%s'?\n"),m_pSelNode->szNodeName);
					
	if (AfxMessageBox(szMessage,MB_OKCANCEL | MB_ICONQUESTION) == IDOK) 
	{
		CTXNodeMessage::getInstance()->
			ResetNode(m_pSelNode->bLogicalAddress);
	}
}

void CPropertyPageNodeControlGateway::OnBtnResetFactory() 
{
	CString szMessage;
	szMessage.Format(_T("Are you sure you want to give node '%s' a factory init ?\n"
						"This actions will clear all names, all bindings, all nodeflags...\n"
						"The node will lose his logicalAddress 0x%02x and will be set to 0xFE!"),
						m_pSelNode->szNodeName,
						m_pSelNode->bLogicalAddress);
	
	if (AfxMessageBox(szMessage,MB_OKCANCEL | MB_ICONQUESTION) == IDOK) 
	{
		CTXNodeMessage::getInstance()->
			FactoryInitNode(m_pSelNode->bLogicalAddress);

		// BE CAREFULLY ...
		// --> SHOULD WE REMOVE NODE FROM DATABASE ...
	}
}



void CPropertyPageNodeControlGateway::OnCheckSerialize() 
{
	BOOL fState;
	
	if (m_CheckSerialize.GetCheck() == 0) fState = FALSE;	
	else fState = TRUE;

	BYTE bDllAccessControl = m_pSelNode->bDLLAccessControl;
	
	if (fState == TRUE) bDllAccessControl |= APPL_SERIALIZE;
	else bDllAccessControl &= (APPL_SERIALIZE ^0xFF);

	if (CTXNodeMessage::getInstance()->SetNodeConfig(m_pSelNode->bLogicalAddress,
													 m_pSelNode->bNodeFlags,
													 bDllAccessControl)) 
	{	
		SetTimer(TIMERID_UPDATE_OBJECT_TIMER,
				 UPDATE_OBJECT_TIMER,
				 (TIMERPROC)NULL);

		m_CheckSerialize.SetCheck(fState);	
	}
	else 
	{
		UpdateRunTimeControls();
	}
}


void CPropertyPageNodeControlGateway::OnTimer(UINT nIDEvent) 
{
	KillTimer(TIMERID_UPDATE_OBJECT_TIMER);

	RequestRuntimeStatussen();
	
	CPropertyPage::OnTimer(nIDEvent);
}

void CPropertyPageNodeControlGateway::OnBtnRequest() 
{
	ASSERT(m_pSelNode);

	CTXNodeGatewayMessages::getInstance()
		->RequestState(m_pSelNode->bLogicalAddress);
	
}

void CPropertyPageNodeControlGateway::OnBtnClear() 
{
	CString szMessage("Clear database.\nAre you sure ?");

	if (AfxMessageBox(szMessage,MB_OKCANCEL | MB_ICONQUESTION) == IDOK) 
	{
		CTXNodeGatewayMessages::getInstance()
			->EraseDatabase(m_pSelNode->bLogicalAddress);	
	}
}

void CPropertyPageNodeControlGateway::OnBnClickedBtnDownloadSystemlog()
{
	using namespace TMAlgoritmesCfg;

	TMDownloadSystemLog(m_pSelNode);
}

void CPropertyPageNodeControlGateway::OnBnClickedBtnFirmware()
{	
	AfxGetMainWnd()->PostMessage(WM_SHOW_DIALOG_FIRMWAREUPDATE, m_pSelNode->getNodeAddress());
}
