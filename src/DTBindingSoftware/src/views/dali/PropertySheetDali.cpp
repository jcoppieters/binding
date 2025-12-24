// PropertyNodesSheet.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "PropertySheetDali.h"
#include "PropertyPageDaliNwk.h"
#include "PropertyPageDaliDevices.h"
#include "PropertyPageDaliLocate.h"
#include "PropertyPageDaliWelcome.h"
#include "PropertyPageDaliGroupsOverview.h"
#include "PropertyPageDaliGroupsOverview2.h"
#include "propertySheetDaliErrors.h"			// Weergave van de fouten zie USE_DEBUG_PROPERTYSHEET_ERRORS
#include "appdlgdaliworker.h"					// Voorlopig hier ????
#include "app/AppDali/appdali_common.h"
#include "Mainfrm.h"
#include "utils/mfc/DialogLayoutHelper.h"	// V17.02
/////////////////////////////////////////////////////////////////////////////
using namespace DUOTECNO::DALI;
/////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
CPropertySheetDali* CPropertySheetDali_Holder::m_pInstance = NULL;
/////////////////////////////////////////////////////////////////////////////
CPropertySheetDali* CPropertySheetDali_Holder::Get() {
	return m_pInstance;
}

CPropertySheetDali* CPropertySheetDali_Holder::Create(CWnd* pParent,CAppDlgDaliWorker& refAppDaliWorker) {
	if (NULL != m_pInstance) {	
		ASSERT(NULL == m_pInstance);
		return NULL;
	}		
	
	m_pInstance = new CPropertySheetDali("DALI Bus config",pParent,refAppDaliWorker);
	if (NULL == m_pInstance) {		
		ASSERT(NULL != m_pInstance);
		return NULL;
	}

	const BOOL fResult = m_pInstance->Create(pParent,
											 WS_SYSMENU |  WS_POPUP | WS_CAPTION | DS_MODALFRAME | WS_VISIBLE,
											 WS_EX_DLGMODALFRAME);										  
	if (FALSE == fResult) {	
		delete m_pInstance;
		m_pInstance = NULL;
		return NULL;
	}				
	ASSERT(m_pInstance->GetSafeHwnd());	
	return m_pInstance;							
}


void CPropertySheetDali_Holder::Destroy() {
	if (NULL == m_pInstance) {
		return;
	}	
	if (NULL != m_pInstance->GetSafeHwnd()) {
		m_pInstance->DestroyWindow();
	}	
	ASSERT(m_pInstance->GetSafeHwnd() == NULL);
	delete m_pInstance;
	m_pInstance = NULL;			
}


/////////////////////////////////////////////////////////////////////////////
// CPropertySheetDali
IMPLEMENT_DYNAMIC(CPropertySheetDali, CPropertySheet)		

CPropertySheetDali::CPropertySheetDali(LPCTSTR pszCaption, CWnd* pParentWnd, CAppDlgDaliWorker& refWorker,UINT iSelectPage)
	: CPropertySheet(pszCaption, pParentWnd), m_pContext(new CPropertyPageDaliContext()), m_refWorker(refWorker)
{		
	ASSERT(NULL != m_pContext);
	for (int i=0;i<MAX_PAGES;i++) {	
		this->m_pPages[i] = NULL;	
	}	
	this->initContext( );
	this->createPages( );
}

CPropertySheetDali::~CPropertySheetDali() {	
	for (int i=0;i<MAX_PAGES;i++) {
		if (NULL !=m_pPages[i]) {
			delete m_pPages[i];
			m_pPages[i] = 0;
		}
	}
	if (NULL != m_pContext) {
		delete m_pContext;
	}
}


