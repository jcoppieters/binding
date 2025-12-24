// DlgAlarmChildProp.cpp : implementation file
//
// TM_CHANGES_ALARM_ARITECH 
// CDlgAlarmChildPropAritech 
// Deze klasse zorgt ervoor dat het type alarm centrale niet kan ingesteld worden.
// De remote code moet in beide dialogen kunnen ingesteld worden.
//
// TM_CHANGES_ALARM_ANB
// CDlgAlarmChildPropAnbBibus 
// Deze klasse zorgt ervoor dat het type alarm centrale en de remote code
// niet kunnen ingesteld worden.


#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgAlarmChildProp.h"

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
// CDlgAlarmChildProp dialog


CDlgAlarmChildProp::CDlgAlarmChildProp(CWnd* pParent /*=NULL*/)
	: CDlgUnitChildProp(pParent),
	  m_bindingAlarmUnit( BINDING_UNIT_TYPE_ALARM_GALAXY, 0xFF, 0xFF)
{
	//{{AFX_DATA_INIT(CDlgAlarmChildProp)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	DEBUG_TRACE("CDlgAlarmChildProp::ctor()\n");
}


void CDlgAlarmChildProp::DoDataExchange(CDataExchange* pDX)
{
	CDlgUnitChildProp::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgAlarmChildProp)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP	
}


BEGIN_MESSAGE_MAP(CDlgAlarmChildProp, CDlgUnitChildProp)
	//{{AFX_MSG_MAP(CDlgAlarmChildProp)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP

	ON_CBN_SELENDOK(IDC_ALARM_CONFIGTYPE, OnAlarmConfigType)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgAlarmChildProp message handlers

void CDlgAlarmChildProp::OnNotifyClose(void)
{
	UpdateRemoteCodeVal( );

	if (ptrOriginalAlarmUnit != NULL) 
	{	
		if (m_fChangesMade == TRUE) 
		{	
			CString szMessage;
			szMessage.Format(_T("Store Changes of Alarm Unit 0x%02X,0x%02X ?"),bNodeAddress,bUnitAddress);

			if (AfxMessageBox(szMessage,MB_YESNO) == IDYES) 
			{				
				// Copy Info to original ptr ...											
				*ptrOriginalAlarmUnit = m_bindingAlarmUnit;

				// Construct property Bindings ...		
				CBindingConversionToNodesProperty ConvertBindingToNodes(ptrOriginalAlarmUnit);
				ConvertBindingToNodes.ProcessPropertyBindings();
			}
		}
	}
}

// Protected...
BOOL CDlgAlarmChildProp::UpdateRemoteCodeVal(void)
{
	char szRemoteCode[ALARM_REMOTECODE_BUFFER_MAX];
	int i;

	if (m_EditRemoteCode.GetModify()) 
	{
		// Remote code kopieren in 
		m_EditRemoteCode.GetWindowText(szRemoteCode,ALARM_REMOTECODE_BUFFER_MAX-1);

		// Lengte van de remote code = index 0
		const int length = strlen(szRemoteCode); 
		m_bindingAlarmUnit.SetRemoteCode(0,length);
	
		// Digit 1 tem 7 invullen
		for (i=0;i<7;i++) 
		{
			if (i < length) 
				m_bindingAlarmUnit.SetRemoteCode(i+1,(szRemoteCode[i] - 0x30));
			else 
				m_bindingAlarmUnit.SetRemoteCode(i+1,0);
		}
		
		// Controle of alle digits tss 0 & 9.
		for (i=0;i<8;i++) 
		{
			if (m_bindingAlarmUnit.GetRemoteCode(i) > 9) 
			{
				break;
			}
		}

		if (i == 8) 
		{
			m_fChangesMade = TRUE;	
		}
		else 
		{		
			CString szMessage("Wrong format of Remotecode can't make the property bindings");
			AfxMessageBox(szMessage);			
			m_fChangesMade = FALSE;
		}		
	}	
	return m_fChangesMade;
}

void CDlgAlarmChildProp::InitControls(CBindingPropertyUnit *ptr)
{
	ASSERT(ptr);

	if (ptr != NULL) 
	{
		ptrOriginalAlarmUnit = (CBindingPropertyAlarmUnit*)ptr;
		m_fChangesMade = FALSE;
		bNodeAddress = ptr->GetNodeAddress();
		bUnitAddress = ptr->GetUnitAddress();

		InitControlsToDefaults();					// First
		m_bindingAlarmUnit = *ptrOriginalAlarmUnit;	// Second !!!
		UpdateControls();
	}
	else 
	{
		CString szError("Error : No Bindingproperty unit created found this Alarm Unit");
		AfxMessageBox(szError);
	}
}

