// DlgUnitExtProp.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgUnitExtProp.h"

#include "nodedatabaseoperations.h"
#include "nodegenestructs.h"
#include "TXUnitMessage.h"

#include "utils/mfc/combobox_util.h"

#include "TMGlobals.h"
using namespace TMNodeDatabase;


// CDlgUnitExtProp dialog

//////////////////////////////////////////////////////////////////////////
CDlgUnitExtProp* CDlgUnitExtProp::m_pInstance = 0;	// Static instance
//////////////////////////////////////////////////////////////////////////


static
BOOL helper_GetAddress(
   CComboBox& refComboBox,
   BYTE* pbNodeAddress,
   BYTE* pbUnitAddress)
{
	BOOL fResult = FALSE;
	
	int selected = refComboBox.GetCurSel( );
	if (selected >= 0)
	{
		const int data = refComboBox.GetItemData(selected);
		*pbNodeAddress = (data & 0xFF00) >> 8;
		*pbUnitAddress = (data & 0xFF);

		fResult = TRUE;
	}	
	return fResult;
}

static
BOOL helper_AddToComboBox(
   CComboBox* pComboBox,
   int index,
   STRUCT_UNIT* pUnit)
{
	if (pComboBox && pUnit)
	{
		const int dataItem = (pUnit->bNodeLogicalAddress<<8) + pUnit->bUnitAddress;
	
		pComboBox->InsertString(index,pUnit->szUnitName);
		pComboBox->SetItemData(index,dataItem);		

		return TRUE;
	}
	return FALSE;
}

static
BOOL helper_GetUnitType(
	CComboBox& refComboBox,
	BYTE* pbUnitType)
{
	BOOL fResult = FALSE;
	
	int selected = refComboBox.GetCurSel( );
	if (selected >= 0)
	{
		*pbUnitType = (BYTE) refComboBox.GetItemData(selected);
		fResult = TRUE;
	}	
	return fResult;
}

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CDlgUnitExtProp, CDialog)

CDlgUnitExtProp::CDlgUnitExtProp(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgUnitExtProp::IDD, pParent)
{
	m_nID = CDlgUnitExtProp::IDD;
}

CDlgUnitExtProp::~CDlgUnitExtProp()
{
}

void CDlgUnitExtProp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_UNITTYPE, m_cmbUnitType);
	DDX_Control(pDX, IDC_COMBO_UNITNAME, m_cmbUnitName);
	DDX_Control(pDX, IDC_STATIC_ADDRESS, m_staticAddress);
	DDX_Control(pDX, IDC_STATIC_INFO, m_staticInfo);
	DDX_Control(pDX, IDC_BUTTON_REQUEST, m_btnRequest);
	DDX_Control(pDX, IDC_STATIC_GROUPBOX, m_staticGroupBox);
}

BEGIN_MESSAGE_MAP(CDlgUnitExtProp, CDialog)	
	ON_BN_CLICKED(IDC_BUTTON_REQUEST, &CDlgUnitExtProp::OnBnClickedButtonRequest)
	ON_BN_CLICKED(IDOK, &CDlgUnitExtProp::OnBnClickedOk)
	ON_CBN_SELENDOK(IDC_COMBO_UNITTYPE, &CDlgUnitExtProp::OnCbnSelendokComboUnittype)
	ON_CBN_SELENDOK(IDC_COMBO_UNITNAME, &CDlgUnitExtProp::OnCbnSelendokComboUnitName)
	ON_WM_TIMER()
END_MESSAGE_MAP()


BOOL CDlgUnitExtProp::Create()
{ 
	return CDialog::Create(m_nID,AfxGetMainWnd());
}

CDlgUnitExtProp* CDlgUnitExtProp::getInstance( )
{
	if (m_pInstance == 0) 
	{
		m_pInstance = new CDlgUnitExtProp();
		if (m_pInstance != 0)
		{
			if (m_pInstance->Create() == TRUE)
			{
				m_pInstance->ShowWindow(TRUE);
			}
			else
			{
				delete m_pInstance;
				m_pInstance = 0;
			}
		}
	}
	ASSERT( m_pInstance );
	return m_pInstance;
}

void CDlgUnitExtProp::Show(void)
{
	if (m_pInstance != 0) 
	{		
		m_pInstance->SetActiveWindow();
	}
}
bool CDlgUnitExtProp::isActive( )
{
	return (m_pInstance != 0 ? true : false);
}