BEGIN_MESSAGE_MAP(CPropertySheetDali, CPropertySheet)		// CHANGES_MVS_2008 : Afleiden van CPropertySheet
	//{{AFX_MSG_MAP(CPropertyNodesSheet)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	// ON_MESSAGE(WM_NODE_STATUS_CHANGED,OnNodeStatusChanged)
	// ON_MESSAGE(WM_SETACTIVE_PAGE, OnShowActivePage)
	ON_COMMAND(IDC_MYBUTTON_RETURN,DestroyPropertySheet)
	ON_WM_TIMER()
	ON_MESSAGE(WM_PROPERTYSHEET_DIALOG_DALI_ERRORS_CLOSE,onPropertySheetDaliDialogErrorsClose)
	ON_MESSAGE(WM_PROPERTYSHEET_DIALOG_DALI_ERRORS_OPEN,onPropertySheetDaliDialogErrorsOpen)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////

void CPropertySheetDali::initContext( ) { }

void CPropertySheetDali::createPages( ) {

	if (NULL == this->m_pContext) {
		ASSERT(this->m_pContext);
		return;
	}

	// Pagina 0: Welcome.	
	this->m_pPages[0] = new CPropertyPageDaliWelcome(m_refWorker, *this->m_pContext);
	this->m_pPages[0]->Construct(CPropertyPageDaliWelcome::IDD);	
	this->m_pPages[0]->m_psp.dwFlags |= PSP_USETITLE;
	this->m_pPages[0]->m_psp.pszTitle = _T("Welcome");	

	// Pagina 1: Network.
	this->m_pPages[1] = new CPropertyPageDaliNwk(m_refWorker, *this->m_pContext);
	this->m_pPages[1]->Construct(CPropertyPageDaliNwk::IDD);	
	this->m_pPages[1]->m_psp.dwFlags |= PSP_USETITLE;
	this->m_pPages[1]->m_psp.pszTitle = _T("Network");	

	// Pagina 2: Devices
	this->m_pPages[2] = new CPropertyPageDaliDevices(m_refWorker, *this->m_pContext);
	this->m_pPages[2]->Construct(CPropertyPageDaliDevices::IDD);	
	this->m_pPages[2]->m_psp.dwFlags |= PSP_USETITLE;
	this->m_pPages[2]->m_psp.pszTitle = _T("Devices");	

#if(0)
	// Pagina 3: Groups overview
	this->m_pPages[3] = new CPropertyPageDaliGroupsOverview(m_refWorker, *this->m_pContext);
	this->m_pPages[3]->Construct(CPropertyPageDaliGroupsOverview::IDD);	
	this->m_pPages[3]->m_psp.dwFlags |= PSP_USETITLE;
	this->m_pPages[3]->m_psp.pszTitle = _T("Groups overview");	
#endif 

	// Pagina 3: Groups overview
	this->m_pPages[3] = new CPropertyPageDaliGroupsOverview2(m_refWorker, *this->m_pContext);
	this->m_pPages[3]->Construct(CPropertyPageDaliGroupsOverview2::IDD);	
	this->m_pPages[3]->m_psp.dwFlags |= PSP_USETITLE;
	this->m_pPages[3]->m_psp.pszTitle = _T("Groups overview");	

	// Pagina 4: Locate devices
	this->m_pPages[4] = new CPropertyPageDaliLocate(m_refWorker, *this->m_pContext);
	this->m_pPages[4]->Construct(CPropertyPageDaliLocate::IDD);	
	this->m_pPages[4]->m_psp.dwFlags |= PSP_USETITLE;
	this->m_pPages[4]->m_psp.pszTitle = _T("Locate devices");	


	// alle paginas toevoegen...
	for (int i=0;i<MAX_PAGES;i++) {
		if (NULL != this->m_pPages[i]) {
			AddPage(this->m_pPages[i]);				
		}
	}
}

