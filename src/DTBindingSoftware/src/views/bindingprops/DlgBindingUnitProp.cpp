// DlgBindingUnitProp.cpp : implementation file
//
// TM_CHANGES_ALARM_ARITECH:
// Ifv het binding unit type wordt een andere property dialoog getoond.

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgBindingUnitProp.h"

#include "MyWindowMessages.h"
#include "MainFrm.h"

#include "factoryDlgBindingUnit.h"

// --------------------------------
#include "TMGlobals.h"
using namespace TMGraphics;
// ---------------------------------

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
CDlgBindingUnitProp* CDlgBindingUnitProp::m_pBindingDlgUnit = NULL;
CBindingUnit *CDlgBindingUnitProp::m_pBindingUnit = NULL;
/////////////////////////////////////////////////////////////////////////////
// CDlgBindingUnitProp dialog

CDlgBindingUnitProp::CDlgBindingUnitProp(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgBindingUnitProp::IDD, pParent) {
	//{{AFX_DATA_INIT(CDlgBindingUnitProp)
	m_szAddress = _T("");
	m_szUnitName = _T("");
	m_labelTextInfo = _T("");
	//}}AFX_DATA_INIT
	//m_pParent = pParent;
	m_nID = CDlgBindingUnitProp::IDD;
	m_pBindingUnit = NULL;
}


void CDlgBindingUnitProp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgBindingUnitProp)
	DDX_Control(pDX, IDC_BUTTON_CONTROL, m_BtnControl);
	DDX_Control(pDX, IDC_SPIN4, m_Spin4);
	DDX_Control(pDX, IDC_SPIN3, m_Spin3);
	DDX_Control(pDX, IDC_SPIN2, m_Spin2);
	DDX_Control(pDX, IDC_SPIN1, m_Spin1);
	DDX_Control(pDX, IDC_EDIT_SPIN4, m_EditSpin4);
	DDX_Control(pDX, IDC_EDIT_SPIN3, m_EditSpin3);
	DDX_Control(pDX, IDC_EDIT_SPIN2, m_EditSpin2);
	DDX_Control(pDX, IDC_EDIT_SPIN1, m_EditSpin1);
	DDX_Control(pDX, IDC_COMBO_METHOD, m_CmbMethod);
	DDX_Text(pDX, IDC_STATIC_ADDRESS, m_szAddress);
	DDX_Text(pDX, IDC_STATIC_UNITNAME, m_szUnitName);
	DDX_Text(pDX, IDC_STATIC_LABELINFO, m_labelTextInfo);	
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgBindingUnitProp, CDialog)
	//{{AFX_MSG_MAP(CDlgBindingUnitProp)
	ON_CBN_SELENDOK(IDC_COMBO_METHOD, OnSelendokComboMethod)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN1, OnDeltaposSpin1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN2, OnDeltaposSpin2)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN3, OnDeltaposSpin3)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN4, OnDeltaposSpin4)
	//}}AFX_MSG_MAP
	//ON_MESSAGE(WM_UNIT_STATUS_CHANGED,OnUnitStatusChanged)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgBindingUnitProp message handlers


// Sedert versie 0x0B01: CFactoryDlgBindingUnit gebruiken.
BOOL CDlgBindingUnitProp::Show(CBindingUnit *pBindingUnit,CPoint TopLeft,const CString& strLabelInfo)
{
	ASSERT(m_pBindingDlgUnit == NULL);

	m_pBindingDlgUnit=CFactoryDlgBindingUnit::Create(pBindingUnit);
	if (0!=m_pBindingDlgUnit)
	{		
		if (m_pBindingDlgUnit->Create() == TRUE) 
		{
			m_pBindingDlgUnit->ShowWindow(TRUE);
			
			// Calculate positions ...
			CRect rect;	
			m_pBindingDlgUnit->GetClientRect(&rect);		
			if (TopLeft.y < rect.Height()) 
			{
				TopLeft.y = (rect.Height())/2 + 10;
			}
		
			// Move window to calcualted pos + InitControls for the dialog ...
			m_pBindingDlgUnit->MoveWindow(TopLeft);		
			m_pBindingDlgUnit->InitControls(pBindingUnit,strLabelInfo);
		}
	}
	return TRUE;
}


// ERROR !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
BOOL CDlgBindingUnitProp::Destroy()
{
	if (m_pBindingDlgUnit != NULL) {
		m_pBindingDlgUnit->DestroyWindow();
		m_pBindingDlgUnit = NULL;	
	}
	ASSERT(m_pBindingDlgUnit == NULL);
	return TRUE;
}