void CDlgAlarmChildProp::InitStaticControls(void)
{
	CDlgUnitChildProp::InitStaticControls();
	CString szStatic;

	{		
		CPoint TopLeft(15,20);
		CSize szControlSize(150,20);
		szStatic.Format(_T("Type: "));
		m_StaticType.Create((LPCTSTR) szStatic,				// CHANGES_MVS_2008_CSTRING
								WS_CHILD | WS_VISIBLE | SS_LEFT,
								CRect(TopLeft,szControlSize),
								this);
	}

	{
		CPoint TopLeft(15,50);
		CSize szControlSize(80,20);
		szStatic.Format(_T("Remote Code :"));
		m_StaticRemoteCode.Create((LPCTSTR) szStatic,				// CHANGES_MVS_2008_CSTRING
								WS_CHILD | WS_VISIBLE | SS_LEFT,
								CRect(TopLeft,szControlSize),
								this);
		
		TopLeft.x += 95;
		szControlSize.cx = 100;
		szControlSize.cy = 20; 
		m_EditRemoteCode.Create(WS_CHILD |WS_VISIBLE | WS_BORDER | 
							  ES_LEFT ,
							  CRect(TopLeft,szControlSize),						
							  this,
							  IDC_ALARM_EDIT_REMOTECODE);		
	
		CFont* const pFont = GetFont();
		if (pFont)
		{
			m_StaticType.SetFont(pFont);				
			m_StaticRemoteCode.SetFont(pFont);
			m_EditRemoteCode.SetFont(pFont);
		}
	}

	{
		CPoint TopLeft(15,80);	
		CSize szControlSize(80,20);
		szStatic.Format(_T("Alarm Type :"));
		m_StaticConfig.Create((LPCTSTR) szStatic,						// CHANGES_MVS_2008_CSTRING
								  WS_CHILD | WS_VISIBLE | SS_LEFT,
								  CRect(TopLeft,szControlSize),
								  this);

		TopLeft.x += 95;
		szControlSize.cx = 100;
		szControlSize.cy = 100;

		m_CmbAlarmType.Create(WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_VSCROLL , 
							 CRect(TopLeft,szControlSize),
							 this,
							 IDC_ALARM_CONFIGTYPE);

		m_CmbAlarmType.SetExtendedUI(); 

		// Virtuele functie: 
		// Iedere implementatie kan zijn eigen types invullen in de combobox.
		this->initAlarmUnitTypeComboBox(&m_CmbAlarmType);

		m_CmbAlarmType.SetFont(GetFont());			
		m_StaticConfig.SetFont(GetFont());	
	}

	this->displayAlarmType( );

#if (ALARM_INTEGRA_USE_REMOTE_CODE==1)
	this->InitStaticControls_Post();
#endif

}

void CDlgAlarmChildProp::InitControlProperties(void)
{	
	m_bindingAlarmUnit.SetToDefaults();
}

void CDlgAlarmChildProp::UpdateControls(void)
{
	this->updateAlarmUnitType(&m_bindingAlarmUnit,
							  &m_CmbAlarmType,
							  TRUE);		// Updaten van de UI.

	UpdateControlsRemoteCode( );
}

void CDlgAlarmChildProp::UpdateControlsRemoteCode(void)
{
	CString szRemoteCode;

	// Index 0 bevat de lengte van de remote code.
	const int length = m_bindingAlarmUnit.GetRemoteCode(0);
	if (length < ALARM_REMOTECODE_BUFFER_MAX) 
	{
		szRemoteCode.Format("");
	
		for (int i=0;i<length;i++) 
		{
			const BYTE bTemp = m_bindingAlarmUnit.GetRemoteCode(i+1);

			// oplossing BUG-0134: '<='
			if (bTemp <= 9) 
			{
				szRemoteCode += (BYTE) (bTemp+0x30);	// CHANGES_MVS_2008
			}
			else 
			{
				szRemoteCode += '?';
			}
		}
	}
	else 
	{
		szRemoteCode.Format("Error!");
	}

	m_EditRemoteCode.SetWindowText(szRemoteCode);
}

void CDlgAlarmChildProp::RestoreDefaults(void)
{
	InitControlsToDefaults();
	UpdateControls();
	m_fChangesMade = TRUE;
}

