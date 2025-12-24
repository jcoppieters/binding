// PropertyPageDaliWelcome.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "PropertyPageDaliWelcome.h"
#include "appdlgdaliworker.h"										// Voorlopig hier ????
#include "app\appCfg\AppCfginfo\cfginfoproductID_def.h"				// sedert V16.36 bepalen van de sbox ultra
// ------------------------------------
#include "TMGlobals.h"
using namespace TMNodeDatabase;
// ------------------------------------

// CPropertyPageDaliWelcome dialog
IMPLEMENT_DYNAMIC(CPropertyPageDaliWelcome, CPropertyPage)

CPropertyPageDaliWelcome::CPropertyPageDaliWelcome(CAppDlgDaliWorker& refWorker, CPropertyPageDaliContext& refContext)
	:  CPropertyPageDaliBaseClass(CPropertyPageDaliWelcome::IDD, refWorker, refContext), m_initDone(false), m_selectedNodeAddress(-1) { }

CPropertyPageDaliWelcome::~CPropertyPageDaliWelcome() { }

void CPropertyPageDaliWelcome::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_NODESELECT, m_comboBoxSelection);
	DDX_Control(pDX, IDC_RICHEDIT_WELCOME_TEXT, m_richEditWelcomeText);
	DDX_Control(pDX, IDC_BUTTON_CONFIGURE, m_btnConfigure);
	DDX_Control(pDX, IDC_STATIC_ICONSTATE, m_staticStateInfoIcon);
	DDX_Control(pDX, IDC_STATIC_STATEINFO, m_staticStateInfoText);
}


BEGIN_MESSAGE_MAP(CPropertyPageDaliWelcome, CPropertyPage)
	ON_CBN_SELCHANGE(IDC_COMBO_NODESELECT, &CPropertyPageDaliWelcome::OnCbnSelchangeComboNodeSelect)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_CONFIGURE, &CPropertyPageDaliWelcome::OnBnClickedButtonConfigure)
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, &CPropertyPageDaliWelcome::OnToolTipNeedText)
END_MESSAGE_MAP()


// CPropertyPageDaliDevices message handlers
BOOL CPropertyPageDaliWelcome::OnInitDialog() {
	CPropertyPage::OnInitDialog();		// CHANGES_MVS_2008 : Afleiden van CPropertyPage
	this->m_initDone = false;
	this->m_selectedNodeAddress = -1;
	this->initStaticControls( );
	this->initDynamicControls( );
	this->updateGUIStateCfgBridgePossible();
	this->SetTimer(TIMER_ID_POLL_SAVE,TIMER_SAVE_INTERVAL_MSEC,(TIMERPROC) NULL);	
	return TRUE;
}


void CPropertyPageDaliWelcome::initStaticControls( ) {
	const CString strRTFText = _T("{\\rtf1\\ansi\\ansicpg1252\\deff0\\nouicompat\\deflang1025")
                    _T("{\\fonttbl{\\f0\\fnil\\fcharset0 Verdana;}}")
                    _T("\\viewkind4\\uc1\\pard\\sa40\\sl120\\slmult1")
                    _T("\\fs18\\b Welcome to the DALI bus configuration.\\b0\\par")
                    _T("\\par")
                    _T("\\fs16The configuration process is divided into two parts:\\par")
                    _T("\\par")
					_T("\\fs16\\b\tPart one: DALI system bus Configuration\\b0\\par")
                    _T("\\fs16\tThis includes:\\par")                    
                    _T("\\fs16\t - Addressing the devices\\par")
                    _T("\\fs16\t - Locating and naming devices\\par")
					_T("\\fs16\t - Grouping control gear devices (optional)\\par")
                    _T("\\fs16\t - Setting device properties (optional)\\par")
                    _T("\\fs16\t - Creating DALI scenes (optional)\\par")                  
                    _T("\\fs16\tThese steps are done on the following pages...\\par")
                    _T("\\par")
					_T("\\fs16\\b\tPart two: Linking DALI Devices to Duotecno Units\\b0\\par")
                    _T("\\fs16\tThis is done through the Duotecno DALI bridge configuration.\\par")
					_T("\\fs16\tThis step allows the DALI devices to be controlled through the Duotecno system.\\par")
					_T("\\par")
					_T("\\fs16\\b Attention:\\b0  If the addressing in the DALI bus system changes, the Duotecno DALI bridge configuration\\par")
					_T("\\fs16may need to be updated.")
                    _T("}");

	m_richEditWelcomeText.SetWindowText(strRTFText);

	this->EnableToolTips(TRUE);
}

