// DlgDimmerChildProp.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgDimmerChildProp.h"

#include "app\appbindings\appbindingunit\bindingunitdim_def.h"
#include "App\Appbindings\AppBindingConv2Nodes\BindingConversionToNodesProperty.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgDimmerChildProp dialog

CDlgDimmerChildProp::CDlgDimmerChildProp(CWnd* pParent /*=NULL*/)
	: CDlgUnitChildProp(pParent),
	m_bindingDimUnit(BINDING_UNIT_TYPE_DIM,0xFF,0xFF),		// TODO 
	m_fChangesAddressCfg(FALSE)
{
	//{{AFX_DATA_INIT(CDlgDimmerChildProp)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgDimmerChildProp::DoDataExchange(CDataExchange* pDX)
{
	CDlgUnitChildProp::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgDimmerChildProp)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgDimmerChildProp, CDlgUnitChildProp)
	//{{AFX_MSG_MAP(CDlgDimmerChildProp)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CHECK_SOLID_STATE, OnCheckSolidState)
	ON_BN_CLICKED(IDC_CHECK_PIRON_DELAYED, OnCheckPirOnDelayed)
	ON_BN_CLICKED(IDC_CHECK_PIROFF_DELAYED, OnCheckPirOffDelayed)
	ON_NOTIFY(UDN_DELTAPOS, IDC_UPDOWN_SPEEDUP, OnSpinSettingsChanged)
	ON_NOTIFY(UDN_DELTAPOS, IDC_UPDOWN_SPEEDDOWN, OnSpinSettingsChanged)
	ON_NOTIFY(UDN_DELTAPOS, IDC_UPDOWN_MINRANGE, OnSpinSettingsChanged)
	ON_NOTIFY(UDN_DELTAPOS, IDC_UPDOWN_MAXRANGE, OnSpinSettingsChanged)
	ON_CBN_SELENDOK(IDC_CMB_DIM_ADDRESSCFG, OnSelectAddressCfg)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

CDlgDimmerChildProp::Mode_t 
CDlgDimmerChildProp::getAddressCfgMode( ) const
{
	Mode_t mode=MODE_NORMAL;
	const unsigned short usSupportedFlags = m_bindingDimUnit.GetSupportedFlags();

	if (usSupportedFlags&CBindingPropertyDimUnit::DIM_SUPPORT_FLAGS_DALI_DEVICE)
	{
		mode=MODE_DALI_DEVICE;
	}
	else if (usSupportedFlags&CBindingPropertyDimUnit::DIM_SUPPORT_FLAGS_DALI_GROUP)
	{
		mode=MODE_DALI_GROUP;
	}
	return mode;
}

void CDlgDimmerChildProp::OnNotifyClose(void)
{
	if ((m_fChangesMade==TRUE)||(m_fChangesAddressCfg==TRUE))
	{
		CString szMessage;
		szMessage.Format(_T("Store Changes of Dimmer Unit 0x%02X,0x%02X ?"),bNodeAddress,bUnitAddress);

		if (AfxMessageBox(szMessage,MB_YESNO) == IDYES) 
		{	
			// Get Info of Controls ...
			CString szText;
			BOOL fMakeNoDefaults = FALSE;

			m_EditBuddySpeedUp.GetWindowText(szText);
			m_bindingDimUnit.set_DimSpeedUp(atoi((LPCTSTR) szText));	// CHANGES_MVS_2008_CSTRING

			m_EditBuddySpeedDown.GetWindowText(szText);
			m_bindingDimUnit.set_DimSpeedDown(atoi((LPCTSTR) szText));	// CHANGES_MVS_2008_CSTRING

			m_EditBuddyMinRange.GetWindowText(szText);
			m_bindingDimUnit.set_DimMinRange(atoi((LPCTSTR) szText));	// CHANGES_MVS_2008_CSTRING

			m_EditBuddyMaxRange.GetWindowText(szText);
			m_bindingDimUnit.set_DimMaxRange(atoi((LPCTSTR) szText));	// CHANGES_MVS_2008_CSTRING
					

			// Sedert versie 0x0B00:
			// Als enkel de bindings voor de addressCfg aangepast werden dan mogen er geen
			// P-bindings aangemaakt worden voor de andere properties.
			if ((m_fChangesAddressCfg==TRUE)&&(m_fChangesMade==FALSE))
			{
				fMakeNoDefaults = TRUE;	
			}

			// Construct property Bindings ...		
			*ptrOriginalDimUnit = m_bindingDimUnit;
			CBindingConversionToNodesProperty ConvertBindingToNodes(ptrOriginalDimUnit);
			ConvertBindingToNodes.ProcessPropertyBindings(fMakeNoDefaults);
		}
	}
}