void CDlgAlarmChildProp::InitControlsToDefaults(void)
{
	DEBUG_TRACE("CDlgAlarmChildProp::InitControlsToDefaults()\n");

	// TM20130328: Aangepast.
	m_bindingAlarmUnit.SetToDefaults( );
}

/*

  TM 20050704 : This causes a bug in the release version !!!!!!!!!!!!!!!!!!

void CDlgAlarmChildProp::OnAlarmConfigType(NMHDR* pNMHDR, LRESULT* pResult)
{
	int nSelection = m_CmbAlarmType.GetCurSel();	
	BindingAlarmUnit.bAlarmCentraleType = (nSelection & 0x01);
	m_fChangesMade = TRUE;
}
*/

void CDlgAlarmChildProp::OnAlarmConfigType(void)
{
	// TM20130328: Gebruik maken van een virtuele functie.

	m_fChangesMade = this->updateAlarmUnitType(
						&m_bindingAlarmUnit,
						&m_CmbAlarmType,
						FALSE);	// Update van de alarmUnit
}

void CDlgAlarmChildProp::setNameAlarmType(const char* s)	
{
	CString szTemp("Type: Undefined");

	if (s)
	{
		szTemp.Format("Type: %s",s);
	}	

	m_StaticType.SetWindowText(szTemp);
}

void CDlgAlarmChildProp::displayAlarmType( )	
{
	setNameAlarmType("Honeywell Galaxy");
}

BOOL CDlgAlarmChildProp::updateAlarmUnitType(
	CBindingPropertyAlarmUnit* pBindingAlarmUnit,
	CComboBox* pComboBox,
	BOOL fUpdateUI)
{
	BOOL result = FALSE;

	ASSERT(pBindingAlarmUnit && pComboBox);

	if (fUpdateUI)
	{
		const int type = pBindingAlarmUnit->GetAlarmCentraleType();
		if (type == ALARM_TYPE_CONFIG_GALAXY60 ||
			type == ALARM_TYPE_CONFIG_GALAXY500)
		{
			result = helper_ComboBoxSetCurSel(pComboBox,type);			
		}
	}
	else
	{		
		const int selection = pComboBox->GetCurSel();	
		if (selection != -1)
		{
			const int type = pComboBox->GetItemData(selection);
			if (type == ALARM_TYPE_CONFIG_GALAXY60 ||
				type == ALARM_TYPE_CONFIG_GALAXY500)
			{
				if (pBindingAlarmUnit->GetAlarmCentraleType() != type)
				{
					pBindingAlarmUnit->SetAlarmCentraleType(type);
					result = TRUE;
				}
			}
		}		
	}
	return result;
}

