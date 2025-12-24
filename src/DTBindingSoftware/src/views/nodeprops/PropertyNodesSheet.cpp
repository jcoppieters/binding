// PropertyNodesSheet.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "PropertyNodesSheet.h"
#include "Mainfrm.h"
#include "MyWindowMessages.h"
#include "NodeDatabase_Util.h"
// -------------------------------------------
#include "PropertyPageNodeBindings.h"
#include "PropertyPageNodeResources.h"
#include "PropertyPageNodeControl.h"
#include "PropertyPageNodeControlGateway.h"
#include "PropertyPageNodeScreenmap.h"
#include "PropertyPageNodeBackup.h"
#include "PropertyPageNodeBitmaps.h"			
#include "PropertyPageNodeBitmapsHC4.h"
#include "PropertyPageNodeBitmapsBasic.h"
#include "PropertyPageNodeBitmapsHC5.h"
#include "PropertyPageNodeConfig.h"				
#include "PropertyPageNodeConfigV2.h"	
#include "PropertyPageNodeSmapLite.h"			
#include "PropertyPageNodeHomeClientMaster.h"
#include "appdlgtasks.h"
#include "nodeScreenmapInfo.h"					
#include "app\appCfg\AppCfginfo\cfginfonode.h"	
#include "app\appCfg\AppCfginfo\cfginfoProductID_def.h"
#include "utils/mfc/DialogLayoutHelper.h"				// V17.02
#include "AppLogging.h"
// --------------------------------------------
#include "TMGlobals.h"
using namespace TMNodeDatabase;
using namespace TMGlobalSettings;
using namespace TMCommunication;			// Tijdelijk nodig om te bepalen of de homeclient dialoog mag getoond worden.
// --------------------------------------------

// --------------------------------------------
#undef ENABLE_DEBUG_DIALOG_LAYOUT
// --------------------------------------------
#ifdef _DEBUG
#define ENABLE_DEBUG_DIALOG_LAYOUT				0			// Op 1 plaatsen om te debuggen.
#else
#define ENABLE_DEBUG_DIALOG_LAYOUT				0
	#define DEBUG_DIALOG_LAYOUT(x)
#endif

/////////////////////////////////////////////////////////////////////////////
#if(ENABLE_DEBUG_DIALOG_LAYOUT == 1)
	#include "utils/mfc/DebugDialogLayout.h"
	#define DEBUG_DIALOG_LAYOUT(x)						CDebugDialogLayout::dumpControlLayout(x)
#else
	#define DEBUG_DIALOG_LAYOUT(x)	
#endif
/////////////////////////////////////////////////////////////////////////////
#if (ENABLE_TRACE_PROPERTYNODES_SHEET == 1)
	#define TRACER_INFO(x)		AppLogging::getInstance().addInfo(x)
	#define TRACER_WARN(x)		AppLogging::getInstance().addWarning(x)
	#define TRACER_ERROR(x)		AppLogging::getInstance().addError(x)
#else 
	#define TRACER_INFO(x)
	#define TRACER_WARN(x)
	#define TRACER_ERROR(x)	
#endif
/////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
namespace {
	static bool showPropertyPageHomeClient(CCanNode* const pNode) {
		return CComApplication::GetInstance()->isSocketHomeClientMasterSupported(pNode);
	}
};
/////////////////////////////////////////////////////////////////////////////
CPropertyNodesSheet* CPropertyNodesSheet_Holder::m_pPropertyNodesSheet = NULL;
/////////////////////////////////////////////////////////////////////////////
CPropertyNodesSheet* CPropertyNodesSheet_Holder::Get() {
	return m_pPropertyNodesSheet;
}

