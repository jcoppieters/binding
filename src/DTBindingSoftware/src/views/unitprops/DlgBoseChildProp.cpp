// DlgBoseChildProp.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgBoseChildProp.h"

#include "bindingconstants.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgBoseChildProp dialog


CDlgBoseChildProp::CDlgBoseChildProp(CWnd* pParent /*=NULL*/)
	: CDlgUnitChildProp(pParent),
	  m_bindingPropertyBoseRSUnit(BINDING_UNIT_TYPE_BOSERS, 0xFF, 0xFF)
{
	//{{AFX_DATA_INIT(CDlgBoseChildProp)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgBoseChildProp::DoDataExchange(CDataExchange* pDX)
{
	CDlgUnitChildProp::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgBoseChildProp)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgBoseChildProp, CDlgUnitChildProp)
	//{{AFX_MSG_MAP(CDlgBoseChildProp)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
enum
{
	ENTRY_BOSE_SYSTEM_LF50			= 0,
	ENTRY_BOSE_SYSTEM_LF48			= 1,
	ENTRY_BOSE_SYSTEM_V35			= 2,
	ENTRY_BOSE_SYSTEM_UNDEFINED
};

/////////////////////////////////////////////////////////////////////////////
const char* CDlgBoseChildProp::szType[5] =
{
	{ "Bose lifestyle system 50" },			// ENTRY_BOSE_SYSTEM_LF50 = 0
	{ "Bose lifestyle system 48" },			// ENTRY_BOSE_SYSTEM_LF48 = 1
	{ "Bose V-class system" },				// ENTRY_BOSE_SYSTEM_V35 = 2
	{ "Undefined" },						// ENTRY_BOSE_SYSTEM_UNDEFINED
	{ "Undefined" }							// ENTRY_BOSE_SYSTEM_UNDEFINED
};

/////////////////////////////////////////////////////////////////////////////
// CDlgBoseChildProp message handlers



void CDlgBoseChildProp::OnNotifyClose(void)
{

}

void CDlgBoseChildProp::InitControls(CBindingPropertyUnit *ptr)
{
	// ptr bevat een tijdelijke pointer naar een CBindingPropertyBoseRsUnit instantie.
	// Moeten enkel weten welke type Bose systeem.

	if (ptr)
	{
		m_bindingPropertyBoseRSUnit.SetConfig( 
			static_cast<CBindingPropertyBoseRsUnit*>(ptr)->GetConfig());
	}

	UpdateControls();

}

void CDlgBoseChildProp::InitStaticControls(void)
{

	CDlgUnitChildProp::InitStaticControls();


	enum
	{
		BOSE_TOPLEFT_STATIC_X	= 5,
		BOSE_TOPLEFT_STATIC_Y	= 25,
		BOSE_STATIC_SIZE_X		= 200,
		BOSE_STATIC_SIZE_Y		= 15,

		IDC_STATIC_TYPE			= 100
	};

	CPoint TopLeft(BOSE_TOPLEFT_STATIC_X,BOSE_TOPLEFT_STATIC_Y);
	CSize szControlSize(BOSE_STATIC_SIZE_X,BOSE_STATIC_SIZE_Y);

	m_staticInfo.Create("",
		                WS_VISIBLE | WS_CHILD | SS_RIGHT | SS_SIMPLE,
						CRect(TopLeft,szControlSize),
						this,
						IDC_STATIC_TYPE);

	m_staticInfo.SetFont(GetFont());


	// De knop "restore to defaults" moet niet zichtbaar zijn want
	// we kunnen niets wijzigen in deze dialoog.

	CWnd* const pWnd = GetDlgItem(IDC_BUTTON_DEFAULTS);
	if (pWnd)
	{
		pWnd->ShowWindow( FALSE );
	}	
}

void CDlgBoseChildProp::InitControlProperties(void)
{

}	

void CDlgBoseChildProp::RestoreDefaults(void)
{

}

void CDlgBoseChildProp::InitControlsToDefaults(void)
{

}

void CDlgBoseChildProp::UpdateControls(void)
{
	int selection = 3;
	const BYTE bConfig = m_bindingPropertyBoseRSUnit.GetConfig( );	
	switch( bConfig )
	{
		case BOSE_LIFESTYLE_SYSTEM50:
		{
			selection = ENTRY_BOSE_SYSTEM_LF50;
						
		} break;

		case BOSE_LIFESTYLE_SYSTEM48:
		{
			selection = ENTRY_BOSE_SYSTEM_LF48;		
				
		} break;

		case BOSE_LIFESTYLE_SYSTEMV35:
		{
			selection = ENTRY_BOSE_SYSTEM_V35;
						
		} break;	
	}

	CString szText;
	szText.Format("Type = %s",szType[selection]);
	m_staticInfo.SetWindowText(szText);
}
