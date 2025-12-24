// DlgUnitChildProp.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgUnitChildProp.h"
#include "DlgUnitProp.h"

// --------------------------------
#include "TMGlobals.h"
using namespace TMGraphics;
// ---------------------------------

// --------------------------------------------
#undef ENABLE_DEBUG_DIALOG_LAYOUT
// --------------------------------------------
#ifdef _DEBUG
#define ENABLE_DEBUG_DIALOG_LAYOUT				0			// Op 1 plaatsen om te debuggen.
#else
#define ENABLE_DEBUG_DIALOG_LAYOUT				0
#define DEBUG_DIALOG_LAYOUT(x)
#endif


#if(ENABLE_DEBUG_DIALOG_LAYOUT == 1)
#include "utils/mfc/DebugDialogLayout.h"
#define DEBUG_DIALOG_LAYOUT(x)						CDebugDialogLayout::dumpControlLayout(x)
#else
#define DEBUG_DIALOG_LAYOUT(x)	
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgUnitChildProp dialog


CDlgUnitChildProp::CDlgUnitChildProp(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgUnitChildProp::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgUnitChildProp)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	//MyWindowRect = ((CDlgUnitProp*) pParent)->rcChildFrame;
}


void CDlgUnitChildProp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgUnitChildProp)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgUnitChildProp, CDialog)
	//{{AFX_MSG_MAP(CDlgUnitChildProp)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_DEFAULTS, OnButtonDefaults)
	//ON_BN_CLICKED(IDC_BUTTON_ACCEPTSETTINGS, OnButtonAccept)
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CDlgUnitChildProp message handlers

void CDlgUnitChildProp::PostNcDestroy() 
{
	delete this;
	//CDialog::PostNcDestroy();
}


BOOL CDlgUnitChildProp::OnInitDialog() {
	CDialog::OnInitDialog();	
	DEBUG_DIALOG_LAYOUT(this);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



BOOL CDlgUnitChildProp::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	BOOL fResult = CDialog::Create(IDD, pParentWnd);
	MyWindowRect = rect;

	InitStaticControls();
	InitControlProperties();

	((CDlgUnitProp*) pParentWnd)->Subscribe(this);

	return fResult;
}

///////////////////////////////////////////////////////////////////////////////
void CDlgUnitChildProp::ShowWindow(BOOL fShow)
{	
	if (fShow == TRUE)
	{	
		MoveWindow(&MyWindowRect);
		CWnd::ShowWindow(TRUE);
		DEBUG_DIALOG_LAYOUT(this);
	}
	else
	{
		CWnd::ShowWindow(FALSE);
	}	
}


void CDlgUnitChildProp::CreateDefaultButton(void)
{
	CButton* const pBtn = &m_BtnRestoreDefaults;

	CPoint TopLeft(BUTTON_RESTORE_DEFAULTS_OFFSET_X,
				  ((MyWindowRect.bottom - MyWindowRect.top)- 
				   (MyWindowRect.bottom - MyWindowRect.top)/4)+
				   BUTTON_RESTORE_DEFAULTS_OFFSET_Y);

	CSize szControlSize(DEFAULT_BUTTON_X_SIZE, DEFAULT_BUTTON_Y_SIZE);

	if (0!=pBtn) {
		pBtn->Create("Restore Defaults", WS_CHILD | WS_VISIBLE,
					 CRect(TopLeft,szControlSize),
					 this,
					 IDC_BUTTON_DEFAULTS);	
		pBtn->SetFont(GetFont());
	}
}

void CDlgUnitChildProp::InitStaticControls(void)
{
	/////// Default + Accept buttons
	CreateDefaultButton();
}

void CDlgUnitChildProp::InitControlProperties(void)
{
	// Pure Virtual ?
}

void CDlgUnitChildProp::OnButtonDefaults(void)
{	
	RestoreDefaults();
}

BOOL CDlgUnitChildProp::ViewModeExtended() const
{
	using namespace TMGlobalSettings;
	return (TMGetDisplaySettings()->IsModeExtended());
}


////////////////////////////////////////////////////////////////////////////////