CPropertyNodesSheet* CPropertyNodesSheet_Holder::Create(CWnd* pParent,CCanNode* pNode,PropertyNodesSheetPage_t page)
{
	if (NULL != m_pPropertyNodesSheet) {	
		ASSERT(NULL == m_pPropertyNodesSheet);
		return NULL;
	}		
	const int firstPage = (int) CPropertyNodesSheet::convertPageNr(page,pNode);

	m_pPropertyNodesSheet = new CPropertyNodesSheet("",pParent,pNode,firstPage);
	if (NULL == m_pPropertyNodesSheet) {		
		TRACER_ERROR("CPropertyNodesSheet_Holder failed to create instance.");
		return NULL;
	}

	const BOOL fResult = m_pPropertyNodesSheet->Create(pParent,
						      WS_SYSMENU |  WS_POPUP | WS_CAPTION | 
							  DS_MODALFRAME | WS_VISIBLE,
							  WS_EX_DLGMODALFRAME);										  
	if (FALSE == fResult) {
		TRACER_ERROR("CPropertyNodesSheet_Holder failed to create window");
		delete m_pPropertyNodesSheet;
		m_pPropertyNodesSheet = NULL;
		return NULL;
	}
				
	ASSERT(m_pPropertyNodesSheet->GetSafeHwnd());
	TMGetFileAccess()->Set_SaveAs_Enable(FALSE);	/* !!! */
	return m_pPropertyNodesSheet;							
}