void CDlgDimmerChildProp::InitControls(CBindingPropertyUnit *ptr)
{
	ASSERT(ptr);
	ptrOriginalDimUnit = (CBindingPropertyDimUnit*)ptr;
	m_bindingDimUnit = *ptrOriginalDimUnit;

	ASSERT(m_bindingDimUnit.GetNodeAddress() != 0xFF);
	ASSERT(m_bindingDimUnit.GetUnitAddress() != 0xFF);

	m_fChangesMade = FALSE;
	m_fChangesAddressCfg = FALSE;

	bNodeAddress = ptr->GetNodeAddress();
	bUnitAddress = ptr->GetUnitAddress();

	//Update Controls ...
	m_SpinMaxRange.SetPos(m_bindingDimUnit.get_DimMaxRange());
	m_SpinMinRange.SetPos(m_bindingDimUnit.get_DimMinRange());
	m_SpinSpeedDown.SetPos(m_bindingDimUnit.get_DimSpeedDown());
	m_SpinSpeedUp.SetPos(m_bindingDimUnit.get_DimSpeedUp());


	controlsAddrCfgInit();

	UpdateControls();
}

/////////////////////////////////////////////////////////////////////////////
// CDlgDimmerChildProp message handlers

void CDlgDimmerChildProp::OnSpinSettingsChanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_fChangesMade = TRUE;	
}
/////////////////////////////////////////////////////////////////////////////

void CDlgDimmerChildProp::InitStaticControls()
{	
	CPoint TopLeft;
	CSize szControlSize;
	CRect rcControl;

	CDlgUnitChildProp::InitStaticControls();

	{
		controlsAddrCfgCreate( );
	}

	// Speed Up : Spinctrl + edit Ctrl
	TopLeft.x = DIMMER_TOPLEFT_SPEEDUP_X;
	TopLeft.y = DIMMER_TOPLEFT_SPEEDUP_Y;
	CreateSpin(&m_EditBuddySpeedUp,IDC_BUDDY_EDIT_SPEEDUP,
			   &m_SpinSpeedUp,IDC_UPDOWN_SPEEDUP,
			   &m_StaticSpeedUp,TopLeft,"Speed Up");
		
	// Speed down 
	TopLeft.x = DIMMER_TOPLEFT_SPEEDDOWN_X;
	TopLeft.y = DIMMER_TOPLEFT_SPEEDDOWN_Y;
	CreateSpin(&m_EditBuddySpeedDown,IDC_BUDDY_EDIT_SPEEDDOWN,
			   &m_SpinSpeedDown,IDC_UPDOWN_SPEEDDOWN,
			   &m_StaticSpeedDown,TopLeft,"Speed Down");
	
	// Min Range
	TopLeft.x = DIMMER_TOPLEFT_MINRANGE_X;
	TopLeft.y = DIMMER_TOPLEFT_MINRANGE_Y;
	CreateSpin(&m_EditBuddyMinRange,IDC_BUDDY_EDIT_MINRANGE,
			   &m_SpinMinRange,IDC_UPDOWN_MINRANGE,
			   &m_StaticMinRange,TopLeft,"Min Value");

	// Max Range
	TopLeft.x = DIMMER_TOPLEFT_MAXRANGE_X;
	TopLeft.y = DIMMER_TOPLEFT_MAXRANGE_Y;
	CreateSpin(&m_EditBuddyMaxRange,IDC_BUDDY_EDIT_MAXRANGE,
			   &m_SpinMaxRange,IDC_UPDOWN_MAXRANGE,
			   &m_StaticMaxRange,TopLeft,"Max Value");

	////// CHECK BUTTONS

	TopLeft.x = DIMMER_TOPLEFT_SOLIDSTATE_ON_X;
	TopLeft.y = DIMMER_TOPLEFT_SOLIDSTATE_ON_Y;

	CreateCheckBtn(&m_CheckBtnSolidState,IDC_CHECK_SOLID_STATE,
				   "Solid State",TopLeft);

	TopLeft.y += DIMMER_CHECK_BUTTON_Y_SPACE;
	CreateCheckBtn(&m_CheckBtnPirOn,IDC_CHECK_PIRON_DELAYED,
				   "Delayed On",TopLeft);
	
	TopLeft.y += DIMMER_CHECK_BUTTON_Y_SPACE;
	CreateCheckBtn(&m_CheckBtnPirOff,IDC_CHECK_PIROFF_DELAYED,
				   "(Pir) Delayed Off",TopLeft);

}