void CPropertyPageDaliWelcome::initDynamicControls( ) {
	if (!this->m_initDone) {
		CList<int,int> listNodes;
		if (this->getWorker().getListDaliNodes(&listNodes)) {
			const int count = listNodes.GetCount();
			if (count > 0) {			
				int selectedItem = 0;	// Huidige node 
				const int currentSelectedNode = this->getWorker().getCurrentNodeAddress();
				POSITION pos = listNodes.GetHeadPosition();
				while(pos!=NULL) {
					int dummy=0;
					CCanNode* pNode = NULL;
					const int nodeAddress = listNodes.GetNext(pos);									
					if (TMGetDocumentNodes()->FindNode(nodeAddress,&dummy,&pNode)) {
						const int itemIndex = m_comboBoxSelection.AddString(pNode->getNodeName());
						if (itemIndex >= 0) {			
							if (currentSelectedNode == nodeAddress) {
								this->m_selectedNodeAddress = nodeAddress;
								selectedItem = itemIndex;	// Huidige node 
							}
							m_comboBoxSelection.SetItemData(itemIndex, nodeAddress);
						}						
					}
				}
				m_comboBoxSelection.SetCurSel(selectedItem);	// Huidige node 
			}		
		}
		this->m_initDone = true;
	}	
}

BOOL CPropertyPageDaliWelcome::OnSetActive() {
	this->updateGUIStateCfgBridgePossible();
	return CPropertyPage::OnSetActive();
}
BOOL CPropertyPageDaliWelcome::OnKillActive() {
	return CPropertyPage::OnKillActive();
}

void CPropertyPageDaliWelcome::OnCbnSelchangeComboNodeSelect() {

	// Voor we een nieuw selecteren controleren of er data moet opgeslagen worden.
	this->pollSaveChanges();

	const int sel = m_comboBoxSelection.GetCurSel();
	if (sel < 0) {
		ASSERT(sel >= 0);
		return;
	}	
	const int nodeAddress = m_comboBoxSelection.GetItemData(sel);
	ASSERT(nodeAddress>=0 && nodeAddress<=0xFF);
	if (FALSE == this->getWorker().setCurrentNodeAddress(nodeAddress)) {
		ASSERT(0);
	}

	// Wanneer er een andere node geselecteerd wordt de andere property pages melden.
	// De vlag wordt gecontroleerd in de onActive( )
	if (nodeAddress != this->m_selectedNodeAddress) {
		CPropertyPageDaliContext& refContext = this->getContext();				
		refContext.setFlag(CPropertyPageDaliContext::FLAG_UPDATE_DIALOG_DEVICES);
		refContext.setFlag(CPropertyPageDaliContext::FLAG_UPDATE_DIALOG_NWK);
		refContext.setFlag(CPropertyPageDaliContext::FLAG_UPDATE_DIALOG_LOCATE);
		refContext.setFlag(CPropertyPageDaliContext::FLAG_UPDATE_DIALOG_GROUPS);		
		this->m_selectedNodeAddress = nodeAddress;

		// Parent property sheet melden dat er een nieuwe node geselecteerd werd.
		// Dit wordt gebruikt voor de weergave van de fout dialoog.
		// Eerst de huidige dialoog afsluiten.
		// Indien nodig wordt de fout dialoog geopend voor de nieuw geselecteerde DALI interface.
		CWnd* const pWnd = this->GetParent();
		if (NULL != pWnd) {
			pWnd->SendMessage(WM_PROPERTYSHEET_DIALOG_DALI_ERRORS_CLOSE);	
			pWnd->PostMessage(WM_PROPERTYSHEET_DIALOG_DALI_ERRORS_OPEN);
		}
	}

	// Updaten van de GUI.
	this->updateGUIStateCfgBridgePossible();
}

void CPropertyPageDaliWelcome::OnTimer(UINT nIDEvent) {
	if (TIMER_ID_POLL_SAVE == nIDEvent) {	
		// Moeten er veranderingen opgeslagen worden.
		this->pollSaveChanges();	

		// Is de status veranderd: Mogelijks door het openen van de Configurator.
		this->updateGUIStateCfgBridgePossible();
	}
}

void CPropertyPageDaliWelcome::pollSaveChanges( ) {
	if (this->getWorker().pollByGuiSaveChanges()) {			
		// TODO: Notify configurator data has changed.
	}	
}

void CPropertyPageDaliWelcome::OnBnClickedButtonConfigure() {
	if (this->m_selectedNodeAddress <= 0 || this->m_selectedNodeAddress >= 0xff) {
		return;
	}

	if (this->getWorker().getStatePendingErrors()) {
		CString strTitle("Open the Duotecno DALI bridge configuration?");
		CString strMessage("Errors were found in the DALI system bus. These may affect the DALI Duotecno Bridge configuration.\r\n\r\n"
						   "First, fix the errors and recheck the system by running 'Search devices' or 'Extend installation'. "
						   "If no errors appear, the DALI system bus is correctly set up.\r\n\r\n"
						   "Do you still want to open the DALI Duotecno Bridge configuration?");

		if (IDNO == this->MessageBox(strMessage,strTitle,MB_YESNO | MB_ICONWARNING)) {
			return;
		}	
	}
	this->getWorker().openDALIDuotecnoBridgeConfiguration();
}