void CPropertySheetDali::initTabCtrlToolTips( ) {

    CTabCtrl* pTabCtrl = GetTabControl();
    if (NULL == pTabCtrl) {
		return;
	}
       
	m_ToolTip.Create(pTabCtrl);
    m_ToolTip.Activate(TRUE);

    for (int i = 0; i < pTabCtrl->GetItemCount(); ++i) {
		CRect rect;
        pTabCtrl->GetItemRect(i, &rect);
        pTabCtrl->MapWindowPoints(this, &rect);

        CString tip;
		switch (i) {
			case 0: tip = _T("Welcome page"); break;
            case 1: tip = _T("Search or address devices in the Dali network"); break;
			case 2: tip = _T("Dali device list and properties"); break;
			case 3: tip = _T("Dali group configuration"); break;
			case 4: tip = _T("Locate Dali input devices"); break;			
		}
		if (!tip.IsEmpty()) {
			m_ToolTip.AddTool(pTabCtrl, tip, &rect, i + 1);
		}
	}    
}

BOOL CPropertySheetDali::PreTranslateMessage(MSG* pMsg)
{
    if (m_ToolTip.m_hWnd != NULL)
        m_ToolTip.RelayEvent(pMsg);

    return CPropertySheet::PreTranslateMessage(pMsg);
}

/////////////////////////////////////////////////////////////////////////////
// CPropertySheetDali message handlers
void CPropertySheetDali::PostNcDestroy() {
}

// Worker afsluiten.
// GUI opkuisen.
void CPropertySheetDali::DestroyPropertySheet(void) {
	if (FALSE == m_refWorker.closeDialog()) {
		ASSERT(0);
	}

	this->KillTimer(TIMER_ID_VISIBILITY_CHECK);
	CPropertySheetDaliErrors_Holder::Destroy();		// Child dialoog.
	CPropertySheetDali_Holder::Destroy();			// Eigen dialoog.
}


// Wanneer er op de exit button rechtsboven gedrukt wordt.
// Wanneer er een taak bezig is dan vragen aan de gebruiker of de dialoog mag afgesloten worden.
void CPropertySheetDali::OnClose()  {
	if (this->isCloseDialog()) {
		DestroyPropertySheet();
	}
}

void CPropertySheetDali::OnTimer(UINT nIDEvent)  {
	if (TIMER_ID_VISIBILITY_CHECK == nIDEvent) {	
		CPropertySheetDaliErrors* const pDialog = CPropertySheetDaliErrors_Holder::Get();
		if (pDialog) {
			if (this->IsWindowVisible() && !pDialog->IsWindowVisible()) {
				// pDialog->SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
				pDialog->ShowWindow(SW_SHOW);
			}		
			else if (!this->IsWindowVisible() && pDialog->IsWindowVisible()) {
				// pDialog->SetWindowPos(&CWnd::wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
				pDialog->ShowWindow(SW_HIDE);
			}
		}
	}	
}

LONG CPropertySheetDali::onPropertySheetDaliDialogErrorsOpen(WPARAM wParam,LPARAM lParam) {
	this->openDialogDaliErrors();
	return TRUE;
}

LONG CPropertySheetDali::onPropertySheetDaliDialogErrorsClose(WPARAM wParam,LPARAM lParam) {
	this->KillTimer(TIMER_ID_VISIBILITY_CHECK);
	CPropertySheetDaliErrors_Holder::Destroy();		// Child dialoog.
	return TRUE;
}

// Q146916
BOOL CPropertySheetDali::OnInitDialog() 
{
	CPropertySheet::OnInitDialog();	
	 static const CSize szButtonReturn(75, 25);
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
	 const CRect rcBtnReturn((rectClient.right - (szButtonReturn.cx + rectBorders.right)),								// left
				  		     ((rectClient.bottom + increaseHeightValue) - (szButtonReturn.cy + rectBorders.bottom)),	// top
							  rectClient.right - rectBorders.left,														// right
							  (rectClient.bottom + increaseHeightValue) - rectBorders.bottom);							// bottom

     m_Button.Create ("Exit", WS_CHILD | WS_VISIBLE, rcBtnReturn, this, IDC_MYBUTTON_RETURN);

	 CFont *pFont = GetFont();	
	 if (pFont != NULL) {
		 m_Button.SetFont(pFont);
	 }	
	 if (FALSE == this->m_refWorker.initOnOpenDialog(this)) {
		ASSERT(0);
	 }

	 this->PostMessage(WM_PROPERTYSHEET_DIALOG_DALI_ERRORS_OPEN);
/* 
	 // Indien er fouten zijn dan wordt de errors dialoog automatisch geopend.
	 this->openDialogDaliErrors();
*/
	 this->initTabCtrlToolTips();
     return TRUE;
}