void CDlgDimmerChildProp::CreateSpin(CEdit *pEdit, UINT EditIDD, CSpinButtonCtrl *pSpin, UINT SpinIDD, CStatic *pStatic, CPoint TopLeft, CString szText)
{
	CSize szControlSize;

	szControlSize.cx = DIMMER_EDIT_BUDDY_X_SIZE;
	szControlSize.cy = DIMMER_EDIT_BUDDY_Y_SIZE;

	pEdit->Create(WS_CHILD |WS_VISIBLE | WS_BORDER | 
							  ES_CENTER ,
							  CRect(TopLeft,szControlSize),						
							  this,
							  EditIDD);
	
	pSpin->Create(WS_CHILD | WS_VISIBLE | WS_BORDER |		               
						 UDS_ALIGNLEFT |UDS_SETBUDDYINT | UDS_ARROWKEYS,
						 CRect(0,0,0,0),
						 this,
						 SpinIDD);

	TopLeft.y -= DIMMER_STATIC_Y_SIZE;
	szControlSize.cx = DIMMER_STATIC_X_SIZE;
	szControlSize.cy = DIMMER_STATIC_Y_SIZE;
	
	pStatic->Create(szText,
				    WS_CHILD | WS_VISIBLE | SS_LEFT,
					CRect(TopLeft,szControlSize),
					this);

	pSpin->SetBuddy(pEdit);

	pEdit->SetFont(GetFont());
	pStatic->SetFont(GetFont());
	pSpin->SetFont(GetFont());
}

void CDlgDimmerChildProp::CreateCheckBtn(CButton *pBtn,UINT ButtonIDD,CString szText,CPoint TopLeft)
{
	CSize szControlSize(DIMMER_CHECK_BUTTON_X_SIZE,DIMMER_CHECK_BUTTON_Y_SIZE);


	pBtn->Create(szText,
				 WS_CHILD | WS_VISIBLE | BS_CHECKBOX ,
				 CRect(TopLeft,szControlSize),
				 this,
				 ButtonIDD);
	
	pBtn->SetFont(GetFont());
}

void CDlgDimmerChildProp::InitControlProperties()
{
	m_SpinSpeedUp.SetRange(DIM_SPEED_MINVALUE,DIM_SPEED_MAXVALUE);
	m_SpinSpeedDown.SetRange(DIM_SPEED_MINVALUE,DIM_SPEED_MAXVALUE);
	m_SpinMinRange.SetRange(DIM_RANGE_MINVALUE,DIM_RANGE_MAXVALUE);
	m_SpinMaxRange.SetRange(DIM_RANGE_MINVALUE,DIM_RANGE_MAXVALUE);
}

void CDlgDimmerChildProp::OnCheckPirOnDelayed(void)
{
	m_bindingDimUnit.set_DelayedOn(m_CheckBtnPirOn.GetCheck()^TRUE);

	m_fChangesMade = TRUE;
	UpdateControls();
}

void CDlgDimmerChildProp::OnCheckPirOffDelayed(void)
{
	m_bindingDimUnit.set_DelayedOff(m_CheckBtnPirOff.GetCheck()^TRUE);

	m_fChangesMade = TRUE;
	UpdateControls();

}

