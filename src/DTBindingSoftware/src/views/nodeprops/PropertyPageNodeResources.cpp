// PropertyPageNodeResources.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "PropertyPageNodeResources.h"

#include "CanNode.h"
#include "TXNodeMessage.h"
#include "NodeRuntimeStatus.h"

#include "app\appCfg\AppCfginfo\cfginfonode.h"	
#include "app\appCfg\AppCfginfo\cfginfoproductID_def.h"	


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropertyPageNodeResources property page

IMPLEMENT_DYNCREATE(CPropertyPageNodeResources, CPropertyPage)

CPropertyPageNodeResources::CPropertyPageNodeResources() : 
CPropertyPageNode(CPropertyPageNodeResources::IDD)
{
	//{{AFX_DATA_INIT(CPropertyPageNodeResources)
	//}}AFX_DATA_INIT
}

CPropertyPageNodeResources::CPropertyPageNodeResources(CCanNode *pNode): 
CPropertyPageNode(CPropertyPageNodeResources::IDD,pNode),
fInitControls(FALSE)
{	
	ASSERT(m_pSelNode);
	ASSERT(fInitControls == FALSE);
}

CPropertyPageNodeResources::~CPropertyPageNodeResources()
{
}

void CPropertyPageNodeResources::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropertyPageNodeResources)
	DDX_Control(pDX, IDC_LIST_NODETYPE, m_ListNodeType);
	DDX_Control(pDX, IDC_LIST_CANERRORS, m_ListErrorsCAN);
	DDX_Control(pDX, IDC_LIST_NETWORKERRORS, m_ListErrorsNetwork);
	DDX_Control(pDX, IDC_LIST_RESOURCES, m_ListResources);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropertyPageNodeResources, CPropertyPage)
	//{{AFX_MSG_MAP(CPropertyPageNodeResources)
	ON_BN_CLICKED(IDC_BTN_UPDATE, OnBtnUpdate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyPageNodeResources message handlers

BOOL CPropertyPageNodeResources::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	fInitControls = TRUE;
	UpdateStaticControls();
	UpdateRunTimeControls();
	RequestRuntimeStatussen();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPropertyPageNodeResources::UpdateRunTimeControls(void)
{
	CString string;
	
	if (fInitControls == TRUE) 
	{
		ASSERT(m_pSelNode);
		ASSERT(m_pSelNode->GetRuntimeStatus());
	
		// Errors ...
		m_ListErrorsCAN.ResetContent();
		m_ListErrorsNetwork.ResetContent();

		if (m_pSelNode->GetRuntimeStatus() != 0)
		{
			CStringArray stringArray;

			m_pSelNode->GetRuntimeStatus()->DumpCanControllerErrors(stringArray);
			for (int i=0,n = stringArray.GetSize();i<n;i++)
			{		
				m_ListErrorsCAN.AddString(stringArray.GetAt(i));
			}

			m_pSelNode->GetRuntimeStatus()->DumpNetworkErrors(stringArray);
			for (int i=0,n = stringArray.GetSize();i<n;i++)
			{		
				m_ListErrorsNetwork.AddString(stringArray.GetAt(i));
			}
		}
	}
}

void CPropertyPageNodeResources::RequestRuntimeStatussen(void)
{
	CTXNodeMessage::getInstance()->RequestNodeNicErrors(m_pSelNode->bLogicalAddress,0);
	CTXNodeMessage::getInstance()->RequestNodeNicErrors(m_pSelNode->bLogicalAddress,1);
}


/////////////////

CString CPropertyPageNodeResources::GetSeperator(void)
{
	CString s;
	s.Format(_T("-----------------------------------------------"));
	return s;
}

void CPropertyPageNodeResources::UpdateStaticControls(void) 
{
	CString string,szTemp;
	
	ASSERT(m_pSelNode);

	m_ListResources.ResetContent();
	// Resources ...	
	string.Format(_T("Max packet length : %d"),m_pSelNode->MaxPacketLength());
	m_ListResources.AddString(string);
	
	string.Format(_T("Max name length : %d"),m_pSelNode->MaxNameLength());
	m_ListResources.AddString(string);

	string.Format(_T("Max temp buffer : %i Bytes"),m_pSelNode->MaxTempBuffer());
	m_ListResources.AddString(string);
	m_ListResources.AddString(GetSeperator());

	string.Format(_T("Max. binding entries : %d"),m_pSelNode->MaxBindingEntries());
	m_ListResources.AddString(string);

	string.Format(_T("Max. condition bindings : %d"),m_pSelNode->MaxConditions());
	m_ListResources.AddString(string);

	string.Format(_T("Max. operator timers : %d"),m_pSelNode->MaxTimerOperators());
	m_ListResources.AddString(string);

	string.Format(_T("Max. condition string length : %d bytes"),m_pSelNode->MaxConditionLength());
	m_ListResources.AddString(string);

	string.Format(_T("Max. number of references : %d"),m_pSelNode->MaxReferences());
	m_ListResources.AddString(string);
	m_ListResources.AddString(GetSeperator());
		
	m_ListNodeType.ResetContent();
/*
	string.Format(_T("Software version : V%02X"),
		m_pSelNode->GetSoftwareVersion());
*/

	// NodeType ...
	CCanNodeInfo_Util::formatSoftwareVersion(*m_pSelNode,szTemp);
	string.Format(_T("Software version : %s"),szTemp);
	m_ListNodeType.AddString(string);

	string.Format(_T("Nr of units : %d"),
		m_pSelNode->bNROfUnits);
	m_ListNodeType.AddString(string);
	m_ListNodeType.AddString(GetSeperator());

	string.Format(_T("Node physical address = 0x%06X"),
		m_pSelNode->ulPhysicalAddress);
	m_ListNodeType.AddString(string);
	string.Format(_T("Assigned logical address = 0x%02x"),
		m_pSelNode->bLogicalAddress);

	m_ListNodeType.AddString(string);
	m_ListNodeType.AddString(GetSeperator());

	switch(m_pSelNode->bNodeType) 
	{
		case NODE_PIN_STD_UNITS: 
		{
			szTemp.Format(_T("Standard node")); 
		} break;
		case NODE_LCD: 
		{
			szTemp.Format(_T("LCD/TFT node")); 
		} break;
		case NODE_PIN_MODEM: 
		{
			szTemp.Format(_T("Modem")); 		
		} break;
		case NODE_PIN_GATEWAY:
		{
			szTemp.Format(_T("Gateway"));
		} break;
		default: 
		{
			szTemp.Format(_T("Reserved node")); 
		} break;
	}

	string.Format(_T("Node type : %s"),szTemp);
	m_ListNodeType.AddString(string);

	if (m_pSelNode->bNodeType == NODE_LCD) 
	{
		if (m_pSelNode->bNodeFlags & NODE_MASTER_LCD) 
		{
			string.Format(_T("Master LCD !"));
			m_ListNodeType.AddString(string);
		}

		switch(m_pSelNode->bNodeFlags & NODE_LCD_TYPE_MASK) {
			case NODE_LCD_TYPE_BASIC: 
				szTemp.Format(_T("Basic LCD")); 
			break;
			case NODE_LCD_TYPE_HC4 : 
				szTemp.Format(_T("HC4 LCD")); 
			break;
			case NODE_LCD_TYPE_HC5 : 
				szTemp.Format(_T("HC5 LCD")); 
			break;
			default : 
				szTemp.Format(_T("Unknown")); 
			break;
		}

		string.Format(_T("LCD type : %s"),szTemp);
		m_ListNodeType.AddString(string);

		if (m_pSelNode->bNodeFlags & NODE_LCD_MATRIX_AUDIO) 
			szTemp.Format(_T("Matrix system"));
		else 
			szTemp.Format(_T("IR-Audio system"));

		string.Format(_T("Audio system : %s"),szTemp);
		m_ListNodeType.AddString(string);
	}

	{
		// Sedert versie 0x0804.
		CString szTempProduct;
		const BYTE bProductID=m_pSelNode->GetProductID();
		
		switch(bProductID)
		{
			case PRODUCT_ID_UNKNOWN:
			case PRODUCT_ID_MODEM_RS232:
			case PRODUCT_ID_MODEM_USB:
			{
				// absorbe...
			} break;
			case PRODUCT_ID_OLED: {
				szTempProduct.Format("Smartouch Oled");	
			} break;
			case PRODUCT_ID_OLED_HOTEL: {
				szTempProduct.Format("Room-e Oled");
			} break;
			case PRODUCT_ID_OLED_TOUCHBTN_4V: {
				szTempProduct.Format("Easytouch Oled");
			} break;

			// Touchbuttons met LED feedback.
			case PRODUCT_ID_TCHBTN_1CO1SE: {
				szTempProduct.Format("One touch button");										   
			} break;
			case PRODUCT_ID_TCHBTN_2CO1SE: {
				szTempProduct.Format("Two touch buttons");										   
			} break;
			case PRODUCT_ID_TCHBTN_4CO1SE: {
				szTempProduct.Format("Four touch buttons");										   
			} break;
			case PRODUCT_ID_TCHBTN_8CO1SE: {
				szTempProduct.Format("Eight touch buttons");										   
			} break;

			// Mechanische Switch buttons.
			case PRODUCT_ID_1BTN_SWITCH_2CO1SE: {
				szTempProduct.Format("One switch button");													
			} break;
			case PRODUCT_ID_2BTN_SWITCH_4CO1SE: {
				szTempProduct.Format("Two switch buttons");													
			} break;
			case PRODUCT_ID_4BTN_SWITCH_8CO1SE: {
				szTempProduct.Format("Four switch buttons");													
			} break;

			// Mechanische Push buttons.
			case PRODUCT_ID_1BTN_PUSH_1CO1SE: {
				szTempProduct.Format("One push button");													
			} break;
			case PRODUCT_ID_2BTN_PUSH_2CO1SE: {
				szTempProduct.Format("Two push buttons");													
			} break;
			case PRODUCT_ID_4BTN_PUSH_4CO1SE: {
				szTempProduct.Format("Four push buttons");													
			} break;

			case PRODUCT_ID_HC_BOX: {
				szTempProduct.Format("Smartbox");	
			} break;
			case PRODUCT_ID_HC_BOX_ULTRA: {
				szTempProduct.Format("Smartbox ultra");		// sedert V16.32									  												
			} break;

			case PRODUCT_ID_GATEWAY_HS: {
				szTempProduct.Format("TCP/IP Homeserver");	
			} break;
			case PRODUCT_ID_GATEWAY_HS_32VIRT: {
				szTempProduct.Format("TCP/IP Homeserver 32v");	
			} break;
			case PRODUCT_ID_GATEWAY_HS_MASTER_100VIRT: {
				szTempProduct.Format("TCP/IP Homeserver 100v");	
			} break;	
			case PRODUCT_ID_GATEWAY_MODBUS_16SE32VIRT: {
				szTempProduct.Format("Gateway modbus");	
			} break;
			case PRODUCT_ID_TFT_32VIRT: {
				szTempProduct.Format("TFT Color");	
			} break;
			case PRODUCT_ID_STD_ANALOG_8SENS: {
				szTempProduct.Format("Analog sensor");	
			} break;
			case PRODUCT_ID_STD_DMX_MASTER_64DIM: {
				szTempProduct.Format("DMX 64 units");	
			} break;
			case PRODUCT_ID_STD_DALI_MASTER_DIM: {
				szTempProduct.Format("DALI Dim Master");	
			} break;
			case PRODUCT_ID_STD_ANALOGIN_8UNIVSE8CO: {
				szTempProduct.Format("Analog input module");	
			} break;
			case PRODUCT_ID_ALARM_TEXECOM: {
				szTempProduct.Format("Alarm Texecom");	
			} break;
			case PRODUCT_ID_ALARM_INTEGRA: {
				szTempProduct.Format("Alarm Satel Integra");	
			} break;
			case PRODUCT_ID_8CHIRTX_AVMARANTZ: {
				szTempProduct.Format("8 channel IRTX + AV-Marantz");
			} break;
			case PRODUCT_ID_8CHIRTX_AVINTEGRA: {
				szTempProduct.Format("8 channel IRTX + AV-Integra");
			} break;
			case PRODUCT_ID_8CHIRTX_AVAUDAC: {
				szTempProduct.Format("8 channel IRTX + AV-Audac");
			} break;
			case PRODUCT_ID_8CHIRTX_AVIP: {
				szTempProduct.Format("8 channel IRTX + AV-IP");
			} break;
			case PRODUCT_ID_AVMATRIX_BOSEST: {
				szTempProduct.Format("Bose soundtouch interface");	
			} break;
			case PRODUCT_ID_AVMATRIX_SONOS:	 {
				szTempProduct.Format("Sonos avmatrix interface");	
			} break;
		}

		if(FALSE==szTempProduct.IsEmpty())
		{
			string.Format(_T("Product : %s"),szTempProduct);
			m_ListNodeType.AddString(string);
		}
	}
}

void CPropertyPageNodeResources::OnBtnUpdate() 
{
	CTXNodeMessage::getInstance()->RequestNodeNicErrors(m_pSelNode->bLogicalAddress,0);	
	CTXNodeMessage::getInstance()->RequestNodeNicErrors(m_pSelNode->bLogicalAddress,1);
}