/*****************************************************************************************************/
// IAppDlgDaliGui_Host	
// Doorgeven aan de property pages.
void CPropertySheetDali::onNotifyGuiStatusConnected(BYTE bNodeAddress, bool statusConnected) {
	if (m_refWorker.getCurrentNodeAddress() != bNodeAddress) {
		return;	
	}

	for (int i=0;i<MAX_PAGES;i++) {
		if ((NULL != this->m_pPages[i]) && (NULL != this->m_pPages[i]->GetSafeHwnd())) {
			this->m_pPages[i]->onNotifyGuiStatusConnected(bNodeAddress, statusConnected);
		}
	}
}
void CPropertySheetDali::onNotifyGuiMsgCommunication(BYTE bNodeAddress, Type_t typeMessage, const CString& msg) {
	if (m_refWorker.getCurrentNodeAddress() != bNodeAddress) {
		return;	
	}

	for (int i=0;i<MAX_PAGES;i++) {
		if ((NULL != this->m_pPages[i]) && (NULL != this->m_pPages[i]->GetSafeHwnd())) {
			this->m_pPages[i]->onNotifyGuiMsgCommunication(bNodeAddress, typeMessage, msg);
		}
	}
}

void CPropertySheetDali::onNotifyGuiTaskStarted(BYTE bNodeAddress, DUOTECNO::DALI::Task_t task) {
	if (m_refWorker.getCurrentNodeAddress() != bNodeAddress) {
		return;	
	}

	// Bij het starten van een taak automatisch de dialoog met fouten sluiten...
	if (APPDALI_FSM_TASK_SEARCH_DEVICES == task ||
		APPDALI_FSM_TASK_COMMISSIONING_EXTEND == task ||
		APPDALI_FSM_TASK_COMMISSIONING_NEW == task) {		
			CPropertySheetDaliErrors_Holder::Destroy();
	}

	for (int i=0;i<MAX_PAGES;i++) {
		if ((NULL != this->m_pPages[i]) && (NULL != this->m_pPages[i]->GetSafeHwnd())) {
			this->m_pPages[i]->onNotifyGuiTaskStarted(bNodeAddress, task);
		}
	}
}

void CPropertySheetDali::onNotifyGuiTaskDone(BYTE bNodeAddress, DUOTECNO::DALI::Task_t task) {
	if (m_refWorker.getCurrentNodeAddress() != bNodeAddress) {
		return;	
	}

	// Indien er fouten zijn dan wordt de errors dialoog automatisch geopend.
	if (APPDALI_FSM_TASK_SEARCH_DEVICES == task ||
		APPDALI_FSM_TASK_COMMISSIONING_EXTEND == task ||
		APPDALI_FSM_TASK_COMMISSIONING_NEW == task) {		
		this->openDialogDaliErrors();
	}

	for (int i=0;i<MAX_PAGES;i++) {
		if ((NULL != this->m_pPages[i]) && (NULL != this->m_pPages[i]->GetSafeHwnd())) {
			this->m_pPages[i]->onNotifyGuiTaskDone(bNodeAddress, task);
		}
	}
}