void CDlgDimmerChildProp::OnCheckSolidState(void)
{
	m_bindingDimUnit.set_SolidState(m_CheckBtnSolidState.GetCheck()^TRUE);

	m_fChangesMade = TRUE;
	UpdateControls();
}

///////////////////////////////////////////////////////////////////////////
/*
void CDlgDimmerChildProp::AcceptSettings(void)
{
	MessageBox("Accept Dimmer Prop - Settings");
}
*/

void CDlgDimmerChildProp::RestoreDefaults(void)
{
	InitControlsToDefaults();

	UpdateControls();
	m_fChangesMade = TRUE;
}

/////////////////////////////////////////////////////////////////////////

void CDlgDimmerChildProp::InitControlsToDefaults(void)
{
	m_bindingDimUnit.SetToDefaults();

	m_SpinMaxRange.SetPos(m_bindingDimUnit.get_DimMaxRange());
	m_SpinMinRange.SetPos(m_bindingDimUnit.get_DimMinRange());
	m_SpinSpeedDown.SetPos(m_bindingDimUnit.get_DimSpeedDown());
	m_SpinSpeedUp.SetPos(m_bindingDimUnit.get_DimSpeedUp());
}

void CDlgDimmerChildProp::UpdateControls(void)
{
	const BOOL fShow = (m_bindingDimUnit.get_SolidState()^TRUE);	

	m_CheckBtnSolidState.SetCheck(m_bindingDimUnit.get_SolidState());
	m_CheckBtnPirOff.SetCheck(m_bindingDimUnit.get_DelayedOff());
	m_CheckBtnPirOn.SetCheck(m_bindingDimUnit.get_DelayedOn());

	m_CheckBtnPirOff.ShowWindow(fShow);
	m_CheckBtnPirOn.ShowWindow(fShow);
		
	m_EditBuddySpeedUp.ShowWindow(fShow);
	m_EditBuddySpeedDown.ShowWindow(fShow);
	m_EditBuddyMinRange.ShowWindow(fShow);
	m_EditBuddyMaxRange.ShowWindow(fShow);

	m_SpinSpeedUp.ShowWindow(fShow);
	m_SpinSpeedDown.ShowWindow(fShow);
	m_SpinMinRange.ShowWindow(fShow);
	m_SpinMaxRange.ShowWindow(fShow);

	m_StaticSpeedUp.ShowWindow(fShow);
	m_StaticSpeedDown.ShowWindow(fShow);
	m_StaticMinRange.ShowWindow(fShow);
	m_StaticMaxRange.ShowWindow(fShow);

	{
		// Address Cfg controls update.
		const CDlgDimmerChildProp::Mode_t mode= getAddressCfgMode( );
		controlsAddrCfgUpdate(mode);
	}
}


// Sedert versie 0x0B00
void CDlgDimmerChildProp::controlsAddrCfgCreate(void)
{
	// aanmaken van de controls.

	const DWORD dwStyleStatic = WS_CHILD;
	const CRect rectStatic(CPoint(DIMMER_TOPLEFT_ADDRESSCFG_X,DIMMER_TOPLEFT_ADDRESSCFG_Y),
						   CSize(DIMMER_SIZE_ADDRESSCFG_X_SIZE,DIMMER_SIZE_ADDRESSCFG_Y_SIZE));
	
	const DWORD dwStyleComboBox = /*WS_VISIBLE |*/ WS_CHILD | CBS_DROPDOWN | WS_VSCROLL;
	const CRect rectCombo(CPoint(DIMMER_TOPLEFT_CMB_ADDRESSCFG_X,DIMMER_TOPLEFT_CMB_ADDRESSCFG_Y),
						  CSize(DIMMER_SIZE_ADDRESSCFG_CMB_X_SIZE,DIMMER_SIZE_ADDRESSCFG_CMB_Y_SIZE)); 


	m_CmbAddrCfgType.Create(
		dwStyleComboBox,
		rectCombo,
		this,
		IDC_CMB_DIM_ADDRESSCFG);

	m_StaticAddrCfgType.Create(
		"",
		dwStyleStatic,
		rectStatic,
		this);

	CFont* const pFont=GetFont();
	if (0!=pFont)
	{
		m_CmbAddrCfgType.SetFont(pFont);
		m_StaticAddrCfgType.SetFont(pFont);
	}
}