void CPropertyNodesSheet_Holder::Destroy()
{
	if (NULL != m_pPropertyNodesSheet) {
		if (NULL != m_pPropertyNodesSheet->GetSafeHwnd()) {
			m_pPropertyNodesSheet->DestroyWindow();
		}
		ASSERT(m_pPropertyNodesSheet->GetSafeHwnd() == NULL);
		delete m_pPropertyNodesSheet;
		m_pPropertyNodesSheet = NULL;

		TMGetFileAccess()->Set_SaveAs_Enable(TRUE);		/* !!! */
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPropertyNodesSheet

IMPLEMENT_DYNAMIC(CPropertyNodesSheet, CPropertySheet)		// CHANGES_MVS_2008 : Afleiden van CPropertySheet



// TM,V16.14:
// Aanpassingen:
// - De eerste pagina wordt niet meer meegegeven in de ctor van CPropertySheet.
//   Deze wordt bijgehouden in de variabel m_firstPage
//   In de OnInitDialog wordt deze gebrukt om de active pagina te selecteren.
//   Er is een controle of deze geldig is.
//   REDEN aanpassing: Wanneer we een verkeerde waarde meegeven in de ctor dan mislukt de create( ).
//   In de onInitDialog() kunnen we de controle uitvoeren.
// - Gebruik van de CPropertyPage::Construct().
//   Deze mag aangeroepen worden voor we de propertyPage toevoegen aan de propertysheet via AddPage( )
CPropertyNodesSheet::CPropertyNodesSheet(LPCTSTR pszCaption, CWnd* pParentWnd, CCanNode* pNode,UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd), pSelNode(pNode), m_firstPage(iSelectPage)
{
	ASSERT(CNodeDatabase_Util::isValid(*TMGetDocumentNodes(),pNode));

	if (1) {
		for (int i=0;i<MAX_PAGES;i++) {	
			m_pPages[i] = NULL;	
		}
	}

	const BYTE bSoftwareVersion = pSelNode->GetSoftwareVersion();

	CString szCaption;
	szCaption.Format(_T("'%s' - (LA=0x%02x)"), pNode->szNodeName, pNode->bLogicalAddress);
	SetTitle(szCaption);

	if(1)
	{
		// pagina voor de bindings: voor alle nodes.
		m_pPages[NODE_BINDINGS] = new CPropertyPageNodeBindings(pNode);
		m_pPages[NODE_BINDINGS]->Construct(CPropertyPageNodeBindings::IDD);	// Sedert V16.14
		m_pPages[NODE_BINDINGS]->m_psp.dwFlags |= PSP_USETITLE;
		m_pPages[NODE_BINDINGS]->m_psp.pszTitle = _T("Bindings");		
	}

	if(1)
	{
		// pagina voor de resources: voor alle nodes
		m_pPages[NODE_RESOURCES] = new CPropertyPageNodeResources(pNode);
		m_pPages[NODE_RESOURCES]->Construct(CPropertyPageNodeResources::IDD);	// Sedert V16.14
		m_pPages[NODE_RESOURCES]->m_psp.dwFlags |= PSP_USETITLE;
		m_pPages[NODE_RESOURCES]->m_psp.pszTitle = _T("Resources");		
	}

	if(1)
	{
		// pagina voor de gateway node.		
		int resourceID = -1;
		m_pPages[NODE_CONTROL] = NULL;
		if ((pNode->bNodeType & NODE_PIN_GATEWAY) == NODE_PIN_GATEWAY) {	
			// Sedert V16.33: Integratie Smartbox ultra
			// Een smartbox die niet ingesteld staat als master moeten we zien als een 
			// gewone node.
			if (PRODUCT_ID_HC_BOX_ULTRA == pNode->GetProductID()) {
				if (pNode->bNodeFlags & NODE_MASTER_LCD) {
					m_pPages[NODE_CONTROL] =  new CPropertyPageNodeControlGateway(pNode);	
					resourceID = CPropertyPageNodeControlGateway::IDD;
				}
				else{
					m_pPages[NODE_CONTROL] =  new CPropertyPageNodeControl(pNode);
					resourceID = CPropertyPageNodeControl::IDD;
				}
			}
			else {
				m_pPages[NODE_CONTROL] =  new CPropertyPageNodeControlGateway(pNode);	
				resourceID = CPropertyPageNodeControlGateway::IDD;
			}
		}
		else {
			m_pPages[NODE_CONTROL] =  new CPropertyPageNodeControl(pNode);
			resourceID = CPropertyPageNodeControl::IDD;
		}


		if (NULL != m_pPages[NODE_CONTROL]) {
			ASSERT(resourceID >= 0);
			m_pPages[NODE_CONTROL]->Construct(resourceID);					// Sedert V16.14
			m_pPages[NODE_CONTROL]->m_psp.dwFlags |= PSP_USETITLE;
			m_pPages[NODE_CONTROL]->m_psp.pszTitle = _T("Control");			
		}
	}

	if (1) 
	{
		// pagina voor de screenmap configuratie.
		int resourceID = -1;
		m_pPages[NODE_SCREENMAPS] = NULL;		
		if (CNodeScreenmapInfo_Util::isSupported(pNode)) {							
			m_pPages[NODE_SCREENMAPS] = new CPropertyPageNodeScreenmapV2(pSelNode);	
			resourceID = CPropertyPageNodeScreenmapV2::IDD;
		}	
		else if (CNodeScreenmapLiteInfo_Util::isSupported(pNode)) {
			m_pPages[NODE_SCREENMAPS] = new CPropertyPageNodeSmapLite(pSelNode);	
			resourceID = CPropertyPageNodeSmapLite::IDD;
		}
		if (NULL != m_pPages[NODE_SCREENMAPS]) {
			ASSERT(resourceID >= 0);
			m_pPages[NODE_SCREENMAPS]->Construct(resourceID);					// Sedert V16.14
			m_pPages[NODE_SCREENMAPS]->m_psp.dwFlags |= PSP_USETITLE;
			m_pPages[NODE_SCREENMAPS]->m_psp.pszTitle = _T("Screenmapping");			
		}
	}


	if(1) 
	{
		// pagina voor de bitmap configuratie.
		if (CNodeBitmapConfig_Util::isSupported(pNode))
		{
			int resourceID = -1;
			m_pPages[NODE_BITMAPS] = NULL;
			const NodeBitmapConfigtype type = CNodeBitmapConfig_Util::getNodetype(pNode);			
			switch (type)
			{
				case NODEBITMAP_CONFIG_TYPE_V62_BASIC:
				case NODEBITMAP_CONFIG_TYPE_V63_BASIC:
				case NODEBITMAP_CONFIG_TYPE_V64_BASIC:
				{
					m_pPages[NODE_BITMAPS] = new CPropertyPageNodeBitmapsBasic(pSelNode);
					resourceID = CPropertyPageNodeBitmapsBasic::IDD;
				} break;
				case NODEBITMAP_CONFIG_TYPE_V64_HC4:
				{
					m_pPages[NODE_BITMAPS] = new CPropertyPageNodeBitmapsHC4(pSelNode);
					resourceID = CPropertyPageNodeBitmapsHC4::IDD;
				} break;
				case NODEBITMAP_CONFIG_TYPE_V64_HC5:
				{
					m_pPages[NODE_BITMAPS] = new CPropertyPageNodeBitmapsHC5(pSelNode);
					resourceID = CPropertyPageNodeBitmapsHC5::IDD;
					
				} break;	
				case NODEBITMAP_CONFIG_TYPE_V65:
				case NODEBITMAP_CONFIG_TYPE_V66:
				{
					// Bij de V65 nodes is er geen property page.
				} break;
				default:
				{							
				} break;
			}
			if (NULL != m_pPages[NODE_BITMAPS]) {
				m_pPages[NODE_BITMAPS]->Construct(resourceID);				// Sedert V16.14
				m_pPages[NODE_BITMAPS]->m_psp.dwFlags |= PSP_USETITLE;
				m_pPages[NODE_BITMAPS]->m_psp.pszTitle = _T("Bitmaps");				
			}
		}
	}

	if(1) 
	{
		// pagina voor de memory backup.
		if (CNodeBackupConfig_Util::isSupported(pNode))
		{
			// Node types V65 & V66: 
			// V65: Geen backup mogelijk - zou wel kunnen via FTP.
			// V66: Backup mogelijk via Cfg'er tool.
			const NodeBackupConfigType type = CNodeBackupConfig_Util::getNodeType(pNode);
			if ((NODEBACKUP_CONFIG_TYPE_V65!=type) && (NODEBACKUP_CONFIG_TYPE_V66!=type))
			{
				m_pPages[NODE_BACKUP] = new CPropertyPageNodeBackup(pSelNode);
				m_pPages[NODE_BACKUP]->Construct(CPropertyPageNodeBackup::IDD);		// Sedert V16.14
				m_pPages[NODE_BACKUP]->m_psp.dwFlags |= PSP_USETITLE;
				m_pPages[NODE_BACKUP]->m_psp.pszTitle = _T("Backup");				
			}
		}
	}

	if(1) 
	{
		// pagina voor de extended configuratie.
		if (CNodeExtConfigInfo_Util::isSupported(pNode)) {
			m_pPages[NODE_CONFIGURATION] = new CPropertyPageNodeConfigV2(pNode);	
			m_pPages[NODE_CONFIGURATION]->Construct(CPropertyPageNodeConfigV2::IDD);		// Sedert V16.14
			m_pPages[NODE_CONFIGURATION]->m_psp.dwFlags |= PSP_USETITLE;
			m_pPages[NODE_CONFIGURATION]->m_psp.pszTitle = _T("Configuration");
			
		}
	}

	if (1) {
		if (showPropertyPageHomeClient(pNode)) {		
			// Tijdelijke oplossing: Nu als static...
			static CAppDlgHomeClientMasterTasks_Host dialogWorker;		

			m_pPages[NODE_HOMECLIENT_MASTER] = new CPropertyPageNodeHomeClientMaster(pNode,&dialogWorker);
			m_pPages[NODE_HOMECLIENT_MASTER]->Construct(CPropertyPageNodeHomeClientMaster::IDD);		// Sedert V16.14
			m_pPages[NODE_HOMECLIENT_MASTER]->m_psp.dwFlags |= PSP_USETITLE;
			m_pPages[NODE_HOMECLIENT_MASTER]->m_psp.pszTitle = _T("Master tasks");			
		}		
	}

	// alle paginas toevoegen...
	if (1) {
		for (int i=0;i<MAX_PAGES;i++) {
			if (m_pPages[i] != 0) {
				AddPage(m_pPages[i]);				
			}
		}
	}

	if (1) {
		/* TM20120830 */
		BYTE bNodeAddress = 0xFF;
		if (pSelNode != 0) {
			bNodeAddress = pSelNode->getNodeAddress();	
		}
		AfxGetMainWnd()->PostMessage(WM_PROPERTYSHEET_NODES_ENTER,bNodeAddress,0);
	}
}


CPropertyNodesSheet::~CPropertyNodesSheet()
{
	if (1)	/* TM20120830 */
	{			
		// Moeten controleren of de node nog geldig is...
		// Kan gebeuren bij afsluiten van de applicatie dat de ptr reeds verwijderd 
		// is uit de node database.

		if (CNodeDatabase_Util::isValid(*TMGetDocumentNodes(),pSelNode))
		{
			BYTE bNodeAddress = 0xFF;
			if (pSelNode != 0) {
				bNodeAddress = pSelNode->getNodeAddress();	
			}
			AfxGetMainWnd()->PostMessage(WM_PROPERTYSHEET_NODES_EXIT,bNodeAddress,0);
		}
	}

	for (int i=0;i<MAX_PAGES;i++) 
	{
		if (m_pPages[i] != 0) {
			delete m_pPages[i];
			m_pPages[i] = 0;
		}
	}
}


BEGIN_MESSAGE_MAP(CPropertyNodesSheet, CPropertySheet)		// CHANGES_MVS_2008 : Afleiden van CPropertySheet
	//{{AFX_MSG_MAP(CPropertyNodesSheet)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_NODE_STATUS_CHANGED,OnNodeStatusChanged)
	// ON_MESSAGE(WM_SETACTIVE_PAGE, OnShowActivePage)
	ON_COMMAND(IDC_MYBUTTON_RETURN,DestroyPropertySheet)

END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////


// zie BUG-0165: Extra controle doen op het type van bitmaps config.
// De nodes V65 en V66 hebben geen property page.
static bool propertyPageBitmapsCfg(CCanNode* pNode) {
	if (CNodeBitmapConfig_Util::isSupported(pNode)) {				
		const NodeBitmapConfigtype cfgType = CNodeBitmapConfig_Util::getNodetype(pNode);
		if (NODEBITMAP_CONFIG_TYPE_V62_BASIC == cfgType ||
			NODEBITMAP_CONFIG_TYPE_V63_BASIC == cfgType ||
			NODEBITMAP_CONFIG_TYPE_V64_BASIC == cfgType ||
			NODEBITMAP_CONFIG_TYPE_V64_HC4 == cfgType ||
			NODEBITMAP_CONFIG_TYPE_V64_HC5 == cfgType) 
		{		
			return true;
		}
	}
	return false;
}

/****************************************************************************
 * Moeten een conversie doen naar het pagina nummer.
 * Als er lege pagina's aanwezig zijn dan moeten deze er van tussen gehaald worden!
 ****************************************************************************
 * NODE_BINDINGS = 0,
 * NODE_RESOURCES = 1,
 * NODE_CONTROL = 2,
 * NODE_SCREENMAPS = 3,
 * NODE_BITMAPS = 4,
 * NODE_BACKUP = 5,
 * NODE_CONFIGURATION = 6,		
 * MAX_PAGES 
 ****************************************************************************/
int CPropertyNodesSheet::convertPageNr(PropertyNodesSheetPage_t page,CCanNode* pNode) {
	int retValue = 1;
	switch(page) {
		case NODES_PAGE_DEFAULT: 
		case NODES_PAGE_RESOURCES: {
			retValue = 1;							 
		} break;					
		case NODES_PAGE_CONTROL: {
			retValue = NODE_CONTROL;					 
		} break;
		case NODES_PAGE_SMAP: {
			if ((CNodeScreenmapInfo_Util::isSupported(pNode)) ||
				(CNodeScreenmapLiteInfo_Util::isSupported(pNode))) {
				retValue = 3;
			}
		} break;	
		case NODES_PAGE_CONFIG: {
			if (CNodeExtConfigInfo_Util::isSupported(pNode)) {
				retValue = NODE_CONTROL + 1;
				if ((CNodeScreenmapInfo_Util::isSupported(pNode))||(CNodeScreenmapLiteInfo_Util::isSupported(pNode))) {
					retValue++;
				}
				if (propertyPageBitmapsCfg(pNode)) {							
					retValue++;				
				}							
			}								
		} break;
		case NODES_PAGE_MASTERTASKS: {
			if (showPropertyPageHomeClient(pNode)) {
				retValue = NODE_CONTROL + 1;
				if (CNodeExtConfigInfo_Util::isSupported(pNode)) {
					retValue++;	
				}
				if ((CNodeScreenmapInfo_Util::isSupported(pNode))||(CNodeScreenmapLiteInfo_Util::isSupported(pNode))) {
					retValue++;
				}
				if (propertyPageBitmapsCfg(pNode)) {
					retValue++;
				}	
			}
		} break;
	}
	return retValue;
}

// Oppassen: setActivePage moeten het pagina nummer meegeven.
// Indien er blanco pagina's aanwezig zijn in het m_pPages array dan moeten deze er van 
// tussen gehaald worden !
void CPropertyNodesSheet::setPage(PropertyNodesSheetPage_t page) {
	CCanNode* const pNode = this->GetAssignedNode();
	if (NULL == pNode) {
		return;
	}

	int pageNR = convertPageNr(page,pNode);
	pageNR = normalizePageNr(pageNR);
	this->SetActivePage(pageNR);
}

/////////////////////////////////////////////////////////////////////////////
// CPropertyNodesSheet message handlers
LONG CPropertyNodesSheet::OnNodeStatusChanged(WPARAM wParam,LPARAM lParam)
{
	ASSERT(pSelNode);	
	const BYTE bNodeAddress = (BYTE) (wParam & 0xFF);
	if (bNodeAddress == pSelNode->bLogicalAddress) {
		for (int i=0;i<MAX_PAGES;i++) {
			if (m_pPages[i] != 0) 
				m_pPages[i]->UpdateRunTimeControls();
		}
	}
	return TRUE;
}

void CPropertyNodesSheet::PostNcDestroy() 
{

}

void CPropertyNodesSheet::DestroyPropertySheet(void)
{
	CPropertyNodesSheet_Holder::Destroy();
}

void CPropertyNodesSheet::OnClose() 
{
	DestroyPropertySheet();
}

// Sedert V16.14
int CPropertyNodesSheet::normalizePageNr(int page) {
	if (page < 0) {
		page = 0;
	}
	else 
	{
		int count = 0;
/*
		if (NULL != this->GetSafeHwnd()) {
			count = this->GetPageCount();
		}
		else {
*/
			for (int i=0;i<MAX_PAGES;++i) {
				if (0 != m_pPages[i]) {
					count++;
				}
			}
/*		} */
		if (page >= count) {
			page = count-1;
		}
	}
	return page;
}

// Q146916
BOOL CPropertyNodesSheet::OnInitDialog() 
{
	CPropertySheet::OnInitDialog();
	static const CSize szButtonReturn(75,25);
	 CDialogLayoutHelper dlgLayoutHelper(this);
	 CRect rc = dlgLayoutHelper.getScreenWindowPos();
	 const CRect rectBorders = dlgLayoutHelper.getWindowBorders();
	 int increaseHeightValue = rectBorders.bottom + rectBorders.top;		// Borders top & bottom.
	 const CRect rectClient = dlgLayoutHelper.getClientAreaRectangle();
   
     // Increase the height of the CPropertySheet by 30
     rc.bottom += increaseHeightValue;
     // Resize the CPropertySheet
     MoveWindow (rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top);
     // Convert to client coordinates
     ScreenToClient (&rc);

     // m_Button is of type CButton and is a member of CMySheet
	 const CRect rcBtnReturn((rectClient.right - (szButtonReturn.cx + rectBorders.right)),									// left
				 		     ((rectClient.bottom + increaseHeightValue) - (szButtonReturn.cy + rectBorders.bottom)),		// top
							  rectClient.right - rectBorders.left,															// right
							 (rectClient.bottom + increaseHeightValue) - rectBorders.bottom);								// bottom

     m_Button.Create ("Return", WS_CHILD | WS_VISIBLE, rcBtnReturn, this, IDC_MYBUTTON_RETURN);

	 CFont *pFont = GetFont();	
	 if (pFont != NULL) {
		 m_Button.SetFont(pFont);
	 }
	 
	 // Sedert V16.14
	 this->SetActivePage(normalizePageNr(m_firstPage));

#if(0)
	 // Zorgt ervoor dat de close button niet meer getoond wordt.
	 ModifyStyle( WS_SYSMENU, 0 );
#endif

	 DEBUG_DIALOG_LAYOUT(this);
     return TRUE;
}