void CPropertySheetDali::onNotifyGuiTaskError(BYTE bNodeAddress, DUOTECNO::DALI::Task_t task,const CString& strMessage) {
	if (m_refWorker.getCurrentNodeAddress() != bNodeAddress) {
		return;	
	}

	// Indien er fouten zijn dan wordt de errors dialoog automatisch geopend.
	if (APPDALI_FSM_TASK_SEARCH_DEVICES == task ||
		APPDALI_FSM_TASK_COMMISSIONING_EXTEND == task ||
		APPDALI_FSM_TASK_COMMISSIONING_NEW == task) {		
		this->openDialogDaliErrors();
	}

	for (int i=0;i<MAX_PAGES;i++) {
		if ((NULL != this->m_pPages[i]) && (NULL != this->m_pPages[i]->GetSafeHwnd())) {
			this->m_pPages[i]->onNotifyGuiTaskError(bNodeAddress,task,strMessage);
		}
	}
}

void CPropertySheetDali::onNotifyGuiFoundDevice(BYTE bNodeAddress, DUOTECNO::DALI::DaliDeviceType_t dalideviceType,int shortAddress, bool newDevice) {
	if (m_refWorker.getCurrentNodeAddress() != bNodeAddress) {
		return;	
	}

	for (int i=0;i<MAX_PAGES;i++) {
		if ((NULL != this->m_pPages[i]) && (NULL != this->m_pPages[i]->GetSafeHwnd())) {
			this->m_pPages[i]->onNotifyGuiFoundDevice(bNodeAddress, dalideviceType,shortAddress,newDevice);
		}
	}
}

void CPropertySheetDali::onNotifyGuiUpdateDeviceSerialNumber(BYTE bNodeAddress, DUOTECNO::DALI::DaliDeviceType_t dalideviceType,int shortAddress, unsigned long long serialNumber) {
	if (m_refWorker.getCurrentNodeAddress() != bNodeAddress) {
		return;	
	}
	for (int i=0;i<MAX_PAGES;i++) {
		if ((NULL != this->m_pPages[i]) && (NULL != this->m_pPages[i]->GetSafeHwnd())) {
			this->m_pPages[i]->onNotifyGuiUpdateDeviceSerialNumber(bNodeAddress, dalideviceType,shortAddress,serialNumber);
		}
	}
}

void CPropertySheetDali::onNotifyGuiUpdateDeviceGTIN(BYTE bNodeAddress, DUOTECNO::DALI::DaliDeviceType_t dalideviceType,int shortAddress, unsigned long long GTIN) {
	if (m_refWorker.getCurrentNodeAddress() != bNodeAddress) {
		return;	
	}
	for (int i=0;i<MAX_PAGES;i++) {
		if ((NULL != this->m_pPages[i]) && (NULL != this->m_pPages[i]->GetSafeHwnd())) {
			this->m_pPages[i]->onNotifyGuiUpdateDeviceGTIN(bNodeAddress, dalideviceType,shortAddress,GTIN);
		}
	}
}

void CPropertySheetDali::onNotifyGuiRxDaliMessage(BYTE bNodeAddress,BYTE lengthBits,uint32_t recievedFrame) {
	if (m_refWorker.getCurrentNodeAddress() != bNodeAddress) {
		return;	
	}
	for (int i=0;i<MAX_PAGES;i++) {
		if ((NULL != this->m_pPages[i]) && (NULL != this->m_pPages[i]->GetSafeHwnd())) {
			this->m_pPages[i]->onNotifyGuiRxDaliMessage(bNodeAddress, lengthBits,recievedFrame);
		}
	}
}

// Overriden wanneer er op de Return button gedrukt wordt.
// Wanneer er een taak bezig is dan vragen aan de gebruiker of de dialoog mag afgesloten worden.
BOOL CPropertySheetDali::OnCommand(WPARAM wParam, LPARAM lParam) {
	if (wParam == IDC_MYBUTTON_RETURN) { // Retturn button clicked...       
		if (FALSE == this->isCloseDialog()) {
			return TRUE;
		}
    }
	return CPropertySheet::OnCommand(wParam,lParam);
}

