// DlgUnitProp.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgUnitProp.h"

#include "MyWindowMessages.h"
#include "DlgUnitChildProp.h"
#include "FactoryDlgUnitProp.h"

// --------------------------------
#include "TMGlobals.h"
using namespace TMGraphics;
// ---------------------------------

#undef ENABLE_DEBUG_DIALOG_LAYOUT
/////////////////////////////////////////////////////////////////////////////
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

// ----------------------------------
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
#define TRACER_POINT_TOPLEFT(x)		TRACE x
// #define TRACER_POINT_TOPLEFT(x)		// TRACE x


/////////////////////////////////////////////////////////////////////////////
#if(0)
static void NormalizeWindowRect_Screens(CWnd* parent,CRect* rect) {

	const int screen_size_x = GetSystemMetrics(SM_CXSCREEN);
	const int screen_size_y = GetSystemMetrics(SM_CYSCREEN);
	
	int newX = -1;
	int newY = -1;

	// X-Coordinaten.
	if (rect->TopLeft().x + rect->Size().cx	>= screen_size_x) {
		newX = screen_size_x - rect->Size().cx - 1;
	}	
	// Y-Coordinaten.
	if (rect->TopLeft().y + rect->Size().cy >= screen_size_y) {
		newY = screen_size_y - rect->Size().cy - 1;
	}

	if (newX>=0) {
		rect->MoveToX(newX);	
	}
	if (newY>=0) {
		rect->MoveToY(newY);	
	}
}

static void NormalizeWindowRect_Screens2(CWnd* parent,CRect* rect) {

	const int screen_size_x = GetSystemMetrics(SM_CXSCREEN);
	const int screen_size_y = GetSystemMetrics(SM_CYSCREEN);

	bool convertToClient = false;
	int newX = -1;
	int newY = -1;

	// X-Coordinaten.
	if (rect->TopLeft().x > screen_size_x) {
		newX = rect->TopLeft().x - screen_size_x;	
		convertToClient = true;
	}
	else {
		if (rect->TopLeft().x + rect->Size().cx	>= screen_size_x) {
			newX = screen_size_x - rect->Size().cx - 1;
		}
	}

	// Y-Coordinaten.
	if (rect->TopLeft().y + rect->Size().cy >= screen_size_y) {
		newY = screen_size_y - rect->Size().cy - 1;
	}

	if (newX>=0) {
		rect->MoveToX(newX);	
	}
	if (newY>=0) {
		rect->MoveToY(newY);	
	}

	if (convertToClient) {
		parent->ClientToScreen(rect);	
	}
}
#endif 


/////////////////////////////////////////////////////////////////////////////
CDlgUnitProp* CDlgUnitProp::m_pDlgUnitProp = NULL;
STRUCT_UNIT* CDlgUnitProp::m_pDocUnit = NULL;
const CCanNode* CDlgUnitProp::m_pDocNode = NULL;
/////////////////////////////////////////////////////////////////////////////
// CDlgUnitProp dialog

CDlgUnitProp::CDlgUnitProp(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgUnitProp::IDD, pParent), pChildDialog(NULL),
	 m_rcChildFrame(0,0,0,0), m_rcMainFrame(0,0,0,0), m_rcFullFrame(0,0,0,0)
{
	//{{AFX_DATA_INIT(CDlgUnitProp)
	m_szAddress = _T("");
	m_szUnitName = _T("");
	//}}AFX_DATA_INIT

	Create(IDD_DIALOG_PROP_UNIT);

	fShowBindingProperties = FALSE;	
	fChildCreated = FALSE;
	pChildDialog = NULL;
}

void CDlgUnitProp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgUnitProp)
	DDX_Control(pDX, IDC_BUTTON_RETURN, m_BtnReturn);
	DDX_Control(pDX, IDC_BUTTON_EXPAND, m_BtnExpand);
	DDX_Control(pDX, IDC_BUTTON_CONTROL, m_BtnControl);
	DDX_Text(pDX, IDC_STATIC_ADDRESS, m_szAddress);
	DDX_Text(pDX, IDC_STATIC_NAME, m_szUnitName);
	DDX_Control(pDX, IDC_STATIC_INFO_LABEL, m_staticUnitInfoLabel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgUnitProp, CDialog)
	//{{AFX_MSG_MAP(CDlgUnitProp)
	ON_BN_CLICKED(IDC_BUTTON_RETURN, OnButtonReturn)
	ON_BN_CLICKED(IDC_BUTTON_EXPAND, OnButtonExpand)
	ON_WM_MOVE()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	//ON_MESSAGE(WM_UNIT_STATUS_CHANGED,OnUnitStatusChanged)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