// Weergeven van het type DALI.
// - DALI device
// - DALI group.
void CDlgDimmerChildProp::controlsAddrCfgInit(void)
{
	const unsigned short usSupportedFlags=m_bindingDimUnit.GetSupportedFlags();
	BOOL fEnableWindow=FALSE;
	CString szStringStatic;

	if (usSupportedFlags&CBindingPropertyDimUnit::DIM_SUPPORT_FLAGS_DALI_DEVICE)
	{
		BYTE bUnitAddress = m_bindingDimUnit.GetUnitAddress( );

		szStringStatic.Format("DALI slave address=%d",bUnitAddress);
	}
	else if (usSupportedFlags&CBindingPropertyDimUnit::DIM_SUPPORT_FLAGS_DALI_GROUP)
	{
	
		int i;
		int t;

		t = m_CmbAddrCfgType.AddString(_T("None"));
		m_CmbAddrCfgType.SetItemData(t,0xFF);	

		for(i=0;i<64;++i)
		{
			CString s;

			s.Format(_T("DALI device %d"),i);
			t = m_CmbAddrCfgType.AddString(s);
			m_CmbAddrCfgType.SetItemData(t,i);
		}
		
		m_CmbAddrCfgType.ShowWindow(TRUE);

		if (bUnitAddress>=64)
		{
			szStringStatic.Format("DALI group address=%d",(bUnitAddress-64));
		}
	}

	fEnableWindow = szStringStatic.IsEmpty()==TRUE ? FALSE : TRUE;

	if(TRUE==fEnableWindow)
	{
		m_StaticAddrCfgType.ShowWindow(fEnableWindow);
		m_StaticAddrCfgType.SetWindowText(szStringStatic);
	}
}

void CDlgDimmerChildProp::controlsAddrCfgUpdate(Mode_t mode)
{
	switch(mode)
	{
		case MODE_NORMAL:
		{
			// Absorbe ...

		} break; /* MODE_NORMAL */
 
		case MODE_DALI_GROUP:
		{

			int itemData = 0xFF;
			const BYTE bAddressMSB = m_bindingDimUnit.get_AddressCfgMSB();
			const BYTE bAddressLSB = m_bindingDimUnit.get_AddressCfgLSB();
			const int count=m_CmbAddrCfgType.GetCount();

			for (int i=0;i<count;++i)
			{
				if (m_CmbAddrCfgType.GetItemData(i)==bAddressLSB)		// UnitAddress
				{
					m_CmbAddrCfgType.SetCurSel(i);
					break;
				}
			}

		} break; /* MODE_DALI_GROUP */

		case MODE_DALI_DEVICE:
		{
			// Absorbe ...

		} break; /* MODE_DALI_DEVICE */

	} /* switch(mode) */
}


void CDlgDimmerChildProp::OnSelectAddressCfg(void)
{
	int sel = m_CmbAddrCfgType.GetCurSel();

	if (sel>=0)
	{
		const int itemData = m_CmbAddrCfgType.GetItemData(sel);	// UnitAddress
		int type=CBindingPropertyDimUnit::ADDRESS_CFG_TYPE_NONE;
		BYTE bMSB=0xFF;
		BYTE bLSB=0xFF;

		if ((itemData>=0)&&(itemData<64))
		{		
			type=CBindingPropertyDimUnit::ADDRESS_CFG_TYPE_DALI_GROUP;

			bMSB=0;
			bLSB=itemData;	// UnitAddress
		}

		m_bindingDimUnit.set_AddressCfgType(type);
		m_bindingDimUnit.set_AddressCfgMSB(bMSB);
		m_bindingDimUnit.set_AddressCfgLSB(bLSB);

		m_fChangesAddressCfg = TRUE;		// Oppassen: ipv m_fChangesMade
	}
}
