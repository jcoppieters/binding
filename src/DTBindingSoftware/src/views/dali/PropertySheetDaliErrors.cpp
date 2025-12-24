// PropertyNodesSheet.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "PropertySheetDaliErrors.h"
#include "PropertyPageDaliErrorsOverview.h"
#include "PropertyPageDaliErrorsDetail.h"
#include "app/appdali/appdali_common.h"
#include "appdlgdaliworker.h"					// Voorlopig hier ????
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
CPropertySheetDaliErrors* CPropertySheetDaliErrors_Holder::m_pInstance = NULL;
/////////////////////////////////////////////////////////////////////////////
CPropertySheetDaliErrors* CPropertySheetDaliErrors_Holder::Get() {
	return m_pInstance;
}

CPropertySheetDaliErrors* CPropertySheetDaliErrors_Holder::Create(CWnd* pParent,CAppDlgDaliWorker& refAppDaliWorker) {
	if (NULL != m_pInstance) {	
		ASSERT(NULL == m_pInstance);
		return NULL;
	}		
	
	m_pInstance = new CPropertySheetDaliErrors("DALI Bus Errors",pParent,refAppDaliWorker);
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


void CPropertySheetDaliErrors_Holder::Destroy() {
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
IMPLEMENT_DYNAMIC(CPropertySheetDaliErrors, CPropertySheet)		

CPropertySheetDaliErrors::CPropertySheetDaliErrors(LPCTSTR pszCaption, CWnd* pParentWnd, CAppDlgDaliWorker& refWorker,UINT iSelectPage)
	: CPropertySheet(pszCaption, pParentWnd), m_refWorker(refWorker), m_pContext(new CPropertyPageDaliErrorsContext())
{		
	for (int i=0;i<MAX_PAGES;i++) {	
		this->m_pPages[i] = NULL;	
	}	
	this->initModel(m_refWorker);
	this->createPages( );
}

CPropertySheetDaliErrors::~CPropertySheetDaliErrors() {	
	for (int i=0;i<MAX_PAGES;i++) {
		if (NULL !=m_pPages[i]) {
			delete m_pPages[i];
			m_pPages[i] = 0;
		}
	}

	if (NULL != m_pContext) {
		delete m_pContext;
		m_pContext = NULL;
	}
}


BEGIN_MESSAGE_MAP(CPropertySheetDaliErrors, CPropertySheet)		// CHANGES_MVS_2008 : Afleiden van CPropertySheet
	//{{AFX_MSG_MAP(CPropertyNodesSheet)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	// ON_MESSAGE(WM_NODE_STATUS_CHANGED,OnNodeStatusChanged)
	// ON_MESSAGE(WM_SETACTIVE_PAGE, OnShowActivePage)
	ON_COMMAND(IDC_MYBUTTON_RETURN,DestroyPropertySheet)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////

void CPropertySheetDaliErrors::initContext( ) { }

void CPropertySheetDaliErrors::createPages( ) {

	if (NULL == this->m_pContext) {
		ASSERT(NULL != this->m_pContext);
		return;
	}
	
	// Pagina 0: Welcome.	
	this->m_pPages[0] = new CPropertyPageDaliErrorsOverview(m_refWorker,*this->m_pContext);
	this->m_pPages[0]->Construct(CPropertyPageDaliErrorsOverview::IDD);	
	this->m_pPages[0]->m_psp.dwFlags |= PSP_USETITLE;
	this->m_pPages[0]->m_psp.pszTitle = _T("Overview");	

	// Pagina 1: Network.
	this->m_pPages[1] = new CPropertyPageDaliErrorsDetail(m_refWorker,*this->m_pContext);
	this->m_pPages[1]->Construct(CPropertyPageDaliErrorsDetail::IDD);	
	this->m_pPages[1]->m_psp.dwFlags |= PSP_USETITLE;
	this->m_pPages[1]->m_psp.pszTitle = _T("Details");	

	// alle paginas toevoegen...
	for (int i=0;i<MAX_PAGES;i++) {
		if (NULL != this->m_pPages[i]) {
			AddPage(this->m_pPages[i]);				
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPropertySheetDali message handlers
void CPropertySheetDaliErrors::PostNcDestroy() {
}

// Worker afsluiten.
// GUI opkuisen.
void CPropertySheetDaliErrors::DestroyPropertySheet(void) {
/*
	if (FALSE == m_refWorker.closeDialog()) {
		ASSERT(0);
	}
*/
	CPropertySheetDaliErrors_Holder::Destroy();
}


// Wanneer er op de exit button rechtsboven gedrukt wordt.
// Wanneer er een taak bezig is dan vragen aan de gebruiker of de dialoog mag afgesloten worden.
void CPropertySheetDaliErrors::OnClose()  {
	//if (this->isCloseDialog()) {
		DestroyPropertySheet();
	//}
}

// Q146916
BOOL CPropertySheetDaliErrors::OnInitDialog() 
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
						    ((rectClient.bottom + increaseHeightValue) - (szButtonReturn.cy + rectBorders.bottom)),		// top
							 rectClient.right - rectBorders.left,														// right
							(rectClient.bottom + increaseHeightValue) - rectBorders.bottom);							// bottom

     m_Button.Create ("Exit", WS_CHILD | WS_VISIBLE,rcBtnReturn,this, IDC_MYBUTTON_RETURN);

	 CFont *pFont = GetFont();	
	 if (pFont != NULL) {
		 m_Button.SetFont(pFont);
	 }	


/*
	 if (FALSE == this->m_refWorker.initOnOpenDialog(this)) {
		ASSERT(0);
	 }
*/
     return TRUE;
}


void CPropertySheetDaliErrors::initModel(CAppDlgDaliWorker& refWorker) {
	if (NULL != this->m_pContext) {
		CAppDaliFsmErrorEntries* pendingErrorsCopy = NULL;
		CAppDaliFsmErrorEntries* const pendingErrors = refWorker.getPendingErrors();
		ASSERT(NULL != pendingErrors);
		if (NULL != pendingErrors) {			
			pendingErrorsCopy = pendingErrors->clone();
			ASSERT(NULL != pendingErrorsCopy);
		}		
		this->m_pContext->setFsmErrors(pendingErrorsCopy);
	}
}
/*****************************************************************************************************/