// Sedert versie 0x0900: Ook de referentie naar de node meegeven.
BOOL CDlgUnitProp::Show(const CCanNode* pNode,STRUCT_UNIT *pUnit,const CString& strLabelText)
{
	if (m_pDlgUnitProp != NULL) {
		if (m_pDlgUnitProp->GetTypeOfDialog() != pUnit->bUnitType) {
			m_pDlgUnitProp->Destroy();
		}
	}
	
	m_pDocUnit = pUnit;
	m_pDocNode = pNode;

	if (m_pDlgUnitProp == NULL) {
		m_pDlgUnitProp = CFactoryDlgUnitProp::Create(pNode,pUnit);
	}

	if (m_pDlgUnitProp != NULL) {
		m_pDlgUnitProp->InitControls(strLabelText);
		m_pDlgUnitProp->ShowWindow(TRUE);
		m_pDlgUnitProp->SetActiveWindow();			
	}
	
	return TRUE;
}

BOOL CDlgUnitProp::Destroy(void) {
	if (m_pDlgUnitProp != NULL) {
		m_pDlgUnitProp->DeleteControls();		
		m_pDlgUnitProp = NULL;	
	}
	ASSERT(m_pDlgUnitProp == NULL);
	return TRUE;
}

BOOL CDlgUnitProp::OnUnitStatusChanged(BYTE bNodeAddress,BYTE bUnitAddress) {
	if (m_pDlgUnitProp != NULL) {
		ASSERT(0!=m_pDocUnit);
		if ((m_pDocUnit->bNodeLogicalAddress == bNodeAddress) &&
			(m_pDocUnit->bUnitAddress == bUnitAddress)) {
			m_pDlgUnitProp->fStatusReceived = TRUE;
			m_pDlgUnitProp->UpdateRuntimeProperties();
		}
		return TRUE;
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
// CDlgUnitProp message handlers
void CDlgUnitProp::Subscribe(CDlgUnitChildProp *pChild)
{
	ASSERT(pChild);
	pChildDialog = pChild;
}

void CDlgUnitProp::unSubscribe(void) 
{
	pChildDialog= NULL;
}

void CDlgUnitProp::NotifyChild(void) 
{
	if (pChildDialog != NULL) 
	{
		pChildDialog->OnNotifyClose();
	}
}

#if(0)		// Ondersteuning van meerdere monitors. (Niet meer nodig sedert V17.02)
void CDlgUnitProp::NormalizeWindowRect(CRect* rect) {
	const int numberOfMonitors = GetSystemMetrics(SM_CMONITORS);
	if (numberOfMonitors > 1) {
		NormalizeWindowRect_Screens2(this,rect);
	}
	else {
		NormalizeWindowRect_Screens(this,rect);
	}
}
#endif

/////////////////////////////////////////////////////////////////////////////
void CDlgUnitProp::InitControls(const CString& strLabelText) {
	ASSERT(m_pDocUnit);
	this->fStatusReceived = FALSE;
	this->m_bTypeOfDialog = m_pDocUnit->bUnitType;	
	InitStaticControls(strLabelText);
	if (TMGetGuiUnit()->GetRuntimeUnit(m_pDocUnit->bNodeLogicalAddress,m_pDocUnit->bUnitAddress,&pRuntimeUnit) == FALSE) {
		AfxMessageBox("Fatal Error, Unable to get runtime unit!");
	}
	UpdateData(FALSE);

	// Virtuele functie aanroepen voor de afgeleide dailogen...
	this->InitControls();
}

void CDlgUnitProp::InitStaticControls(const CString& strLabelText)
{
	ASSERT(m_pDocUnit);

	const BYTE bNodeAddress = m_pDocUnit->bNodeLogicalAddress;
	const BYTE bUnitAddress = m_pDocUnit->bUnitAddress;

	m_BtnReturn.SetIcon(TMGetGraphics()->BtnReturn);

	// init button : Give node + unit Address
	m_BtnControl.Init(bNodeAddress,bUnitAddress);
	
	// Init Static Name + Address
	m_szAddress.Format("Address: 0x%02x,Unit 0x%02x",bNodeAddress,bUnitAddress);
	m_szUnitName.Format("Name: %s",m_pDocUnit->szUnitName);

	// Updaten van extra text label.
	const BOOL flagShowLabel = strLabelText.IsEmpty() ? FALSE : TRUE;
	m_staticUnitInfoLabel.ShowWindow(flagShowLabel);
	if (flagShowLabel) {
		const CString str = "Info: " + strLabelText;
		m_staticUnitInfoLabel.SetWindowText(str);
	}
}

void CDlgUnitProp::UpdateRuntimeProperties() {	
	CApplicationGuiUnit::AppUiIconsExtraInfo_t appUiExtraInfo = CApplicationGuiUnit::APPUI_ICONS_INFO_NONE;
	if ((NULL != m_pDocUnit->m_pAppUnitData) && (CAppUnitDataDali::TYPE_UNIT_DATA_DALI == m_pDocUnit->m_pAppUnitData->getUnitDataType())) {				
		appUiExtraInfo = CApplicationGuiUnit::APPUI_ICONS_INFO_DALI;
	}

	UnitIconType iImage;
	TMGetGuiUnit()->GetIcon(m_pDocUnit->bNodeLogicalAddress, m_pDocUnit->bUnitAddress, appUiExtraInfo, &iImage );			
	const HICON hIcon = TMGetGraphics()->UnitIcons[iImage];
	ASSERT(NULL != hIcon);
	m_BtnControl.SetIcon(hIcon);
}

///////////////////////////////////////////////////////

void CDlgUnitProp::PostNcDestroy() 
{
	CDialog::PostNcDestroy();

	if (m_pDlgUnitProp != NULL) {
		delete m_pDlgUnitProp;	
		m_pDlgUnitProp = NULL;
	}
}

void CDlgUnitProp::OnButtonReturn() 
{
	NotifyChild();			// ASK CHILD IF CREATED TO STORE SETTINGS !!!!!
	DeleteControls();
}

void CDlgUnitProp::DeleteControls(void)
{
	CleanUp();
	DestroyWindow();
}

void CDlgUnitProp::CalculateDialogCoordinates()
{
	(GetDlgItem(IDC_GROUP_CHILD))->GetWindowRect(&m_rcChildFrame);
	
	// The ScreenToClient function converts the screen coordinates of a 
	// specified point on the screen to client-area coordinates.
	ScreenToClient(&m_rcChildFrame);

	// Retrieves the coordinates of a window's client area. 
	// The client coordinates specify the upper-left and lower-right corners of 
	// the client area. 
	// Because client coordinates are relative to the upper-left corner of a window's client area, 
	// the coordinates of the upper-left corner are (0,0).	
	GetClientRect(&this->m_rcFullFrame);	

	CRect rectWindow;
	GetWindowRect(&rectWindow);
	const CSize borderHeader(rectWindow.Width() - this->m_rcFullFrame.Width(), rectWindow.Height() - this->m_rcFullFrame.Height());

	// Mainframe area => Onderkant stopt waar de childframe start.
	this->m_rcMainFrame = this->m_rcFullFrame;
	this->m_rcMainFrame.bottom = this->m_rcChildFrame.top;

	// Bij alle rectangle area's wordt de top (header) + border bottom bijgevoegd.
	this->m_rcMainFrame.bottom += borderHeader.cy;
	this->m_rcFullFrame.bottom += borderHeader.cy;
	this->m_rcChildFrame.bottom += borderHeader.cy;

	// V17.02: Rechterkant breeder maken.
	this->m_rcMainFrame.right += borderHeader.cx;
	this->m_rcFullFrame.right += borderHeader.cx;
	this->m_rcChildFrame.right += borderHeader.cx;
}

void CDlgUnitProp::ShowFullDialog(BOOL fShow)
{
	const CPoint ptTopLeft(this->getPointTopLeft());
	const CSize size(getRectMainFrame().Size());
	m_BtnExpand.ShowWindow(FALSE);
	MoveWindow(ptTopLeft.x,ptTopLeft.y,size.cx,size.cy);
}

void CDlgUnitProp::OnButtonExpand() 
{
	fShowBindingProperties ^= TRUE;
	ShowFullDialog(fShowBindingProperties);
}


CPoint CDlgUnitProp::getPointTopLeft() {
	CRect rectWindow;
	GetWindowRect(&rectWindow);	
	TRACER_POINT_TOPLEFT(("Window topLeft= %d,%d\r\n", rectWindow.TopLeft().x, rectWindow.TopLeft().y));	
#if(0)
	NormalizeWindowRect(&rectWindow);	// Ondersteuning van meerdere monitors. (Niet meer nodig sedert V17.02)
#endif
	return rectWindow.TopLeft();
}

void CDlgUnitProp::OnMove(int x, int y) {
	CDialog::OnMove(x, y);	
	DEBUG_DIALOG_LAYOUT(this);	
}

void CDlgUnitProp::OnSetFont(CFont* pFont) 
{
	pDefaultFont = pFont;	
	CDialog::OnSetFont(pFont);
}

void CDlgUnitProp::OnClose() {
	this->CleanUp();
	this->DestroyWindow();
}