BOOL CDlgBindingUnitProp::OnUnitStatusChanged(BYTE bNodeAddress,BYTE bUnitAddress)
{
	if (m_pBindingUnit == NULL) return FALSE;
	if (m_pBindingDlgUnit == NULL) return FALSE;
	
	if (m_pBindingUnit->GetNodeAddress() == bNodeAddress &&
		m_pBindingUnit->GetUnitAddress() == bUnitAddress) 
	{
		m_pBindingDlgUnit->UpdateRuntimeProperties();
	}
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
void CDlgBindingUnitProp::PostNcDestroy() {
	delete this;
	m_pBindingDlgUnit=0;
}

void CDlgBindingUnitProp::MoveWindow(CPoint topleft)
{
	CRect rcMainWnd,rcWindow;

	AfxGetMainWnd()->GetWindowRect(&rcMainWnd);
	
	GetWindowRect(&rcWindow);

	const int xsize = rcWindow.Width();
	const int ysize = rcWindow.Height();	

	rcWindow.top = topleft.y + rcMainWnd.top;
	rcWindow.left = topleft.x + rcMainWnd.left;
	rcWindow.bottom = topleft.y + ysize + rcMainWnd.top;
	rcWindow.right = topleft.x + xsize + rcMainWnd.left;

	CWnd::MoveWindow(rcWindow);
}

void CDlgBindingUnitProp::UpdateRuntimeProperties(void) 
{
	INVARIANT(NULL != this->m_pBindingUnit);

	CApplicationGuiUnit::AppUiIconsExtraInfo_t appUiExtraInfo = CApplicationGuiUnit::APPUI_ICONS_INFO_NONE;
	if (m_pBindingUnit->GetApplicationFlags() & CBindingUnit::APPFLAG_DALI_BINDING_UNIT) {
		appUiExtraInfo = CApplicationGuiUnit::APPUI_ICONS_INFO_DALI;
	}
	UnitIconType iImage;
	TMGetGuiUnit()->GetIcon(m_pBindingUnit->GetNodeAddress(), m_pBindingUnit->GetUnitAddress(), appUiExtraInfo, &iImage);			
	const HICON hIcon = TMGetGraphics()->UnitIcons[iImage];
	ASSERT(NULL != hIcon);
	m_BtnControl.SetIcon(hIcon);
}

void CDlgBindingUnitProp::InitControls(CBindingUnit *pUnit,const CString& strLabelInfo)
{
	m_pBindingUnit = pUnit;
	ASSERT(m_pBindingUnit);

	const BYTE bNodeAddress = m_pBindingUnit->GetNodeAddress();
	const BYTE bUnitAddress = m_pBindingUnit->GetUnitAddress();

	m_BtnControl.Init(bNodeAddress,bUnitAddress);

	m_szUnitName.Format(_T("Name: %s"),m_pBindingUnit->GetName());		
	m_szAddress.Format(_T("Address: 0x%02x,0x%02x"),bNodeAddress,bUnitAddress);

	if (strLabelInfo. IsEmpty()) {
		m_labelTextInfo = _T("");
	}
	else {
		m_labelTextInfo.Format(_T("Info: %s"),strLabelInfo);
	}
	UpdateData(FALSE);
	InitControls();
	UpdateRuntimeProperties();
}


BOOL CDlgBindingUnitProp::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_DELETE ) 
	{	
		const BYTE bNodeAddress = m_pBindingUnit->GetNodeAddress();
		const BYTE bUnitAddress = m_pBindingUnit->GetUnitAddress();

		unsigned long wParam = (bNodeAddress << 8) | bUnitAddress;

		AfxGetMainWnd()->SendMessage(WM_ADDFLOW_REMOVE_CONTROL,wParam);
		AfxGetMainWnd()->SendMessage(WM_DESTROY_DIALOG_BINDINGUNIT);

		return TRUE;	// Coz Window is destroyed !!!!
	}	
	return CDialog::PreTranslateMessage(pMsg);
}

//////////////////////////////////////////////////////////////////////////////
void CDlgBindingUnitProp::OnSelendokComboMethod() 
{
	OnCmbBoxDataChanged();
	UpdateControls();
}
void CDlgBindingUnitProp::OnDeltaposSpin1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* const pNMUpDown = (NM_UPDOWN*)pNMHDR;	
	if (NULL != pNMUpDown) {
		const int iStep = ((pNMUpDown->iDelta)*(-1));
		OnSpin1DataChanged(iStep);
	}
	UpdateControls();
	*pResult = 0;
}

void CDlgBindingUnitProp::OnDeltaposSpin2(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* const pNMUpDown = (NM_UPDOWN*)pNMHDR;
	if (NULL != pNMUpDown) {
		const int iStep = ((pNMUpDown->iDelta)*(-1));
		OnSpin2DataChanged(iStep);
	}
	UpdateControls();
	*pResult = 0;
}

void CDlgBindingUnitProp::OnDeltaposSpin3(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* const pNMUpDown = (NM_UPDOWN*)pNMHDR;
	if (NULL != pNMUpDown) {
		const int iStep = ((pNMUpDown->iDelta)*(-1));
		OnSpin3DataChanged(iStep);
	}
	UpdateControls();
	*pResult = 0;
}

void CDlgBindingUnitProp::OnDeltaposSpin4(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* const pNMUpDown = (NM_UPDOWN*)pNMHDR;
	if (NULL != pNMUpDown) {
		const int iStep = ((pNMUpDown->iDelta)*(-1));
		OnSpin4DataChanged(iStep);
	}
	UpdateControls();
	*pResult = 0;
}

//////////////////////////////////////////////////////////////////////////////

void CDlgBindingUnitProp::EnableControls(BOOL fSpin1,BOOL fSpin2,BOOL fSpin3,BOOL fSpin4)
{
	EnableSpinCtrl(&m_EditSpin1,&m_Spin1,fSpin1);
	EnableSpinCtrl(&m_EditSpin2,&m_Spin2,fSpin2);
	EnableSpinCtrl(&m_EditSpin3,&m_Spin3,fSpin3);
	EnableSpinCtrl(&m_EditSpin4,&m_Spin4,fSpin4);
}

void CDlgBindingUnitProp::EnableSpinCtrl(CMyEditCtrl* pEditCtrl,CSpinButtonCtrl* pSpin,BOOL enable) {
	pEditCtrl->ShowWindow(enable);
	pSpin->ShowWindow(enable);
}

//////////////////////////////////////////////////////////////////////////////

BOOL CDlgBindingUnitProp::ViewModeExtended() const
{
	using namespace TMGlobalSettings;
	return (TMGetDisplaySettings()->IsModeExtended());
}
	 
BOOL CDlgBindingUnitProp::ViewModeNormal() const
{
	using namespace TMGlobalSettings;
	return (TMGetDisplaySettings()->IsModeNormal());
}
	 

