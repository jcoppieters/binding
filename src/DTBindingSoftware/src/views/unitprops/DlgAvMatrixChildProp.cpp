// DlgAvMatrixChildProp.cpp : implementation file

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgAvMatrixChildProp.h"

#include "App\Appbindings\AppBindingConv2Nodes\BindingConversionToNodesProperty.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/***********************************************************/
#define DEBUG_LOCAL_ON		0
/***********************************************************/
#if (DEBUG_LOCAL_ON == 1)
	#define DEBUG_TRACE(x)		TRACE(x)
#else
	#define	DEBUG_TRACE(x)
#endif
/***********************************************************/

/////////////////////////////////////////////////////////////////////////////
namespace
{
	BOOL helper_ComboBoxSetCurSel(
		CComboBox* pComboBox,
		int dataItem)
	{
		BOOL result = FALSE;
		const int count = pComboBox->GetCount( );
		for (int i=0;i<count;i++)
		{
			if (pComboBox->GetItemData(i) == dataItem)
			{
				pComboBox->SetCurSel(i);
				result = TRUE;
				break;
			}
		}
		return result;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDlgAvMatrixChildProp dialog


CDlgAvMatrixChildProp::CDlgAvMatrixChildProp(CWnd* pParent)
	: CDlgUnitChildProp(pParent),
	  m_bindingAVMatrixUnit(BINDING_UNIT_TYPE_AVMATRIX, 0xFF, 0xFF),			// OPPASSEN: GEEN TYPE !!!!!!
	  m_ptrOriginalAVMatrixUnit(NULL)
{
	//{{AFX_DATA_INIT(CDlgAvMatrixChildProp)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CDlgAvMatrixChildProp::DoDataExchange(CDataExchange* pDX)
{
	CDlgUnitChildProp::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgAvMatrixChildProp)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP	
}


BEGIN_MESSAGE_MAP(CDlgAvMatrixChildProp, CDlgUnitChildProp)
	//{{AFX_MSG_MAP(CDlgAvMatrixChildProp)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP

	ON_CBN_SELENDOK(IDC_AVMATRIX_CONFIGTYPE, OnAVMatrixConfigType)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgAlarmChildProp message handlers

void CDlgAvMatrixChildProp::OnNotifyClose(void)
{	
	if (m_ptrOriginalAVMatrixUnit != NULL) 
	{	
		if (m_fChangesMade == TRUE) 
		{	
			CString szMessage;
			szMessage.Format(_T("Store Changes of the AVMatrix Unit 0x%02X,0x%02X ?"),bNodeAddress,bUnitAddress);

			if (AfxMessageBox(szMessage,MB_YESNO) == IDYES) 
			{				
				// Copy Info to original ptr ...											
				*m_ptrOriginalAVMatrixUnit = m_bindingAVMatrixUnit;

				// Construct property Bindings ...		
				CBindingConversionToNodesProperty ConvertBindingToNodes(m_ptrOriginalAVMatrixUnit);
				ConvertBindingToNodes.ProcessPropertyBindings();
			}
		}
	}
}

void CDlgAvMatrixChildProp::InitControls(CBindingPropertyUnit *ptr)
{
	ASSERT(ptr);
	if (ptr != NULL) 
	{
		m_ptrOriginalAVMatrixUnit = dynamic_cast<CBindingPropertyAVMatrixUnit*>(ptr);
		if (NULL != m_ptrOriginalAVMatrixUnit) {
			m_fChangesMade = FALSE;
			bNodeAddress = ptr->GetNodeAddress();
			bUnitAddress = ptr->GetUnitAddress();
			InitControlsToDefaults();	// First
			m_bindingAVMatrixUnit = *m_ptrOriginalAVMatrixUnit;	// Second !!!
			UpdateControls();
		}
		else {
			const CString szError("Error : No valid Bindingproperty unit for this AVMatrix Unit");
			AfxMessageBox(szError);
		}
	}
	else {
		const CString szError("Error : No Bindingproperty unit created found this AVMatrix Unit");
		AfxMessageBox(szError);
	}
}

void CDlgAvMatrixChildProp::InitStaticControls(void)
{
	CDlgUnitChildProp::InitStaticControls();
	CString szStatic;

	// Control static: Type 
	{		
		CPoint TopLeft(15,20);
		CSize szControlSize(150,20);
		szStatic.Format(_T("Type: "));
		m_StaticType.Create((LPCTSTR) szStatic,WS_CHILD | WS_VISIBLE | SS_LEFT,CRect(TopLeft,szControlSize),this);
	}

	// Combobox om het subtype te selecteren.
	{
		CPoint TopLeft(15,50);	
		CSize szControlSize(80,20);

		// Tekst label van de combobox.
		szStatic.Format(_T("AVMatrix Type:"));
		m_StaticSubType.Create((LPCTSTR) szStatic,WS_CHILD | WS_VISIBLE | SS_LEFT,CRect(TopLeft,szControlSize),this);

		TopLeft.y -= 2;
		TopLeft.x += 80; // 95;
		szControlSize.cx = 150;		
		szControlSize.cy = 100;

		// De Combobox.
		m_CmbSubType.Create(WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_VSCROLL ,CRect(TopLeft,szControlSize),this,IDC_AVMATRIX_CONFIGTYPE);
		m_CmbSubType.SetExtendedUI(); 

		// Virtuele functie: 
		// Iedere implementatie kan zijn eigen types invullen in de combobox.
		this->initAVMatrixSubTypeComboBox(&m_CmbSubType);

		CFont* const pFont = GetFont();
		if (pFont)
		{
			m_StaticType.SetFont(pFont);	
			m_CmbSubType.SetFont(pFont);			
			m_StaticSubType.SetFont(pFont);	
		}
	}

	this->displayAVMatrixType();
}

void CDlgAvMatrixChildProp::OnAVMatrixConfigType(void)
{
	static const BOOL UPDATE_BINDINGUNIT = FALSE;	// Moeten deze op false zetten zodat de binding aangemaakt wordt...
	m_fChangesMade = this->updateAVMatrixSubType(&m_bindingAVMatrixUnit,&m_CmbSubType,UPDATE_BINDINGUNIT);
}

void CDlgAvMatrixChildProp::InitControlProperties(void)
{
	m_bindingAVMatrixUnit.SetToDefaults();
}

void CDlgAvMatrixChildProp::RestoreDefaults(void)
{
	InitControlsToDefaults();
	UpdateControls();
	m_fChangesMade = TRUE;
}

void CDlgAvMatrixChildProp::InitControlsToDefaults(void)
{
	m_bindingAVMatrixUnit.SetToDefaults();
}

void CDlgAvMatrixChildProp::UpdateControls(void)
{
	static const BOOL UPDATE_GUI = TRUE;
	this->updateAVMatrixSubType(&m_bindingAVMatrixUnit,&m_CmbSubType,UPDATE_GUI);	
}

void CDlgAvMatrixChildProp::setNameAVMatrixType(const CString& name) 
{
	m_StaticType.SetWindowText(name);
}

void CDlgAvMatrixChildProp::showControlsMatrixType(BOOL flag)
{
	m_StaticType.ShowWindow(flag);
}

void CDlgAvMatrixChildProp::showControlsSubType(BOOL flag) 
{
	m_StaticSubType.ShowWindow(flag);
	m_CmbSubType.ShowWindow(flag);
}


// CDlgAVMatrixChildPropAudac

CDlgAVMatrixChildPropAudac::CDlgAVMatrixChildPropAudac(CWnd* pParent) :
CDlgAvMatrixChildProp(pParent) { }

void CDlgAVMatrixChildPropAudac::displayAVMatrixType( )
{
	setNameAVMatrixType("Audac AV-Matrix");
}

BOOL CDlgAVMatrixChildPropAudac::updateAVMatrixSubType(CBindingPropertyAVMatrixUnit* p,CComboBox* pComboBox,BOOL fUpdateUI)
{
	BOOL result = FALSE;
	ASSERT(p && pComboBox);
	if (fUpdateUI)
	{
		const int type = p->GetSubType( );		
		result = helper_ComboBoxSetCurSel(pComboBox,type);		
	}
	else
	{		
		const int selection = pComboBox->GetCurSel();	
		if (selection != -1)
		{	
			const int type = pComboBox->GetItemData(selection);
		
			if (p->GetSubType( ) != type) {
				p->SetSubType(type);
				result=TRUE;
			}		
		}		
	}
	return result;
}

void CDlgAVMatrixChildPropAudac::initAVMatrixSubTypeComboBox(CComboBox* pComboBox)
{
	enum {
		AVMATRIX_AUDAC_TYPE_MTX = 0,
		AVMATRIX_AUDAC_TYPE_M2 = 1
	};

	if (pComboBox)
	{
		pComboBox->InsertString(0,"Audac MTX");		
		pComboBox->SetItemData(0,AVMATRIX_AUDAC_TYPE_MTX);	

		pComboBox->InsertString(1,"Audac M2");		
		pComboBox->SetItemData(1,AVMATRIX_AUDAC_TYPE_M2);	
	}
}

void CDlgAVMatrixChildPropAudac::UpdateControls( )
{
	this->CDlgAvMatrixChildProp::UpdateControls( );

	showControlsSubType(TRUE);
	showControlsMatrixType(TRUE);
}

// CDlgAVMatrixChildPropIntegra

CDlgAVMatrixChildPropIntegra::CDlgAVMatrixChildPropIntegra(CWnd* pParent) :
CDlgAvMatrixChildProp(pParent) { }

void CDlgAVMatrixChildPropIntegra::displayAVMatrixType( )
{
	setNameAVMatrixType("Integra AV-Matrix");
}

BOOL CDlgAVMatrixChildPropIntegra::updateAVMatrixSubType(CBindingPropertyAVMatrixUnit* p,CComboBox* pComboBox,BOOL fUpdateUI)
{
	BOOL result = FALSE;
	ASSERT(p && pComboBox);
	if (fUpdateUI)
	{
		const int type = p->GetSubType( );		
		result = helper_ComboBoxSetCurSel(pComboBox,type);		
	}
	else
	{		
		const int selection = pComboBox->GetCurSel();	
		if (selection != -1)
		{	
			const int type = pComboBox->GetItemData(selection);		
			if (p->GetSubType( ) != type) {
				p->SetSubType(type);
				result=TRUE;
			}		
		}		
	}
	return result;
}

void CDlgAVMatrixChildPropIntegra::initAVMatrixSubTypeComboBox(CComboBox* pComboBox)
{
	enum {
		AVMATRIX_INTEGRA_NORMAL			= 0,			// gewone volumes.
		AVMATRIX_INTEGRA_VOLUME_SCALING = 1				// volume scaling
	};

	if (pComboBox)
	{
		pComboBox->InsertString(0,"DRX-2, DRX-3");		
		pComboBox->SetItemData(0,AVMATRIX_INTEGRA_NORMAL);	

		pComboBox->InsertString(1,"DRX-4, DRX-5, DRX-7");		
		pComboBox->SetItemData(1,AVMATRIX_INTEGRA_VOLUME_SCALING);	
	}
}

void CDlgAVMatrixChildPropIntegra::UpdateControls( )
{
	this->CDlgAvMatrixChildProp::UpdateControls( );

	showControlsSubType(TRUE);
	showControlsMatrixType(TRUE);
}