void CDlgAlarmChildProp::initAlarmUnitTypeComboBox(
	CComboBox* pComboBox)
{	
	if (pComboBox)
	{
		pComboBox->InsertString(0,"Galaxy 60");		
		pComboBox->SetItemData(0,ALARM_TYPE_CONFIG_GALAXY60);		// TM20130328

		pComboBox->InsertString(1,"Galaxy 500");
		pComboBox->SetItemData(1,ALARM_TYPE_CONFIG_GALAXY500);		// TM20130328	
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDlgAlarmChildPropAritech 

CDlgAlarmChildPropAritech::CDlgAlarmChildPropAritech(CWnd* pParent) :
Base( pParent )
{
	DEBUG_TRACE("CDlgAlarmChildPropAritech::ctor()\n");
}

void CDlgAlarmChildPropAritech::UpdateControls(void)
{
	this->Base::UpdateControls( );

	m_CmbAlarmType.ShowWindow( FALSE );
	m_StaticConfig.ShowWindow( FALSE );
}

void CDlgAlarmChildPropAritech::displayAlarmType( )	
{
	setNameAlarmType("Aritech ATS master");
}

BOOL CDlgAlarmChildPropAritech::updateAlarmUnitType(
	CBindingPropertyAlarmUnit* pBindingAlarmUnit,
	CComboBox* pComboBox,
	BOOL fUpdateUI)
{
	return FALSE;
}

void CDlgAlarmChildPropAritech::initAlarmUnitTypeComboBox(
	CComboBox* pComboBox)
{	
	// Absorbe 
}

/////////////////////////////////////////////////////////////////////////////
// CDlgAlarmChildPropAnbBibus : TM_CHANGES_ALARM_ANB

CDlgAlarmChildPropAnbBibus::CDlgAlarmChildPropAnbBibus(CWnd* pParent) :
Base( pParent )
{
	DEBUG_TRACE("CDlgAlarmChildPropAnbBibus::ctor()\n");
}

void CDlgAlarmChildPropAnbBibus::UpdateControls(void)
{
	this->Base::UpdateControls( );

	m_CmbAlarmType.ShowWindow( FALSE );
	m_StaticConfig.ShowWindow( FALSE );
	m_EditRemoteCode.ShowWindow( FALSE );
	m_StaticRemoteCode.ShowWindow( FALSE );
}

void CDlgAlarmChildPropAnbBibus::displayAlarmType( )	
{
	setNameAlarmType("ANB BiBus");
}

BOOL CDlgAlarmChildPropAnbBibus::updateAlarmUnitType(
	CBindingPropertyAlarmUnit* pBindingAlarmUnit,
	CComboBox* pComboBox,
	BOOL fUpdateUI)
{
	return FALSE;
}

void CDlgAlarmChildPropAnbBibus::initAlarmUnitTypeComboBox(
	CComboBox* pComboBox)
{	
	// Absorbe 
}


/////////////////////////////////////////////////////////////////////////////
// CDlgAlarmChildPropTexecom : TM_CHANGES_ALARM_TEXECOM:
// Remote code + type alarm centrale instellen.

CDlgAlarmChildPropTexecom::CDlgAlarmChildPropTexecom(CWnd* pParent) :
Base( pParent )
{
	DEBUG_TRACE("CDlgAlarmChildPropTexecom::ctor()\n");
}

void CDlgAlarmChildPropTexecom::UpdateControls(void)
{
	this->Base::UpdateControls( );

	m_CmbAlarmType.ShowWindow( TRUE );
	m_StaticConfig.ShowWindow( TRUE );
	m_EditRemoteCode.ShowWindow( TRUE );
	m_StaticRemoteCode.ShowWindow( TRUE );
}

void CDlgAlarmChildPropTexecom::displayAlarmType( )	/* TM_CHANGES_ALARM_TEXECOM */
{
	setNameAlarmType("Texecom Alarm");
}

BOOL CDlgAlarmChildPropTexecom::updateAlarmUnitType(
	CBindingPropertyAlarmUnit* pBindingAlarmUnit,
	CComboBox* pComboBox,
	BOOL fUpdateUI)
{
	BOOL result = FALSE;

	ASSERT(pBindingAlarmUnit && pComboBox);

	if (fUpdateUI)
	{
		const int type = pBindingAlarmUnit->GetAlarmCentraleType();
		if (type == ALARM_TYPE_CONFIG_TEXECOM_PREMIER48 ||
			type == ALARM_TYPE_CONFIG_TEXECOM_PREMIER88 ||
			type == ALARM_TYPE_CONFIG_TEXECOM_PREMIER168)
		{
			result = helper_ComboBoxSetCurSel(pComboBox,type);			
		}
	}
	else
	{		
		const int selection = pComboBox->GetCurSel();	
		if (selection != -1)
		{
			const int type = pComboBox->GetItemData(selection);
			if (type == ALARM_TYPE_CONFIG_TEXECOM_PREMIER48 ||
				type == ALARM_TYPE_CONFIG_TEXECOM_PREMIER88 ||
				type == ALARM_TYPE_CONFIG_TEXECOM_PREMIER168)
			{
				if (pBindingAlarmUnit->GetAlarmCentraleType() != type)
				{
					pBindingAlarmUnit->SetAlarmCentraleType(type);
					result = TRUE;
				}
			}
		}		
	}
	return result;
}


void CDlgAlarmChildPropTexecom::initAlarmUnitTypeComboBox(
	CComboBox* pComboBox)
{	
	if (pComboBox)
	{
		pComboBox->InsertString(0,"Premier 48");		
		pComboBox->SetItemData(0,ALARM_TYPE_CONFIG_TEXECOM_PREMIER48);		

		pComboBox->InsertString(1,"Premier 88");
		pComboBox->SetItemData(1,ALARM_TYPE_CONFIG_TEXECOM_PREMIER88);		

		pComboBox->InsertString(2,"Premier 168");
		pComboBox->SetItemData(2,ALARM_TYPE_CONFIG_TEXECOM_PREMIER168);	
	}
}

/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// CDlgAlarmChildPropIntegra : TM_CHANGES_ALARM_INTEGRA

CDlgAlarmChildPropIntegra::CDlgAlarmChildPropIntegra(CWnd* pParent) :
Base( pParent )
{
	DEBUG_TRACE("CDlgAlarmChildPropAnbBibus::ctor()\n");
}

void CDlgAlarmChildPropIntegra::UpdateControls(void)
{
	this->Base::UpdateControls( );

	m_CmbAlarmType.ShowWindow( FALSE );
	m_StaticConfig.ShowWindow( FALSE );

#if (ALARM_INTEGRA_USE_REMOTE_CODE==1)
	m_StaticRemoteCode.ShowWindow( TRUE );
	m_EditRemoteCode.ShowWindow( TRUE );
#else
	m_StaticRemoteCode.ShowWindow( FALSE );
	m_EditRemoteCode.ShowWindow( FALSE );
#endif

}

void CDlgAlarmChildPropIntegra::displayAlarmType( )	
{
	setNameAlarmType("Satel Integra");
}

BOOL CDlgAlarmChildPropIntegra::updateAlarmUnitType(
	CBindingPropertyAlarmUnit* pBindingAlarmUnit,
	CComboBox* pComboBox,
	BOOL fUpdateUI)
{
	return FALSE;
}

void CDlgAlarmChildPropIntegra::initAlarmUnitTypeComboBox(
	CComboBox* pComboBox)
{	
	// Absorbe 
}

#if (ALARM_INTEGRA_USE_REMOTE_CODE==1)

void CDlgAlarmChildPropIntegra::InitStaticControls_Post(void)
{
	this->m_StaticRemoteCode.SetWindowText(_T("User code:"));
}

// Protected...
// Bij de Integra alarm centrale bevat idx 0 niet de lengte maar is de
// eerste digit.
// Op deze manier kunnen er 8 digits gebruikt worden voor de remotecode.
BOOL CDlgAlarmChildPropIntegra::UpdateRemoteCodeVal(void)
{
	char szRemoteCode[ALARM_REMOTECODE_BUFFER_MAX+1];
	int i;

	if (m_EditRemoteCode.GetModify()) 
	{
		// Remote code kopieren in 
		m_EditRemoteCode.GetWindowText(szRemoteCode,ALARM_REMOTECODE_BUFFER_MAX+1);

		// Lengte van de remote code = index 0
		int length = strlen(szRemoteCode); 

		if (length > ALARM_REMOTECODE_BUFFER_MAX)
		{
			length=ALARM_REMOTECODE_BUFFER_MAX;
		}

		for (i=0;i<length;i++)
		{
			m_bindingAlarmUnit.SetRemoteCode(i,szRemoteCode[i]-0x30);
		}

		for (;i<ALARM_REMOTECODE_BUFFER_MAX;++i)
		{
			m_bindingAlarmUnit.SetRemoteCode(i,0);
		}
	
		// Controle of alle digits tss 0 & 9.
		for (i=0;i<ALARM_REMOTECODE_BUFFER_MAX;i++) 
		{
			const BYTE bVal = m_bindingAlarmUnit.GetRemoteCode(i);

			if (!(bVal>=0 && bVal<=9))
			{
				break;
			}
		}

		if (i==ALARM_REMOTECODE_BUFFER_MAX) 
		{
			m_fChangesMade = TRUE;	
		}
		else 
		{		
			CString szMessage("Wrong format of Remotecode can't make the property bindings");
			AfxMessageBox(szMessage);			
			m_fChangesMade = FALSE;
		}		
	}	
	return m_fChangesMade;
}

// Protected...
// Bij de Integra alarm centrale bevat idx 0 niet de lengte maar is de
// eerste digit.
// Op deze manier kunnen er 8 digits gebruikt worden voor de remotecode.
void CDlgAlarmChildPropIntegra::UpdateControlsRemoteCode(void)
{
	CString szRemoteCode;

	for (int i=0;i<ALARM_REMOTECODE_BUFFER_MAX;i++) 
	{
		const BYTE bTemp = m_bindingAlarmUnit.GetRemoteCode(i);

		// oplossing BUG-0134: '<='
		if (bTemp>=0 &&bTemp<=9) 
		{
			szRemoteCode+= (BYTE)(bTemp+0x30);	// CHANGES_MVS_2008
		}
		else 
		{
			szRemoteCode += '?';
		}
	}		
	m_EditRemoteCode.SetWindowText(szRemoteCode);
}
#endif