// CDlgUnitExtProp message handlers
void CDlgUnitExtProp::OnBnClickedButtonRequest()
{
	BYTE bUnitType = 0xFF;

	if (helper_GetUnitType(m_cmbUnitType,&bUnitType))
	{
		if (bUnitType == UNIT_TYPE_SENS)
		{
			BYTE bNodeAddress = 0xFF;
			BYTE bUnitAddress = 0xFF;

			if (helper_GetAddress(m_cmbUnitName,&bNodeAddress,&bUnitAddress))
			{								  
				// Request berichten versturen.
				CTXSensUnitMessage txUnitMessage;
				txUnitMessage.RequestMeasuredValueOffset(bNodeAddress,bUnitAddress);		
				txUnitMessage.RequestNTCVersionInfo(bNodeAddress,bUnitAddress);

				// Timer starten - Bij de timer expiry wordt de GUI upgedate.
				SetTimer(WM_TIMER_UPDATE_PROPERTIES,
						 WM_TIMER_RELOAD_VALUE,
						 (TIMERPROC) NULL);

				// Bericht naar de gebruiker.
				m_staticInfo.SetWindowText("Please wait\nRequesting info...");
			}
		}
	}
}

void CDlgUnitExtProp::OnBnClickedOk()
{
	DestroyWindow( );
}

void CDlgUnitExtProp::PostNcDestroy()
{
	delete this;
	m_pInstance = NULL;
}

void CDlgUnitExtProp::InitStaticControls(void)
{
	using DUOTECNO::MFC_HELPER::CComboxbox_util;

	if (1)
	{
		// Unit types aan de combo box bijvoegen...

		// Momenteel enkel de sensor unit...
		CString str((LPCSTR)IDS_UNIT_TYPE_SENS); 
		m_cmbUnitType.InsertString(0,str);
		m_cmbUnitType.SetItemData(0,UNIT_TYPE_SENS);

		// Andere units volgen hier ...

		// Speciale code die ervoor zorgt dat de pijlen op/neer
		// zichtbaar zijn voor de combobox.
		CComboxbox_util::setComboBoxSetHeight(&m_cmbUnitType);
	}


	if (1)
	{
		m_cmbUnitType.SetCurSel( 0 );

		// Standaard de sensor units weergeven.
		if (AddUnitsToComboBox(UNIT_TYPE_SENS) > 0)
		{
			m_cmbUnitName.SetCurSel(0);

			DisplayCurrentUnitProperties( );		
		}
		else
		{
			// Indien er geen units aanwezig zijn.
			m_cmbUnitName.InsertString(0,"N/A");
			m_cmbUnitName.SetItemData(0,0xFFFF);	
		}

		// Speciale code die ervoor zorgt dat de pijlen op/neer
		// zichtbaar zijn voor de combobox.
		CComboxbox_util::setComboBoxSetHeight(&m_cmbUnitName);
	}
}