// De knop button config enkel enablen wanneer er devices aanwezig zijn...
void CPropertyPageDaliWelcome::updateGUIStateCfgBridgePossible( ) {
	BOOL enableConfigButton = TRUE;
	LPCSTR identifierIcon = NULL;
	CString strInfo;
	const CAppDlgDaliWorker::StateDaliBridgeCfg_t state = this->getWorker().getDALIDuotecnoBridgeCfgState( );
	switch(state) {
		case CAppDlgDaliWorker::STATE_DALIBRIDGE_CFG_DISABLED: {
			// Er is nog geen database.		
			enableConfigButton = FALSE;
			identifierIcon = MAKEINTRESOURCE(IDI_INFORMATION);
			strInfo = _T("First configure the DALI bus system.");
		} break;

		case CAppDlgDaliWorker::STATE_DALIBRIDGE_CFG_ENABLED: {
			// Ok idle	
			enableConfigButton = TRUE;
			identifierIcon = NULL;
			strInfo.Empty();
		} break;

		case CAppDlgDaliWorker::STATE_DALIBRIDGE_CFG_PENDING_CHANGES: {
			// Er zijn veranderingen gebeurd.										   
			enableConfigButton = TRUE;
			identifierIcon = MAKEINTRESOURCE(IDI_INFORMATION);
			strInfo = _T("Changes in the DALI bus system.\r\n"
						 "Configure the Duotecno DALI bridge");
		} break;

		case CAppDlgDaliWorker::STATE_DALIBRIDGE_CFG_PENDING_ERRORS: {
			// Er zijn nog fouten aanwezig.									   
			enableConfigButton = TRUE;
			identifierIcon = MAKEINTRESOURCE(IDI_WARNING);
			strInfo = _T("Issues pending in the DALI bus system,\r\n" 
						 "Fix problems first before configure.");
		} break;

		case CAppDlgDaliWorker::STATE_DALIBRIDGE_CFG_BUSY: {
			// Er is configuratie bezig.
			enableConfigButton = FALSE;
			identifierIcon = MAKEINTRESOURCE(IDI_INFORMATION);
			strInfo = _T("Addressing DALI-bus busy\r\n"
						 "Wait until done");			   														   
		} break;
	}

	this->m_btnConfigure.EnableWindow(enableConfigButton);

	// Weergave van het icoon.
	HICON hIconState = NULL;
	if (NULL != identifierIcon) {
		hIconState = (HICON)LoadImage(NULL, identifierIcon, IMAGE_ICON, 32, 32, LR_SHARED);		
	}
	if (NULL != hIconState) {		
		m_staticStateInfoIcon.SetIcon(hIconState);
	}
	m_staticStateInfoIcon.ShowWindow(hIconState != NULL ? TRUE : FALSE);

	// Weergave van de info tekst.
	m_staticStateInfoText.SetWindowText(strInfo);
	m_staticStateInfoText.ShowWindow(strInfo.IsEmpty() ? FALSE : TRUE);
}


// Weergave van de tooltip.
//
// Oppassen: In de dialoog *.rc mogen er geen identieke ID's voorkomen.
// Wanneer er 2 dezelfde ID's voorkomen (bvb IDC_STATIC) dan werken de tooltips van de controls niet.
// Mogelijkheid is om de volgorde van de controls in de *.rc aan te passen.
// Eerst de controls (met een uniek ID) die een tooltip nodig hebben.
//
// EnableToolTips(TRUE) aanroepen in de OnInitDialog( )
BOOL CPropertyPageDaliWelcome::OnToolTipNeedText(UINT id, NMHDR *pNMHDR, LRESULT *pResult) {
	TOOLTIPTEXTA* pTTT = (TOOLTIPTEXTA*)pNMHDR;
	UINT_PTR identifier = pNMHDR->idFrom;
	if (pTTT->uFlags & TTF_IDISHWND) {
		identifier = ::GetDlgCtrlID((HWND)identifier);  // Haal ID van de control op
	}
	BOOL handled = FALSE;
	switch(identifier) {
		case IDC_RICHEDIT_WELCOME_TEXT: {
			this->m_strToolTipText = _T("Read this text for more information");
			handled = TRUE;										 
		} break;
		case IDC_COMBO_NODESELECT: {
			this->m_strToolTipText = _T("Select Duotecno node (DALI-bus system)");
			handled = TRUE;
		} break;
		case IDC_BUTTON_CONFIGURE: {
			this->m_strToolTipText = _T("Configure the Duotecno DALI bridge settings");
			handled = TRUE;
		} break;		 
	}
	if (handled) {
		pTTT->lpszText = (LPTSTR)(LPCTSTR) this->m_strToolTipText;
		*pResult = 0;
	}
	return handled;
}