// Wanneer er een taak bezig is dan vragen aan de gebruiker of de dialoog mag afgesloten worden.
BOOL CPropertySheetDali::isCloseDialog( ) {	
	if (FALSE == promptUserCfgTaskBusyAskCloseDialog()) {
		return FALSE;
	}
	if (FALSE == promptUserChangesOpenConfiguratorCloseDialog()) {
		return FALSE;
	}
	return TRUE;
}

BOOL CPropertySheetDali::promptUserCfgTaskBusyAskCloseDialog() {
	BOOL taskBusy = FALSE;
	for (int i = 0; i<MAX_PAGES; i++) {
		if (NULL != this->m_pPages[i]) {
			if (this->m_pPages[i]->taskBusy()) {
				taskBusy = TRUE;
				break;
			}
		}
	}
	if (FALSE == taskBusy) {
		return TRUE;
	}
	const CString strTitle("DALI bus configuration busy.");
	const CString msg("If you close this dialog the current task will stop\r\n"
		"Close the dialog and cancel the current task?");
	if (IDYES == this->MessageBox(msg, strTitle, MB_YESNO | MB_ICONQUESTION)) {
		return TRUE;
	}
	return FALSE;
}

BOOL CPropertySheetDali::promptUserChangesOpenConfiguratorCloseDialog() {

	// Indien er fouten aanwezig zijn dan de vraag niet stellen.
	if (this->m_refWorker.getStatePendingErrors()) {
		return TRUE;
	}

	// Indien er geen aanpassingen zijn mogen de dialoog afsluiten.
	if (!this->m_refWorker.getChangesCfgDuotecnoBridgeNeeded()) {
		return TRUE;
	}



#if(0)
	const CString strTitle("Open Duotecno DALI bridge configuration?");
	const CString strMsg("There have been changes to the DALI system bus.\r\n"
		"The adjustments will only work once the Duotecno Bridge is configured with the new settings.\r\n\r\n"
		"Would you like to open the Duotecno DALI Bridge configuration now ? ");

	if (IDNO == this->MessageBox(strMsg, strTitle, MB_YESNO | MB_ICONQUESTION)) {
		return TRUE;
	}


	CWnd* const pWnd = AfxGetMainWnd();
	if (NULL == pWnd) {
		return TRUE;
	}
#endif 


	const CString strTitle("Duotecno DALI bridge configuration.");
	const CString strMsg("There have been changes to the DALI system bus.\r\n"
					     "The adjustments will only work once the Duotecno Bridge is configured with the new settings.\r\n\r\n"
						 "When pressing the 'ok'-button the configuration will open.");
	
	if (IDOK != this->MessageBox(strMsg, strTitle, MB_OK | MB_ICONINFORMATION)) {
		return TRUE;
	}
	
	// Open van de Duotecno DALI bridge + vlag veranderingen wissen.
	this->m_refWorker.openDALIDuotecnoBridgeConfiguration();
	this->m_refWorker.setChangesCfgDuotecnoBridge(false);
	return FALSE;
}


void CPropertySheetDali::openDialogDaliErrors( ) {

	CAppDaliFsmErrorEntries* const pendingErrors = this->m_refWorker.getPendingErrors();
	if (NULL == pendingErrors) {
		return;
	}
	if (pendingErrors->getCount() <= 0) {
		return;
	}
	CPropertySheetDaliErrors* pInstance = CPropertySheetDaliErrors_Holder::Get();
	if (NULL == pInstance) {
		pInstance = CPropertySheetDaliErrors_Holder::Create(this,m_refWorker);		
	}
	if (NULL == pInstance){
		ASSERT(NULL != pInstance);
		return;
	}
	this->SetTimer(TIMER_ID_VISIBILITY_CHECK,1000,(TIMERPROC)NULL);

	// Venster altijd als het bovenste venster zetten...
	pInstance->SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	pInstance->ShowWindow(SW_SHOWNORMAL);		

	// TODO: Evt wanneer de dialoog niet meer actief is de modeless dialoog niet meer in fout zetten??
	// m_pChildDialog->SetWindowPos(&CWnd::wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}
/*****************************************************************************************************/