BOOL CDlgUnitExtProp::OnInitDialog()
{
	CDialog::OnInitDialog();

	InitStaticControls( );	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgUnitExtProp::OnCbnSelendokComboUnittype()
{
	BYTE bUnitType = 0xFF;
	if (helper_GetUnitType(m_cmbUnitType,&bUnitType))
	{					
		switch (bUnitType)
		{			
			case UNIT_TYPE_SENS:	
			{			
				ShowUnitPropertyControls( TRUE );

				// Alle sensor units toevoegen aan de combo box.
				if (AddUnitsToComboBox(bUnitType) > 0)
				{
					m_cmbUnitName.SetCurSel(0);
				}
				else
				{
					// Indien er geen units aanwezig zijn.
					m_cmbUnitName.InsertString(0,"N/A");
					m_cmbUnitName.SetItemData(0,0xFFFF);	
				}

			} break;

			default:
			{
				ShowUnitPropertyControls( FALSE );

			} break;
		}
	}	
	else
	{
		ShowUnitPropertyControls( FALSE );
	}
}

void CDlgUnitExtProp::OnCbnSelendokComboUnitName()
{
	DisplayCurrentUnitProperties( );	
}

int CDlgUnitExtProp::AddUnitsToComboBox(BYTE bUnitType)
{
	int count = 0;
	CPtrList units;	
	CNodeDatabaseOperations NodeDatabaseOperations(TMGetRuntimeNodes());
	
	m_cmbUnitName.ResetContent( );	// Alle vorige wissen.

	if (NodeDatabaseOperations.SubClassUnits(bUnitType,&units) > 0)
	{					
		// Add sensor units to the combo box...
		typedef TPtrListIterator<STRUCT_UNIT*> CPtrListUnit_it;
		
		for (CPtrListUnit_it iterator(units);
			 !iterator.IsDone( );
			 iterator.Next())
		{		
			if (helper_AddToComboBox(&m_cmbUnitName,
									 count,
									 iterator.CurrentItem()))
			{
				count++;
			}
		}
	}
	
	return count;
}


void CDlgUnitExtProp::DisplayUnitProperty(BYTE bNodeAddress, BYTE bUnitAddress)
{
	static STRUCT_UNIT* pUnit = 0;

	if (pUnit == 0 ||
		((pUnit !=0) &&
		  ((pUnit->bNodeLogicalAddress != bNodeAddress) || 
		  (pUnit->bUnitAddress != bUnitAddress))))
	{
		CNodeDatabaseOperations NodeDatabaseOperations(TMGetRuntimeNodes());
		if (!NodeDatabaseOperations.Find(bNodeAddress,bUnitAddress,&pUnit))
		{
			pUnit = 0;
		}
	}

	if (pUnit != 0)
	{
		if (1)
		{
			// Display node en unit address.
			CString szAddress;
			szAddress.Format(_T("Address: 0x%02X,0x%02X"),
							 bNodeAddress,
							 bUnitAddress);

			m_staticAddress.SetWindowText(szAddress);
		}

		if (1)
		{	
			CString szInfo("No Info Available");

			if (pUnit->bUnitType == UNIT_TYPE_SENS)
			{			
				CString szInfoNTCVersion;
				CString szInfoOffsetValue;

				if (pUnit->UnitSpecs.SensorUnit.sMeasuredValueOffset != 
					SENS_TEMP_OFFSETVALUE_UNSUPPORTED)
				{
					szInfoOffsetValue.Format("Measured Value offset: %.1f C",
											 (float)((float)pUnit->UnitSpecs.SensorUnit.sMeasuredValueOffset/10));
				}

				if (pUnit->UnitSpecs.SensorUnit.usNTCVersionInfo != 
					SENS_TEMP_NTCVERSION_UNSUPPORTED)
				{
					szInfoNTCVersion.Format("NTC Version: V%02X.%02X",								 
											pUnit->UnitSpecs.SensorUnit.usNTCVersionInfo/256,
											pUnit->UnitSpecs.SensorUnit.usNTCVersionInfo%256);				
				}	
				
				if (!szInfoOffsetValue.IsEmpty() || !szInfoNTCVersion.IsEmpty())
				{
					szInfo.Format("%s\n%s\n",
								 (szInfoOffsetValue.IsEmpty() ? "" : szInfoOffsetValue),
								 (szInfoNTCVersion.IsEmpty() ? "" : szInfoNTCVersion));
				}
			}

			m_staticInfo.SetWindowText(szInfo);			
		}	
	}
}

void CDlgUnitExtProp::DisplayCurrentUnitProperties( )
{
	// De properties tonen van de huidig geselekteerde unit.

	BYTE bNodeAddress = 0xFF;
	BYTE bUnitAddress = 0xFF;

	if (helper_GetAddress(m_cmbUnitName,&bNodeAddress,&bUnitAddress))
	{	
		DisplayUnitProperty(bNodeAddress, bUnitAddress);
	}
	else
	{
		m_staticInfo.SetWindowText("No Info Available");
	}
}

void CDlgUnitExtProp::OnTimer(UINT_PTR nIDEvent)
{
	// In de timer Expiry wordt de unit properties upgedate.
	// De timer wordt gestart nadat de requests werden verstuurd.

	if (nIDEvent == WM_TIMER_UPDATE_PROPERTIES)
	{
		KillTimer(WM_TIMER_UPDATE_PROPERTIES);

		DisplayCurrentUnitProperties( );
	}
	CDialog::OnTimer(nIDEvent);
}

void CDlgUnitExtProp::ShowUnitPropertyControls(BOOL fShow)
{
	m_cmbUnitName.ShowWindow(fShow);
	m_staticAddress.ShowWindow(fShow);
	m_staticInfo.ShowWindow(fShow);
	m_btnRequest.ShowWindow(fShow);
	m_staticGroupBox.ShowWindow(fShow);
}
