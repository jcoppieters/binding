// DlgSensPropChild.cpp : implementation file
//
// Changes: TM20120402: CBindingPropertySensUnit via Set() en Get() 
// Changes: TM20120402: CSpinButtonCtrl op een andere manier.
// Changes: TM20120402: Offset Measured Value toegevoegd.
// Changes: TM20120404: Spin Controls op een andere manier geimplementeerd.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgSensPropChild.h"
#include "nodegenestructs.h"
#include "App\Appbindings\AppBindingConv2Nodes\BindingConversionToNodesProperty.h"
#include "app\appbindings\appbindingproperty\bindingpropertyunitsens_utils.h"
#include "tmglobals.h"

#include "utils/mfc/combobox_util.h"

using namespace TMNodeDatabase;
using namespace TMBindingDatabase;


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSensPropChild dialog

CDlgSensPropChild::CDlgSensPropChild(CWnd* pParent /*=NULL*/)
	: CDlgUnitChildProp(pParent),
	m_bindingSensUnit(BINDING_UNIT_TYPE_SENS,0xFF,0xFF) 
{
	//{{AFX_DATA_INIT(CDlgSensPropChild)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT	
}

void CDlgSensPropChild::DoDataExchange(CDataExchange* pDX)
{
	CDlgUnitChildProp::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSensPropChild)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSensPropChild, CDlgUnitChildProp)
	//{{AFX_MSG_MAP(CDlgSensPropChild)
	//}}AFX_MSG_MAP
	ON_NOTIFY(UDN_DELTAPOS, IDC_UPDOWN_HYS_SUN_HIGH, OnUpDownHysValueHighSun)
	ON_NOTIFY(UDN_DELTAPOS, IDC_UPDOWN_HYS_HSUN_HIGH, OnUpDownHysValueHighHSun)
	ON_NOTIFY(UDN_DELTAPOS, IDC_UPDOWN_HYS_MOON_HIGH, OnUpDownHysValueHighMoon)
	ON_NOTIFY(UDN_DELTAPOS, IDC_UPDOWN_HYS_HMOON_HIGH, OnUpDownHysValueHighHMoon)
	ON_NOTIFY(UDN_DELTAPOS, IDC_UPDOWN_HYS_SUN_LOW, OnUpDownHysValueLowSun)
	ON_NOTIFY(UDN_DELTAPOS, IDC_UPDOWN_HYS_HSUN_LOW, OnUpDownHysValueLowHSun)
	ON_NOTIFY(UDN_DELTAPOS, IDC_UPDOWN_HYS_MOON_LOW, OnUpDownHysValueLowMoon)
	ON_NOTIFY(UDN_DELTAPOS, IDC_UPDOWN_HYS_HMOON_LOW, OnUpDownHysValueLowHMoon)
	ON_NOTIFY(UDN_DELTAPOS, IDC_UPDOWN_HYSOFFSET_HIGH, OnUpDownHysOffsetHigh)
	ON_NOTIFY(UDN_DELTAPOS, IDC_UPDOWN_HYSOFFSET_LOW, OnUpDownHysOffsetLow)	
	ON_NOTIFY(UDN_DELTAPOS, IDC_UPDOWN_SAMPLE_INTERVAL, OnUpdownSampleInterval)
	ON_NOTIFY(UDN_DELTAPOS, IDC_UPDOWN_OFFSET_MEASUREDVAL, OnUpdownMeasuredval)
	ON_NOTIFY(UDN_DELTAPOS, IDC_UPDOWN_SETPOINT_RANGE_MIN, OnUpdownSetpointRangeMin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_UPDOWN_SETPOINT_RANGE_MAX, OnUpdownSetpointRangeMax)
	ON_BN_CLICKED(IDC_BUTTON_RESETVALUES,OnButtonResetValues)
	ON_CBN_SELENDOK(IDC_COMBOBOX_SENSPROFILE, OnSelectSensProfile)
	ON_CBN_SELENDOK(IDC_CMB_SENS_DELEGATION, OnSelectDelegate)
	ON_BN_CLICKED(IDC_CHECK_SENS_EXTERNAL, OnCheckExternalSensor)
	ON_MESSAGE(WM_UPDATE_CONTROLS,OnUpdateControls)
	ON_CBN_SELENDOK(IDC_COMBOBOX_PIDCOOLING_UNIT, OnSelectPidCoolingUnit)
	ON_CBN_SELENDOK(IDC_COMBOBOX_PIDHEATING_UNIT, OnSelectPidHeatingUnit)
	ON_NOTIFY(UDN_DELTAPOS, IDC_UPDOWN_PIDCOOLING_KP, OnUpDownPidParam)
	ON_NOTIFY(UDN_DELTAPOS, IDC_UPDOWN_PIDCOOLING_KI, OnUpDownPidParam)
	ON_NOTIFY(UDN_DELTAPOS, IDC_UPDOWN_PIDCOOLING_KD, OnUpDownPidParam)
	ON_NOTIFY(UDN_DELTAPOS, IDC_UPDOWN_PIDHEATING_KP, OnUpDownPidParam)
	ON_NOTIFY(UDN_DELTAPOS, IDC_UPDOWN_PIDHEATING_KI, OnUpDownPidParam)
	ON_NOTIFY(UDN_DELTAPOS, IDC_UPDOWN_PIDHEATING_KD, OnUpDownPidParam)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

static BOOL comboboxAdd(CComboBox* const pControl,const CString text,const DWORD dwItemData) {
	const int idx=pControl->AddString(text);
	if (idx>=0) {								
		pControl->SetItemData(idx,dwItemData);	
		return TRUE;
	}		
	return FALSE;
}

// Add unit to the combobox.
// text : unitName, data = node+unit address.
static BOOL comboBoxAddUnit(CComboBox* const pControl,STRUCT_UNIT* pUnit) {	
	CString szEntry;	
	szEntry.Format("%s",pUnit->szUnitName);
	return comboboxAdd(pControl,szEntry,((pUnit->bNodeLogicalAddress<<8) + pUnit->bUnitAddress));
}

static BOOL comboBoxAddUnit(CComboBox* const pControl,CNodeDatabase* pNodeDatabase,
							BYTE bNodeAddress, BYTE bUnitAddress) 
{
	if (0 == pControl || 0 == pNodeDatabase) {
		ASSERT(0 != pControl);
		ASSERT(0 != pNodeDatabase);
		return FALSE;
	}
	if (0xFF == bNodeAddress || 0xFF == bUnitAddress) {
		ASSERT(bNodeAddress!=0xFF);
		ASSERT(bUnitAddress!=0xFF);
		return FALSE;
	}

	CNodeDatabaseOperations nodeDatabaseOperations(pNodeDatabase);
	STRUCT_UNIT* pUnit=0;	
	if (FALSE==nodeDatabaseOperations.Find(bNodeAddress,bUnitAddress,&pUnit)) {
		return FALSE;
	}
	ASSERT((bNodeAddress == pUnit->bNodeLogicalAddress) && (bUnitAddress == pUnit->bUnitAddress));
	return comboBoxAddUnit(pControl,pUnit);
}

/////////////////////////////////////////////////////////////////////////////

BYTE CDlgSensPropChild::getBindingUnitType( ) const {
	BYTE bRetVal = BINDING_UNIT_TYPE_SENS;
	bRetVal = m_bindingSensUnit.GetBindingUnitType( );
	ENSURE(bRetVal == BINDING_UNIT_TYPE_SENS ||
		   bRetVal == BINDING_UNIT_TYPE_SENSUNIV);
	return bRetVal;	
}

void CDlgSensPropChild::ComboBoxDelegationAddItem(BYTE bNodeAddress,BYTE bUnitAddress) {
	comboBoxAddUnit(&m_CmbDelegation,TMGetDocumentNodes(),bNodeAddress,bUnitAddress);
}

void CDlgSensPropChild::ComboBoxDelegationSelItem(BYTE bNodeAddress,BYTE bUnitAddress) {
	const int count = m_CmbDelegation.GetCount( );
	int i;
	int select=0;
	for(i=0;i<count;++i) {
		const DWORD dwItemData=m_CmbDelegation.GetItemData(i);	
		if ((bNodeAddress==((dwItemData&0xFF00)>>8))&&(bUnitAddress==(dwItemData&0xFF))) {
			ASSERT(bNodeAddress!=0xFF);
			ASSERT(bUnitAddress!=0xFF);
			select=i;
			break;
		}
	}
	m_CmbDelegation.SetCurSel(select);	
}

// Sedert versie 0x0c03 wordt de mode meegegeven als een param.
// ifv deze mode wordt een andere lijst gebruikt om de combobox op te vullen.
void CDlgSensPropChild::ComboBoxDelegationInit(mode_t mode)
{
	using DUOTECNO::MFC_HELPER::CComboxbox_util;

	// Alle items uit de combobox verwijderen.
	m_CmbDelegation.ResetContent( );

	CComboxbox_util::setComboBoxSetHeight(&m_CmbDelegation);

	switch(mode)
	{
		case MODE_DELEGATION_UNSUPPORTED:
		{
			// absorbe...
		} break;

		case MODE_DELEGATE_ATTACH_SUPPORTED:
		case MODE_DELEGATOR_ATTACHED:
		case MODE_DELEGATE_ATTACHED:
		{
			// Alle items toevoegen, Enkel de eigen unit niet.
			// Oppassen: Hier wordt de 'g_listUnitsDelegation' lijst gebruikt.
			// Deze lijst bevat alle units die delegation ondersteunen.
			 
			POSITION pos = g_listUnitsDelegation.GetHeadPosition();

			ASSERT(m_bindingSensUnit.GetSupportedFlags()&CBindingPropertySensUnit::SENS_SUPPORT_FLAGS_DELEGATION);

			while(pos!=NULL) 
			{
				CBindingPropertySensUnit* pEntry = 
					dynamic_cast<CBindingPropertySensUnit*>
						((CBindingPropertyUnit*)g_listUnitsDelegation.GetNext(pos));

				if (pEntry!=NULL)
				{		
					const BYTE bNodeAddress = pEntry->GetNodeAddress();
					const BYTE bUnitAddress = pEntry->GetUnitAddress();

					// Mag niet van dezelfde node zijn.
					if (bNodeAddress!=m_bindingSensUnit.GetNodeAddress())
					{		
						// Controleren: 
						// Er mag nog geen delegation toegekend zijn aan een andere unit.
						if ((pEntry->GetDelegationType()==CBindingPropertySensUnit::DELEGATION_TYPE_NONE) ||
							((pEntry->GetDelegationNodeAddress()==m_bindingSensUnit.GetNodeAddress())&&
							 (pEntry->GetDelegationUnitAddress()==m_bindingSensUnit.GetUnitAddress())))
						{
							ComboBoxDelegationAddItem(bNodeAddress,bUnitAddress);
						}
					}
				}
			}
		} break;

		case MODE_DELEDATION_REQUESTMEASURE_VALUES_SUPPORTED:
		case MODE_DELEDATION_REQUESTMEASURE_VALUES_ATTACHED:
		{		
			// Alle items uit de lijst toevoegen.
			// Oppassen: Hier wordt de 'g_listUnitsSensReadMeasuredValue' lijst gebruikt.
			// Deze lijst bevat alle units die kunnen gebruikt worden om de gemeten waarde op te vragen.
			// Er moet niet extra gecontroleerd worden (Een sensor die de SENS_SUPPORT_FLAGS_REQUESTMEASUREDVALUE
			// vlag geeft wordt niet toegevoegd aan deze lijst...)
		
			POSITION pos = g_listUnitsSensReadMeasuredValue.GetHeadPosition();

			ASSERT(m_bindingSensUnit.GetSupportedFlags()&CBindingPropertySensUnit::SENS_SUPPORT_FLAGS_REQUESTMEASUREDVALUE);

			while(pos!=NULL) 
			{
				CBindingPropertySensUnit* pEntry = 
					dynamic_cast<CBindingPropertySensUnit*>
						((CBindingPropertyUnit*)g_listUnitsDelegation.GetNext(pos));

				if (pEntry!=NULL)
				{		
					const BYTE bNodeAddress = pEntry->GetNodeAddress();
					const BYTE bUnitAddress = pEntry->GetUnitAddress();

					ASSERT(bNodeAddress!=0xFF);
					ASSERT(bUnitAddress!=0xFF);

					if (!((bNodeAddress==m_bindingSensUnit.GetNodeAddress())&&
						  (bUnitAddress==m_bindingSensUnit.GetUnitAddress())))
					{
						ComboBoxDelegationAddItem(bNodeAddress,bUnitAddress);	
					}
					else
					{
						// De lijst is al gefiltered dus mag nooit voorkomen.
						ASSERT(!((bNodeAddress==m_bindingSensUnit.GetNodeAddress())&&
								 (bUnitAddress==m_bindingSensUnit.GetUnitAddress())));

					}
				}
			} /* while(pos!=NULL)  */

		} break;
	}
}

// Sedert versie 0x0C03.
// Ondersteuning voor modbus sensor: opvragen van de gemeten waarde via een andere sensor.
void CDlgSensPropChild::ComboBoxDelegationUpdate(mode_t mode)
{
	switch(mode)
	{
		case MODE_DELEGATE_ATTACH_SUPPORTED:
		{
			// Absorbe...		

			ASSERT(m_bindingSensUnit.GetSupportedFlags()&
				   CBindingPropertySensUnit::SENS_SUPPORT_FLAGS_DELEGATION);

			ASSERT(m_bindingSensUnit.GetSupportedFlags()&
				CBindingPropertySensUnit::SENS_SUPPORT_FLAGS_ATTACH_DELEGATE);

			ASSERT(!(m_bindingSensUnit.GetSupportedFlags()&
					 CBindingPropertySensUnit::SENS_SUPPORT_FLAGS_REQUESTMEASUREDVALUE));
		} break;

		case MODE_DELEGATOR_ATTACHED:
		{
			const BYTE bNodeAddress = m_bindingSensUnit.GetDelegationNodeAddress();
			const BYTE bUnitAddress = m_bindingSensUnit.GetDelegationUnitAddress();

			ASSERT(bNodeAddress!=0xFF);
			ASSERT(bUnitAddress!=0xFF);

			ASSERT(m_bindingSensUnit.GetDelegationType()==
				   CBindingPropertySensUnit::DELEGATION_TYPE_DELEGATOR);

			ASSERT(m_bindingSensUnit.GetSupportedFlags()&
				   CBindingPropertySensUnit::SENS_SUPPORT_FLAGS_DELEGATION);
			ASSERT(!(m_bindingSensUnit.GetSupportedFlags()&
					 CBindingPropertySensUnit::SENS_SUPPORT_FLAGS_REQUESTMEASUREDVALUE));

			if ((0xFF!=bNodeAddress)&&(0xFF!=bUnitAddress))
			{
				ComboBoxDelegationSelItem(bNodeAddress,bUnitAddress);
			}
		} break;

		case MODE_DELEGATE_ATTACHED:
		{
			const BYTE bNodeAddress = m_bindingSensUnit.GetDelegationNodeAddress();
			const BYTE bUnitAddress = m_bindingSensUnit.GetDelegationUnitAddress();

			ASSERT(bNodeAddress!=0xFF);
			ASSERT(bUnitAddress!=0xFF);

			ASSERT(m_bindingSensUnit.GetDelegationType()==
				   CBindingPropertySensUnit::DELEGATION_TYPE_DELEGATE);		

			ASSERT(m_bindingSensUnit.GetSupportedFlags()&
				   CBindingPropertySensUnit::SENS_SUPPORT_FLAGS_DELEGATION);
			ASSERT(!(m_bindingSensUnit.GetSupportedFlags()&
					 CBindingPropertySensUnit::SENS_SUPPORT_FLAGS_REQUESTMEASUREDVALUE));

			if ((0xFF!=bNodeAddress)&&(0xFF!=bUnitAddress))
			{
				ComboBoxDelegationSelItem(bNodeAddress,bUnitAddress);
			}
		} break;

		case MODE_DELEDATION_REQUESTMEASURE_VALUES_SUPPORTED:
		{
			// absorbe...

			ASSERT(m_bindingSensUnit.GetSupportedFlags()&
				CBindingPropertySensUnit::SENS_SUPPORT_FLAGS_REQUESTMEASUREDVALUE);
			ASSERT(!(m_bindingSensUnit.GetSupportedFlags()&
					 CBindingPropertySensUnit::SENS_SUPPORT_FLAGS_DELEGATION));

		} break;

		case MODE_DELEDATION_REQUESTMEASURE_VALUES_ATTACHED:
		{
			const BYTE bNodeAddress = m_bindingSensUnit.GetDelegationNodeAddress();
			const BYTE bUnitAddress = m_bindingSensUnit.GetDelegationUnitAddress();

			ASSERT(bNodeAddress!=0xFF);
			ASSERT(bUnitAddress!=0xFF);

			ASSERT(m_bindingSensUnit.GetDelegationType()==
				CBindingPropertySensUnit::DELEGATION_TYPE_REQUESTVALUE);	

			ASSERT(m_bindingSensUnit.GetSupportedFlags()&
				CBindingPropertySensUnit::SENS_SUPPORT_FLAGS_REQUESTMEASUREDVALUE);
			ASSERT(!(m_bindingSensUnit.GetSupportedFlags()&
					 CBindingPropertySensUnit::SENS_SUPPORT_FLAGS_DELEGATION));

			if ((0xFF!=bNodeAddress)&&(0xFF!=bUnitAddress))
			{
				ComboBoxDelegationSelItem(bNodeAddress,bUnitAddress);
			}
		} break;
	}/* switch(mode) */
}

void CDlgSensPropChild::staticDelegationUpdate(mode_t mode)
{
	if(mode==MODE_DELEGATOR_ATTACHED)
	{
		// Er is een delegator gekoppeld.

		const BYTE bNodeAddress = m_bindingSensUnit.GetDelegationNodeAddress();
		const BYTE bUnitAddress = m_bindingSensUnit.GetDelegationUnitAddress();

		ASSERT(m_bindingSensUnit.GetDelegationType()==
		   CBindingPropertySensUnit::DELEGATION_TYPE_DELEGATOR);

		ASSERT(bNodeAddress!=0xFF);
		ASSERT(bUnitAddress!=0xFF);

		ASSERT(m_bindingSensUnit.GetSupportedFlags()&
			   CBindingPropertySensUnit::SENS_SUPPORT_FLAGS_DELEGATION);
		ASSERT(!(m_bindingSensUnit.GetSupportedFlags()&
				 CBindingPropertySensUnit::SENS_SUPPORT_FLAGS_REQUESTMEASUREDVALUE));

		if ((0xFF!=bNodeAddress)&&(0xFF!=bUnitAddress))
		{	
			CNodeDatabaseOperations nodeDatabaseOperations(TMGetDocumentNodes());
			STRUCT_UNIT* pUnit=0;
			if (TRUE==nodeDatabaseOperations.Find(bNodeAddress,bUnitAddress,&pUnit))
			{
				CString szText;
				szText.Format(_T("Sensor unit is controlled by '%s'"),pUnit->szUnitName);
				m_staticDelegation.SetWindowText(szText);
			}					
			ASSERT(pUnit!=0);		// Unit could not be found...
		}
	}
}

// veranderingen versie 0x0C03:
// Ondersteuning Modbus sensor unit die de gemeten waarde opvraagt aan een gekoppelde 
// sensor unit.
void CDlgSensPropChild::ControlsDelegationUpdate(mode_t mode)
{
	BOOL fEnable = FALSE;

	switch(mode)
	{
		case MODE_DELEGATION_UNSUPPORTED:
		{
			// Er wordt niks van delegation ondersteund...

			ASSERT(!(m_bindingSensUnit.GetSupportedFlags()&
					CBindingPropertySensUnit::SENS_SUPPORT_FLAGS_REQUESTMEASUREDVALUE));

			fEnable = TRUE;

			m_CheckBtnDelegationExtSensor.ShowWindow(FALSE);
			m_CmbDelegation.ShowWindow(FALSE);

			m_staticDelegation.ShowWindow(FALSE);

		} break;

		case MODE_DELEGATE_ATTACH_SUPPORTED:
		{		
			fEnable = TRUE;

			// Disabled wanneer:		
			// - Geen items in de listbox aanwezig zijn 			
			if (m_CmbDelegation.GetCount( )==0)
			{
				m_CheckBtnDelegationExtSensor.ShowWindow(FALSE);
			} 
			else
			{
				m_CheckBtnDelegationExtSensor.ShowWindow(TRUE);
			}

			m_CheckBtnDelegationExtSensor.SetCheck(0);
			m_CmbDelegation.ShowWindow(FALSE);

		} break;

		case MODE_DELEGATOR_ATTACHED:
		{
			fEnable = TRUE;

			m_CheckBtnDelegationExtSensor.ShowWindow(FALSE);
			m_CmbDelegation.ShowWindow(FALSE);
			m_staticDelegation.ShowWindow(TRUE);

		} break;		
		
		case MODE_DELEGATE_ATTACHED:
		{
			fEnable = FALSE;

			m_CheckBtnDelegationExtSensor.ShowWindow(TRUE);
			m_CheckBtnDelegationExtSensor.SetCheck(1);
			m_CmbDelegation.ShowWindow(TRUE);
			m_staticDelegation.ShowWindow(FALSE);	// Zie BUG-0150

		} break;

		case MODE_DELEDATION_REQUESTMEASURE_VALUES_SUPPORTED:	// sedert 0x0C03
		{
			fEnable = TRUE;

			if (m_CmbDelegation.GetCount( )==0)
			{
				m_CheckBtnDelegationExtSensor.ShowWindow(FALSE);
			}
			else
			{
				m_CheckBtnDelegationExtSensor.ShowWindow(TRUE);
			}

			m_CheckBtnDelegationExtSensor.SetCheck(0);
			m_CmbDelegation.ShowWindow(FALSE);
			m_staticDelegation.ShowWindow(FALSE);	// Zie BUG-0150

		} break;

		case MODE_DELEDATION_REQUESTMEASURE_VALUES_ATTACHED:	// Sedert 0x0C03
		{	
			fEnable = TRUE;

			m_CheckBtnDelegationExtSensor.ShowWindow(TRUE);
			m_CheckBtnDelegationExtSensor.SetCheck(1);
			m_CmbDelegation.ShowWindow(TRUE);

		} break;

	} /* switch(mode) */

	ComboBoxDelegationUpdate(mode);
	staticDelegationUpdate(mode);

	for (int i=0;i<2;i++) 
	{
		for (int j=0;j<4;j++) 
		{	
			m_EditBuddyHysValue[i][j].EnableWindow(fEnable);
			m_SpinHysVal[i][j].EnableWindow(fEnable);
		}
		m_EditBuddyHysOffset[i].EnableWindow(fEnable);
		m_SpinHysOffset[i].EnableWindow(fEnable);
	}

	m_EditBuddySample.EnableWindow(fEnable);
	m_SpinSample.EnableWindow(fEnable);
	m_CmbSensType.EnableWindow(fEnable);
	m_CmbSetProfile.EnableWindow(fEnable);
	m_EditBuddyOffsetMeasuredVal.EnableWindow(fEnable);
	m_SpinOffsetMeasuredVal.EnableWindow(fEnable);
}

// Sedert versie 0x0C03.
// Ondersteuning voor modbus sensor: opvragen van de gemeten waarde via een andere sensor.
CDlgSensPropChild::mode_t CDlgSensPropChild::DelegationGetMode(void)
{
	mode_t mode = MODE_DELEGATION_UNSUPPORTED;
	const unsigned short usSupportedFlags = m_bindingSensUnit.GetSupportedFlags(); 

	if (usSupportedFlags&
		CBindingPropertySensUnit::SENS_SUPPORT_FLAGS_DELEGATION)
	{	
		const BYTE bDelegationType = m_bindingSensUnit.GetDelegationType();

		if (CBindingPropertySensUnit::DELEGATION_TYPE_DELEGATOR==
			bDelegationType)
		{
			mode=MODE_DELEGATOR_ATTACHED;
		}
		else if (CBindingPropertySensUnit::DELEGATION_TYPE_DELEGATE==
			bDelegationType)
		{
			mode=MODE_DELEGATE_ATTACHED;
		}
		else
		{
			if (usSupportedFlags&
				CBindingPropertySensUnit::SENS_SUPPORT_FLAGS_ATTACH_DELEGATE)
			{
				mode = MODE_DELEGATE_ATTACH_SUPPORTED;
			}
		}
	}
	else if (usSupportedFlags&
		CBindingPropertySensUnit::SENS_SUPPORT_FLAGS_REQUESTMEASUREDVALUE)
	{
		const BYTE bDelegationType = m_bindingSensUnit.GetDelegationType();

		mode=MODE_DELEDATION_REQUESTMEASURE_VALUES_SUPPORTED;

		if (CBindingPropertySensUnit::DELEGATION_TYPE_REQUESTVALUE==
			bDelegationType)
		{
			mode=MODE_DELEDATION_REQUESTMEASURE_VALUES_ATTACHED;
		}

		ASSERT(mode!=MODE_DELEGATION_UNSUPPORTED);
	}
	return mode;
}

void CDlgSensPropChild::setCtrlSampleTime(int value)
{
	CString szValue;
	const BYTE bBindingUnitType = this->getBindingUnitType( );

	REQUIRE(value>=0 && value<=120);

	switch(bBindingUnitType)
	{
		case BINDING_UNIT_TYPE_SENS:
		{
			szValue.Format("%d sec",value);
		} break;

		case BINDING_UNIT_TYPE_SENSUNIV:
		{
			if (value>=SENSUNIV_SAMPLETIME_MINUTE)
			{
				value-=(SENSUNIV_SAMPLETIME_MINUTE-1);
				szValue.Format("%d min",value);
			}
			else 
			{
				szValue.Format("%d sec",value);
			}
		} break;
	}	
	m_EditBuddySample.SetWindowText(szValue);
}

LONG CDlgSensPropChild::OnUpdateControls(WPARAM wParam,LPARAM lParam)
{
	if (m_bindingSensUnit.GetSensType() == SENS_CONFIG_TEMPERATURE) 
	{		
		CString szValue;
		BYTE bValue;
		const CDlgSensPropChild::mode_t mode = DelegationGetMode( );
				
		ControlsDelegationUpdate(mode);
		ControlsSetpointRangeUpdate();
		ControlsPidUpdate();				// Sedert V16.00: USE_SENSOR_PIDCONTROL	

		for (int i=0;i<2;i++) {
			for (int j=0;j<4;j++) {
				bValue = m_bindingSensUnit.GetHysValues(i,j);
				szValue.Format("%1d,%1d",bValue/10,bValue%10);
				m_EditBuddyHysValue[i][j].SetWindowText(szValue);
			}
			bValue = m_bindingSensUnit.GetHysOffsetValues(i);
			szValue.Format("%1d,%1d",bValue/10,bValue%10);
			m_EditBuddyHysOffset[i].SetWindowText(szValue);
		}

		setCtrlSampleTime(m_bindingSensUnit.GetSampleTime());

		if (m_CmbSetProfile.GetCurSel() != 0) {
			m_CmbSetProfile.SetCurSel(0);
		}

		if (1) {
			const short sValue= m_bindingSensUnit.GetMeasuredValueOffset();
			szValue.Format("%.1f",(float) ((float)(sValue)/10));
			m_EditBuddyOffsetMeasuredVal.SetWindowTextA(szValue);
		}
	}
	return TRUE;
}

void CDlgSensPropChild::OnNotifyClose(void)
{
	if (m_fChangesMade == TRUE)
	{
		CString szMessage;
		szMessage.Format(_T("Store Changes of Sensor Unit 0x%02X,0x%02X ?"),
						 bNodeAddress,
						 bUnitAddress);

		if (AfxMessageBox(szMessage,MB_YESNO) == IDYES) 
		{						
			// Construct property Bindings ...		
			*ptrOriginalSensUnit = m_bindingSensUnit;
			CBindingConversionToNodesProperty ConvertBindingToNodes(ptrOriginalSensUnit);
			
			if (ConvertBindingToNodes.ProcessPropertyBindings()>=0)
			{
				// Delegation bindings:
				// Controleren of er via delegation een buddy geconfigureerd is.
				// Hiervoor bindings aanmaken.
				if (ptrOriginalSensUnit->GetSupportedFlags()&
					CBindingPropertySensUnit::SENS_SUPPORT_FLAGS_DELEGATION)
				{
					BOOL fVerifyDelegation = FALSE;

					TRACE("\r\n"
						  "CDlgSensPropChild::ptrOriginalSensUnit:type=%d,address=0x%02x,0x%02x"
						  "\r\n",
						  ptrOriginalSensUnit->GetDelegationType(),
						  ptrOriginalSensUnit->GetDelegationNodeAddress(),
						  ptrOriginalSensUnit->GetDelegationUnitAddress());

					// Indien delegation ondersteund is voor deze binding unit dan algoritme starten
					// Die bindings aanmaakt voor de delegator/delegate...
					bindingPropertyUnitSens_delegationBuddyAdjust(
							g_listUnitsDelegation,
							*ptrOriginalSensUnit);		

					// Controleren of er een buddy aangemaakt werd.
					if (ptrOriginalSensUnit->GetDelegationType()!= 
						CBindingPropertySensUnit::DELEGATION_TYPE_NONE)
					{
						const BOOL fVerify=
							bindingPropertyUnitSens_delegationBuddyVerify(
										g_listUnitsDelegation,
										*ptrOriginalSensUnit);

						if (FALSE==fVerify)
						{				
							CString s;

							s.Format(_T("Problem to configure external sensor.\r\n"
										"Reason: The delegator binding in node 0x%02x could not be added.\r\n"),
										ptrOriginalSensUnit->GetDelegationNodeAddress());

							AfxMessageBox(s,MB_ICONERROR);

							// Hier wordt de binding verwijderd.
							bindingPropertyUnitSens_delegationBindingsRemove(
								TMGetBindingFiles(),
								ptrOriginalSensUnit);
						}
					}
				}
			}
		}
	}
}


// Zie BUG-0179: Bij het maken van de defaults wordt de laatste waarde gebruikt. 
// vb: Als er 2 sensoren van de smartbox aanwezig zijn.
//     Sensor1: 20sec, sensor2: 23sec.
//     Bij het maken van defaults voor sensor 1 wordt er in de GUI 
//     23sec getoond en wordt er een binding gemaakt voor de 20 seconden.
//
// Reden: CBindingPropertyUnit is gekoppeld aan de CBindingPropertyUnitSensDefault* m_pDefaults
// Deze m_pDefaults is gekoppeld aan de static instance aangemaakt door de factory.
// Voor iedere versie is er maar 1 instantie. (Bevat data van de laats aangemaakte met de hoogste idx)
void CDlgSensPropChild::InitControls(CBindingPropertyUnit *ptr)
{
	ASSERT(ptr);
	ptrOriginalSensUnit = (CBindingPropertySensUnit*)ptr;
	m_bindingSensUnit = *ptrOriginalSensUnit;

	m_fChangesMade = FALSE;
	bNodeAddress = ptr->GetNodeAddress();
	bUnitAddress = ptr->GetUnitAddress();
	
	ASSERT(bNodeAddress!=0xFF);
	ASSERT(bUnitAddress!=0xFF);

	// Sedert versie 0x0C03 wordt de mode meegeven aan de functie 
	// Wanneer er een nieuwe unit toegekend wordt moet de combobox 
	// opnieuw geinitialisserd worden met nieuwe items...

	const mode_t mode = DelegationGetMode();	
	ComboBoxDelegationInit(mode);

	initStaticControls(this->getBindingUnitType());

	// Controls updaten.
	UpdateControls();
}


void CDlgSensPropChild::initStaticControls(BYTE bBindingUnitType)
{
	CString szHystHigh(_T("Hyst. High °C"));
	CString szHystLow(_T("Hyst. Low °C"));
	CString szHystOffsetHigh(_T("High °C"));
	CString szHystOffsetLow(_T("Low °C"));
	BOOL fEnable = TRUE;

	switch(bBindingUnitType)
	{
		case  BINDING_UNIT_TYPE_SENS:
		{
		
		} break;
		
		case BINDING_UNIT_TYPE_SENSUNIV:
		{
			fEnable = FALSE;
			szHystHigh = (_T("Hyst. High %"));
			szHystLow= (_T("Hyst. Low %"));
			szHystOffsetHigh = (_T("High %"));
			szHystOffsetLow = (_T("Low %"));
		} break;
	}

	m_BtnGroup.SetWindowText(szHystHigh);
	m_BtnGroup2.SetWindowText(szHystLow);
	m_StaticHysOffset[0].SetWindowText(szHystOffsetHigh);
	m_StaticHysOffset[1].SetWindowText(szHystOffsetLow);

	m_CmbSetProfile.ShowWindow(fEnable);
	m_EditBuddyOffsetMeasuredVal.ShowWindow(fEnable);
	m_SpinOffsetMeasuredVal.ShowWindow(fEnable);	
	m_BtnGroupOffsetMeasuredValue.ShowWindow(fEnable);
						  
}

/////////////////////////////////////////////////////////////////////////////
// CDlgSensPropChild message handlers

// Sedert versie 0x0900
void CDlgSensPropChild::ControlsDelegationCreate(void)
{
	const DWORD dwStyleCheckBox =  WS_CHILD /*| WS_VISIBLE*/ | BS_CHECKBOX; 
	const CRect rectCheckBox(CPoint(0,0),CSize(SENS_CHECK_BUTTON_X_SIZE,SENS_CHECK_BUTTON_Y_SIZE));

	// 0x0E07: WS_VSCROLL bijgevoegd.
	const DWORD dwStyleComboBox = /*WS_VISIBLE |*/ WS_CHILD | CBS_DROPDOWN | WS_VSCROLL;
	const CRect rectCombo(CPoint(140,0),CSize(160,100));

	const DWORD dwStyleStatic = WS_CHILD;
	const CRect rectStatic(CPoint(0,0),CSize(300,20));

	m_CheckBtnDelegationExtSensor.Create(
		"Use external sensor",
		dwStyleCheckBox ,
		rectCheckBox,
		this,
		IDC_CHECK_SENS_EXTERNAL);


	m_CmbDelegation.Create(
		dwStyleComboBox,
		rectCombo,
		this,
		IDC_CMB_SENS_DELEGATION);


	m_staticDelegation.Create(
		"",
		dwStyleStatic,
		rectStatic,
		this);

	CFont* const pFont=GetFont();
	if (0!=pFont)
	{
		m_CmbDelegation.SetFont(pFont);
		m_CheckBtnDelegationExtSensor.SetFont(pFont);
		m_staticDelegation.SetFont(pFont);
	}
}

// Sedert versie 0x0E00
void CDlgSensPropChild::ControlsSetpointRangeCreate(void)
{
	if (1)
	{
		CPoint topLeft(SENS_TOPLEFT_GROUP_SP_RANGE_X,SENS_TOPLEFT_GROUP_SP_RANGE_Y);
		CSize controlSize(SENS_GROUP_SIZE_X,SENS_GROUP_SP_RANGE_SIZE_Y);
		CString szTitle(_T("Setpoint range"));

		m_BtnGroupSPRange.Create((LPCTSTR) szTitle,			
						  WS_VISIBLE | WS_CHILD | BS_GROUPBOX,
						  CRect(topLeft,controlSize),
						  this,
						  IDC_GROUPBOX_SETPOINT_RANGE);
	}

	if (1)
	{
		CString szTitle(_T("Min"));
		CPoint topLeft(SENS_TOPLEFT_SP_RANGE_MIN_X,SENS_TOPLEFT_SP_RANGE_MIN_Y);

		_CreateSpin(&m_EditBuddySPRange[0],
					IDC_BUDDY_EDIT_SETPOINT_RANGE_MIN,
					&m_SpinSPRange[0],
					IDC_UPDOWN_SETPOINT_RANGE_MIN,
					&m_StaticSPRange[0],
					topLeft,
					(LPCTSTR)szTitle);	
	}

	if (1)
	{
		CString szTitle(_T("Max"));
		CPoint topLeft(SENS_TOPLEFT_SP_RANGE_MAX_X,SENS_TOPLEFT_SP_RANGE_MAX_Y);

		_CreateSpin(&m_EditBuddySPRange[1],
				    IDC_BUDDY_EDIT_SETPOINT_RANGE_MAX,
					&m_SpinSPRange[1],
					IDC_UPDOWN_SETPOINT_RANGE_MAX,
					&m_StaticSPRange[1],
					topLeft,
					(LPCTSTR)szTitle);		// CHANGES_MVS_2008_CSTRING
	}

	CFont* const pFont=GetFont();
	if (0!=pFont)
	{
		m_BtnGroupSPRange.SetFont(pFont);
		m_EditBuddySPRange[0].SetFont(pFont);
		m_EditBuddySPRange[1].SetFont(pFont);
		m_StaticSPRange[0].SetFont(pFont);
		m_StaticSPRange[1].SetFont(pFont);
	}
}

void CDlgSensPropChild::ControlsSetpointRangeUpdate(void)
{
	BOOL visible = FALSE;

	if (m_bindingSensUnit.getSetpointRangeSupported())
	{
		if (1)
		{
			CString szValue;
			short value = m_bindingSensUnit.getSetpointRangeValueMin();
			szValue.Format("%d,%1d",value/10,value%10);
			m_EditBuddySPRange[0].SetWindowText(szValue);
		}

		if (1)
		{
			CString szValue;
			short value = m_bindingSensUnit.getSetpointRangeValueMax();
			szValue.Format("%d,%1d",value/10,value%10);
			m_EditBuddySPRange[1].SetWindowText(szValue);
		}

		visible=TRUE;
	}	

	m_BtnGroupSPRange.ShowWindow(visible);
	m_StaticSPRange[0].ShowWindow(visible);
	m_StaticSPRange[1].ShowWindow(visible);
	m_EditBuddySPRange[0].ShowWindow(visible);
	m_EditBuddySPRange[1].ShowWindow(visible);
	m_SpinSPRange[0].ShowWindow(visible);
	m_SpinSPRange[1].ShowWindow(visible);
}

// Sedert V16.00: USE_SENSOR_PIDCONTROL	
void CDlgSensPropChild::ControlsPidCreate(void) {
	if (1)
	{
		// Groep box: PID Cooling.
		const CPoint topLeft(0,SENS_TOPLEFT_GROUP_PID_Y);
		const CSize controlSize(SENS_GROUP_SIZE_X,SENS_GROUP_PID_SIZE_Y);
		const CString szTitle(_T("PID Cooling"));

		m_btnGroupPIDControl[SENS_PIDCONTROL_WORKINGMODE_COOLING].Create((LPCTSTR) szTitle,			
				WS_VISIBLE | WS_CHILD | BS_GROUPBOX, CRect(topLeft,controlSize),
				this, IDC_GROUPBOX_PID_COOLING);
	}

	if (1)
	{
		// Groep box: PID Heating.
		const CPoint topLeft(140,SENS_TOPLEFT_GROUP_PID_Y);
		const CSize controlSize(SENS_GROUP_SIZE_X,SENS_GROUP_PID_SIZE_Y);
		const CString szTitle(_T("PID Heating"));

		m_btnGroupPIDControl[SENS_PIDCONTROL_WORKINGMODE_HEATING].Create((LPCTSTR) szTitle,			
				WS_VISIBLE | WS_CHILD | BS_GROUPBOX, CRect(topLeft,controlSize),
				this, IDC_GROUPBOX_PID_HEATING);
	}

	if (1)
	{	
		CPoint topLeft(10,SENS_TOPLEFT_GROUP_PID_Y+15);

		{				
			// Sedert V16.01: WS_VSCROLL toevoegen
			CSize szControlSize(120,100);
			m_CmbPIDControlUnit[SENS_PIDCONTROL_WORKINGMODE_COOLING].Create(WS_VISIBLE | WS_CHILD | CBS_DROPDOWN | WS_VSCROLL,
							 CRect(topLeft,szControlSize),
							 this,
							 IDC_COMBOBOX_PIDCOOLING_UNIT);			
		}

		topLeft.y += 25;
		_CreateSpin2(&m_EditBuddyPIDParams[SENS_PIDCONTROL_WORKINGMODE_COOLING][SENS_PIDCONTROL_PARAM_KP],
					IDC_BUDDY_EDIT_PIDCOOLING_KP,
					&m_SpinPIDParams[SENS_PIDCONTROL_WORKINGMODE_COOLING][SENS_PIDCONTROL_PARAM_KP],
					IDC_UPDOWN_PIDCOOLING_KP,
					&m_StaticPIDParams[SENS_PIDCONTROL_WORKINGMODE_COOLING][SENS_PIDCONTROL_PARAM_KP],
					topLeft,
					(LPCTSTR)_T("Kp:"));	

		topLeft.y += 22;
		_CreateSpin2(&m_EditBuddyPIDParams[SENS_PIDCONTROL_WORKINGMODE_COOLING][SENS_PIDCONTROL_PARAM_KI],
					IDC_BUDDY_EDIT_PIDCOOLING_KI,
					&m_SpinPIDParams[SENS_PIDCONTROL_WORKINGMODE_COOLING][SENS_PIDCONTROL_PARAM_KI],
					IDC_UPDOWN_PIDCOOLING_KI,
					&m_StaticPIDParams[SENS_PIDCONTROL_WORKINGMODE_COOLING][SENS_PIDCONTROL_PARAM_KI],
					topLeft,
					(LPCTSTR)_T("Ki:"));

		topLeft.y += 22;
		_CreateSpin2(&m_EditBuddyPIDParams[SENS_PIDCONTROL_WORKINGMODE_COOLING][SENS_PIDCONTROL_PARAM_KD],
					IDC_BUDDY_EDIT_PIDCOOLING_KD,
					&m_SpinPIDParams[SENS_PIDCONTROL_WORKINGMODE_COOLING][SENS_PIDCONTROL_PARAM_KD],
					IDC_UPDOWN_PIDCOOLING_KD,
					&m_StaticPIDParams[SENS_PIDCONTROL_WORKINGMODE_COOLING][SENS_PIDCONTROL_PARAM_KD],
					topLeft,
					(LPCTSTR)_T("Kd:"));
	}

	if (1)
	{	
		CPoint topLeft(150,SENS_TOPLEFT_GROUP_PID_Y+15);

		{				
			// Sedert V16.01: WS_VSCROLL toevoegen
			CSize szControlSize(120,100);
			m_CmbPIDControlUnit[SENS_PIDCONTROL_WORKINGMODE_HEATING].Create(WS_VISIBLE | WS_CHILD | CBS_DROPDOWN | WS_VSCROLL,
							 CRect(topLeft,szControlSize),
							 this,
							 IDC_COMBOBOX_PIDHEATING_UNIT);			
		}

		topLeft.y += 25;
		_CreateSpin2(&m_EditBuddyPIDParams[SENS_PIDCONTROL_WORKINGMODE_HEATING][SENS_PIDCONTROL_PARAM_KP],
					IDC_BUDDY_EDIT_PIDHEATING_KP,
					&m_SpinPIDParams[SENS_PIDCONTROL_WORKINGMODE_HEATING][SENS_PIDCONTROL_PARAM_KP],
					IDC_UPDOWN_PIDHEATING_KP,
					&m_StaticPIDParams[SENS_PIDCONTROL_WORKINGMODE_HEATING][SENS_PIDCONTROL_PARAM_KP],
					topLeft,
					(LPCTSTR)_T("Kp:"));	

		topLeft.y += 22;
		_CreateSpin2(&m_EditBuddyPIDParams[SENS_PIDCONTROL_WORKINGMODE_HEATING][SENS_PIDCONTROL_PARAM_KI],
					IDC_BUDDY_EDIT_PIDHEATING_KI,
					&m_SpinPIDParams[SENS_PIDCONTROL_WORKINGMODE_HEATING][SENS_PIDCONTROL_PARAM_KI],
					IDC_UPDOWN_PIDHEATING_KI,
					&m_StaticPIDParams[SENS_PIDCONTROL_WORKINGMODE_HEATING][SENS_PIDCONTROL_PARAM_KI],
					topLeft,
					(LPCTSTR)_T("Ki:"));

		topLeft.y += 22;
		_CreateSpin2(&m_EditBuddyPIDParams[SENS_PIDCONTROL_WORKINGMODE_HEATING][SENS_PIDCONTROL_PARAM_KD],
					IDC_BUDDY_EDIT_PIDHEATING_KD,
					&m_SpinPIDParams[SENS_PIDCONTROL_WORKINGMODE_HEATING][SENS_PIDCONTROL_PARAM_KD],
					IDC_UPDOWN_PIDHEATING_KD,
					&m_StaticPIDParams[SENS_PIDCONTROL_WORKINGMODE_HEATING][SENS_PIDCONTROL_PARAM_KD],
					topLeft,
					(LPCTSTR)_T("Kd:"));
	}	

	{
		// Alle units weergeven in de combobox...

		// Wissen van de lijst in de combobox + eerste item is de disable.
		m_CmbPIDControlUnit[SENS_PIDCONTROL_WORKINGMODE_COOLING].ResetContent();
		comboboxAdd(&m_CmbPIDControlUnit[SENS_PIDCONTROL_WORKINGMODE_COOLING],"",((0xff<<8) + 0xff));		

		m_CmbPIDControlUnit[SENS_PIDCONTROL_WORKINGMODE_HEATING].ResetContent();	
		comboboxAdd(&m_CmbPIDControlUnit[SENS_PIDCONTROL_WORKINGMODE_HEATING],"",((0xff<<8) + 0xff));

		// Combobox: De lijst van de units aanvullen.
		POSITION pos = g_listUnitsDimPIDControl.GetHeadPosition();
		while(pos!=NULL) {
			STRUCT_UNIT* const pUnit = reinterpret_cast<STRUCT_UNIT*>(g_listUnitsDimPIDControl.GetNext(pos));
			if (0 != pUnit) {
				comboBoxAddUnit(&m_CmbPIDControlUnit[SENS_PIDCONTROL_WORKINGMODE_COOLING], pUnit);
				comboBoxAddUnit(&m_CmbPIDControlUnit[SENS_PIDCONTROL_WORKINGMODE_HEATING], pUnit);
			}
		}	
	}

	CFont* const pFont=GetFont();
	if (0!=pFont) {
		m_btnGroupPIDControl[SENS_PIDCONTROL_WORKINGMODE_COOLING].SetFont(pFont);
		m_btnGroupPIDControl[SENS_PIDCONTROL_WORKINGMODE_HEATING].SetFont(pFont);
		m_CmbPIDControlUnit[SENS_PIDCONTROL_WORKINGMODE_COOLING].SetFont(pFont);
		m_CmbPIDControlUnit[SENS_PIDCONTROL_WORKINGMODE_HEATING].SetFont(pFont);
	}

	{
		// Sedert V16.01: Deze zorgt ervoor dat er pijlen in de combobox komen.
		using DUOTECNO::MFC_HELPER::CComboxbox_util;
		CComboxbox_util::setComboBoxSetHeight(&m_CmbPIDControlUnit[SENS_PIDCONTROL_WORKINGMODE_COOLING]);
		CComboxbox_util::setComboBoxSetHeight(&m_CmbPIDControlUnit[SENS_PIDCONTROL_WORKINGMODE_HEATING]);
	}
}


// Sedert V16.03:
// PID regeling instelling enkel in expert view weergeven.
void CDlgSensPropChild::ControlsPidUpdate(void) {

	const BOOL visible = m_bindingSensUnit.getPIDControlSupported() && ViewModeExtended();
	
	// Weergave van de velden enkel indien deze ondersteund worden door deze unit...
	for (int mode=0;mode<SENS_PIDCONTROL_WORKINGMODE_MAX;++mode) {
		m_btnGroupPIDControl[mode].ShowWindow(visible);
		m_CmbPIDControlUnit[mode].ShowWindow(visible);
		for (int param=0;param<SENS_PIDCONTROL_PARAM_MAX;++param) {
			m_StaticPIDParams[mode][param].ShowWindow(visible);
			m_EditBuddyPIDParams[mode][param].ShowWindow(visible);
			m_SpinPIDParams[mode][param].ShowWindow(visible);
		}
	}		


	if (visible) {

		const BOOL fDelegate = (CDlgSensPropChild::MODE_DELEGATE_ATTACHED == DelegationGetMode( ));		

		// Oppassen: NOTE
		// De eerste in de combobox is een disable...
		for (int mode=0;mode<SENS_PIDCONTROL_WORKINGMODE_MAX;++mode) {			
			BYTE bNodeAddress = 0xff;
			BYTE bUnitAddress = 0xff;
			if (!m_bindingSensUnit.getPropertyPIDControlUnit(mode,&bNodeAddress,&bUnitAddress)) {
				bNodeAddress = 0xff;
				bUnitAddress = 0xff;			
			}

			int selected=0;		// zie NOTE
			if (0xff != bNodeAddress || 0xff != bUnitAddress) {				
				const int itemData = (bNodeAddress<<8) + bUnitAddress;
				const int count = m_CmbPIDControlUnit[mode].GetCount( );
				int i;				
				for(i=0;i<count;++i) {					
					if (m_CmbPIDControlUnit[mode].GetItemData(i) == itemData) {		
						selected=i;
						break;
					}
				}
			}
		
			// Geselecteerde unit... in de combobox.
			m_CmbPIDControlUnit[mode].SetCurSel(selected);	
			m_CmbPIDControlUnit[mode].EnableWindow(!fDelegate);

			for (int param=0;param<SENS_PIDCONTROL_PARAM_MAX;++param) {
				// updaten van de waarden: kp,ki,kd
				CString szTempValue;
				const int value = m_bindingSensUnit.getPropertyPIDParam(mode,param);
				szTempValue.Format("%d",value);
				m_EditBuddyPIDParams[mode][param].SetWindowText(szTempValue);			
				// updaten window enable/disable.
				m_EditBuddyPIDParams[mode][param].EnableWindow((selected!=0)&&(!fDelegate));	// zie NOTE
			}
		}
	}
}

// Sedert V16.00: USE_SENSOR_PIDCONTROL: Positie aangepast
int CDlgSensPropChild::getBtnRestoreDefaultsPosY(void) {
	return (((MyWindowRect.bottom-MyWindowRect.top)-(MyWindowRect.bottom-MyWindowRect.top)/5)+20);
}

void CDlgSensPropChild::InitStaticControls()
{	
	CStatic *pStatic;
	CEdit *pEdit;
	CSpinButtonCtrl *pSpin;
	CString szStatic;
	CString szHystHigh(_T("Hyst. High °C"));
	CString szHystLow(_T("Hyst. Low °C"));
	CString szHystOffsetHigh(_T("High °C"));
	CString szHystOffsetLow(_T("Low °C"));


	CDlgUnitChildProp::InitStaticControls();

	{					
		ControlsDelegationCreate( );	// Sedert versie 0x0900		
		ControlsSetpointRangeCreate();	// Sedert versie 0x0E00
		ControlsPidCreate();			// Sedert V16.00: USE_SENSOR_PIDCONTROL	
		
		// Create Hyst Values High
		CSize szControlSize(SENS_GROUP_SIZE_X,SENS_GROUP_HYS_SIZE_Y);
		CPoint TopLeft(SENS_TOPLEFT_GROUP_HYSHIGH_X,SENS_TOPLEFT_GROUP_HYSHIGH_Y);

		m_BtnGroup.Create((LPCTSTR) szHystHigh,			// CHANGES_MVS_2008_CSTRING
						  WS_VISIBLE | WS_CHILD | BS_GROUPBOX,
						  CRect(TopLeft,szControlSize),
						  this,
						  IDC_GROUPBOX_HYSVALUES_HIGH);
		
		m_BtnGroup.SetFont(GetFont());

		// 4 modes : Hyst Values 
		szStatic.Format(_T("Sun"));
		pStatic = &m_StaticHysValue[HYST_OFFSET_HIGH][MODE_SUN];
		pEdit = &m_EditBuddyHysValue[HYST_OFFSET_HIGH][MODE_SUN];
		pSpin = &m_SpinHysVal[HYST_OFFSET_HIGH][MODE_SUN];
		TopLeft.x = SENS_TOPLEFT_HYSVAL_HIGH_X;
		TopLeft.y = SENS_TOPLEFT_HYSVAL_HIGH_Y;
		_CreateSpin(pEdit,IDC_BUDDY_EDIT_HYS_SUN_HIGH,
				   pSpin,IDC_UPDOWN_HYS_SUN_HIGH,
				   pStatic,TopLeft,(LPCTSTR) szStatic);	// CHANGES_MVS_2008_CSTRING

		szStatic.Format(_T("Half sun"));
		pStatic = &m_StaticHysValue[HYST_OFFSET_HIGH][MODE_HSUN];
		pEdit = &m_EditBuddyHysValue[HYST_OFFSET_HIGH][MODE_HSUN];
		pSpin = &m_SpinHysVal[HYST_OFFSET_HIGH][MODE_HSUN];
		TopLeft.x += SENS_OFFSET_HYSVAL_X;
		_CreateSpin(pEdit,IDC_BUDDY_EDIT_HYS_HSUN_HIGH,
				   pSpin,IDC_UPDOWN_HYS_HSUN_HIGH,
				   pStatic,TopLeft,(LPCTSTR) szStatic);	// CHANGES_MVS_2008_CSTRING
	
		szStatic.Format(_T("Half moon"));
		pStatic = &m_StaticHysValue[HYST_OFFSET_HIGH][MODE_HMOON];
		pEdit = &m_EditBuddyHysValue[HYST_OFFSET_HIGH][MODE_HMOON];
		pSpin = &m_SpinHysVal[HYST_OFFSET_HIGH][MODE_HMOON];
		TopLeft.y += SENS_OFFSET_HYSVAL_Y;
		_CreateSpin(pEdit,IDC_BUDDY_EDIT_HYS_HMOON_HIGH,
				   pSpin,IDC_UPDOWN_HYS_HMOON_HIGH,
				   pStatic,TopLeft,(LPCTSTR) szStatic);	// CHANGES_MVS_2008_CSTRING

		szStatic.Format(_T("Moon"));
		pStatic = &m_StaticHysValue[HYST_OFFSET_HIGH][MODE_MOON];
		pEdit = &m_EditBuddyHysValue[HYST_OFFSET_HIGH][MODE_MOON];
		pSpin = &m_SpinHysVal[HYST_OFFSET_HIGH][MODE_MOON];
		TopLeft.x -= SENS_OFFSET_HYSVAL_X;
		_CreateSpin(pEdit,IDC_BUDDY_EDIT_HYS_MOON_HIGH,
				   pSpin,IDC_UPDOWN_HYS_MOON_HIGH,
				   pStatic,TopLeft,(LPCTSTR) szStatic);	// CHANGES_MVS_2008_CSTRING
	}
	
	{ // Create Hyst Values Low
		CSize szControlSize(SENS_GROUP_SIZE_X,SENS_GROUP_HYS_SIZE_Y);
		CPoint TopLeft(SENS_TOPLEFT_GROUP_HYSLOW_X,SENS_TOPLEFT_GROUP_HYSLOW_Y);
		
		m_BtnGroup2.Create((LPCTSTR) szHystLow,			// CHANGES_MVS_2008_CSTRING
						  WS_VISIBLE | WS_CHILD | BS_GROUPBOX,
						  CRect(TopLeft,szControlSize),
						  this,
						  IDC_GROUPBOX_HYSVALUES_LOW);
	
		m_BtnGroup2.SetFont(GetFont());

		// 4 modes : Hyst Values 
		szStatic.Format(_T("Sun"));
		pStatic = &m_StaticHysValue[HYST_OFFSET_LOW][MODE_SUN];
		pEdit = &m_EditBuddyHysValue[HYST_OFFSET_LOW][MODE_SUN];
		pSpin = &m_SpinHysVal[HYST_OFFSET_LOW][MODE_SUN];
		TopLeft.x = SENS_TOPLEFT_HYSVAL_LOW_X;
		TopLeft.y = SENS_TOPLEFT_HYSVAL_LOW_Y;
		_CreateSpin(pEdit,IDC_BUDDY_EDIT_HYS_SUN_LOW,
				   pSpin,IDC_UPDOWN_HYS_SUN_LOW,
				   pStatic,TopLeft,(LPCTSTR) szStatic);		// CHANGES_MVS_2008_CSTRING

		szStatic.Format(_T("Half sun"));
		pStatic = &m_StaticHysValue[HYST_OFFSET_LOW][MODE_HSUN];
		pEdit = &m_EditBuddyHysValue[HYST_OFFSET_LOW][MODE_HSUN];
		pSpin = &m_SpinHysVal[HYST_OFFSET_LOW][MODE_HSUN];
		TopLeft.x += SENS_OFFSET_HYSVAL_X;
		_CreateSpin(pEdit,IDC_BUDDY_EDIT_HYS_HSUN_LOW,
				   pSpin,IDC_UPDOWN_HYS_HSUN_LOW,
				   pStatic,TopLeft,(LPCTSTR) szStatic);		// CHANGES_MVS_2008_CSTRING

		szStatic.Format(_T("Half moon"));
		pStatic = &m_StaticHysValue[HYST_OFFSET_LOW][MODE_HMOON];
		pEdit = &m_EditBuddyHysValue[HYST_OFFSET_LOW][MODE_HMOON];
		pSpin = &m_SpinHysVal[HYST_OFFSET_LOW][MODE_HMOON];
		TopLeft.y += SENS_OFFSET_HYSVAL_Y;
		_CreateSpin(pEdit,IDC_BUDDY_EDIT_HYS_HMOON_LOW,
				   pSpin,IDC_UPDOWN_HYS_HMOON_LOW,
				   pStatic,TopLeft,(LPCTSTR) szStatic);		// CHANGES_MVS_2008_CSTRING

		szStatic.Format(_T("Moon"));
		pStatic = &m_StaticHysValue[HYST_OFFSET_LOW][MODE_MOON];
		pEdit = &m_EditBuddyHysValue[HYST_OFFSET_LOW][MODE_MOON];
		pSpin = &m_SpinHysVal[HYST_OFFSET_LOW][MODE_MOON];
		TopLeft.x -= SENS_OFFSET_HYSVAL_X;
		_CreateSpin(pEdit,IDC_BUDDY_EDIT_HYS_MOON_LOW,
				   pSpin,IDC_UPDOWN_HYS_MOON_LOW,
				   pStatic,TopLeft,(LPCTSTR) szStatic);		// CHANGES_MVS_2008_CSTRING
	}

	{
		CSize szControlSize(SENS_GROUP_HYSOFF_SIZE_X,SENS_GROUP_HYSOFF_SIZE_Y);
		CPoint TopLeft(SENS_TOPLEFT_GROUP_HYSOFF_X,SENS_TOPLEFT_GROUP_HYSOFF_Y);

		szStatic.Format(_T("Hysterisis Offset values"));
		m_BtnGroup3.Create((LPCTSTR) szStatic,				// CHANGES_MVS_2008_CSTRING
						  WS_VISIBLE | WS_CHILD | BS_GROUPBOX,
						  CRect(TopLeft,szControlSize),
						  this,
						  IDC_GROUPBOX_HYSVALUES_LOW);
	
		m_BtnGroup3.SetFont(GetFont());

		TopLeft.x = SENS_TOPLEFT_HYSOFF_HIGH_X;
		TopLeft.y = SENS_TOPLEFT_HYSOFF_HIGH_Y;
		_CreateSpin(&m_EditBuddyHysOffset[0],IDC_BUDDY_EDIT_HYSOFFSET_HIGH,
				   &m_SpinHysOffset[0],IDC_UPDOWN_HYSOFFSET_HIGH,
				   &m_StaticHysOffset[0],TopLeft,(LPCTSTR) szHystOffsetHigh);		// CHANGES_MVS_2008_CSTRING
		
		TopLeft.x += SENS_OFFSET_HYSVAL_X;
		_CreateSpin(&m_EditBuddyHysOffset[1],IDC_BUDDY_EDIT_HYSOFFSET_LOW,
				   &m_SpinHysOffset[1],IDC_UPDOWN_HYSOFFSET_LOW,
				   &m_StaticHysOffset[1],TopLeft,(LPCTSTR) szHystOffsetLow);		// CHANGES_MVS_2008_CSTRING		
	}

	{
		/* TM 20120402: Nieuwe control om de offset waarde op de gemeten waarde */

		CPoint TopLeft(SENS_TOPLEFT_OFFSET_X, SENS_TOPLEFT_OFFSET_Y);
		CreateSpin(&m_EditBuddyOffsetMeasuredVal,
				    IDC_BUDDY_EDIT_OFFSET_MEASUREDVAL,
				    &m_SpinOffsetMeasuredVal,
				    IDC_UPDOWN_OFFSET_MEASUREDVAL,				    
					TopLeft);

		// Sedert V16.00: Groupbox ipv een static text.

		CPoint topLeftGroupBox;
		topLeftGroupBox.x = 0;
		topLeftGroupBox.y = SENS_TOPLEFT_OFFSET_Y - SENS_STATIC_Y_SIZE;

		CSize szControlSize(SENS_GROUP_SIZE_X,45);
		szStatic.Format(_T("Measured value offset"));
		m_BtnGroupOffsetMeasuredValue.Create((LPCTSTR) szStatic,				
						  WS_VISIBLE | WS_CHILD | BS_GROUPBOX,
						  CRect(topLeftGroupBox,szControlSize),
						  this,
						  IDC_BUTTON_GROUP_OFFSET_MEASUREDVALUE);
		m_BtnGroupOffsetMeasuredValue.SetFont(GetFont());		
	}

	{
		const CSize ControlSpinSize(90,20);
		CPoint TopLeft(SENS_TOPLEFT_SAMPLE_X,SENS_TOPLEFT_SAMPLE_Y);
		CreateSpin(&m_EditBuddySample,IDC_BUDDY_EDIT_SAMPLE_INTERVAL,
				   &m_SpinSample,IDC_UPDOWN_SAMPLE_INTERVAL,TopLeft,
				   ControlSpinSize);
		m_EditBuddySample.ModifyStyle(SS_RIGHT|SS_CENTER, SS_LEFT);

		CPoint topLeftGroupBox;
		topLeftGroupBox.x = 140;
		topLeftGroupBox.y = SENS_TOPLEFT_OFFSET_Y - SENS_STATIC_Y_SIZE;
				
		const CSize szControlSize(SENS_GROUP_SIZE_X,45);
		szStatic.Format(_T("Sample interval"));
		m_BtnGroupSampleInterval.Create((LPCTSTR) szStatic,				
						  WS_VISIBLE | WS_CHILD | BS_GROUPBOX,
						  CRect(topLeftGroupBox,szControlSize),
						  this,
						  IDC_BUTTON_GROUP_SAMPLEINTERVAL);
		m_BtnGroupSampleInterval.SetFont(GetFont());	
	}
	
	{
		CPoint TopLeft(SENS_TOPLEFT_COMBO_X,SENS_TOPLEFT_COMBO_Y);
		CSize szControlSize(SENS_COMBO_SIZE_X,SENS_COMBO_SIZE_Y);

		/* 
		 * TM 20120402: Combobox niet zichtbaar zetten.
		 * In de plaats van de combobox worden de controls gezet om de offset op
		 * de gemeten waarde.
		 */
		m_CmbSensType.Create(/*WS_VISIBLE |*/ WS_CHILD | WS_DISABLED | CBS_DROPDOWN,
							 CRect(TopLeft,szControlSize),
							 this,
							 IDC_COMBOBOX_SENSTYPE);
		
		m_CmbSensType.SetFont(GetFont());
	
	}

	{
		CPoint TopLeft(SENS_TOPLEFT_COMBO_SENSPROFILE_X,getBtnRestoreDefaultsPosY());
		CSize szControlSize(125,100);	
		m_CmbSetProfile.Create(WS_VISIBLE | WS_CHILD | CBS_DROPDOWN ,
							   CRect(TopLeft,szControlSize),
							   this,
							   IDC_COMBOBOX_SENSPROFILE);		
		m_CmbSetProfile.SetFont(GetFont());	
	}
}

void CDlgSensPropChild::InitControlProperties()
{
	m_CmbSensType.AddString("Temperature °C");
	m_CmbSensType.AddString("Light Lux");
	m_CmbSensType.AddString("Current Ampere");
			
	m_CmbSetProfile.InsertString(0,"Sensor Profile");
	m_CmbSetProfile.InsertString(1,"Only Heating");
	m_CmbSetProfile.InsertString(2,"Only Cooling");
	m_CmbSetProfile.InsertString(3,"Heating+Cooling");

	m_CmbSetProfile.SetItemData(0,SENS_PROFILE_CUSTOM);
	m_CmbSetProfile.SetItemData(1,SENS_PROFILE_HEATING);
	m_CmbSetProfile.SetItemData(2,SENS_PROFILE_COOLING);
	m_CmbSetProfile.SetItemData(3,SENS_PROFILE_BOTH);	
}

/////////////////////////////////////////////////////////////////////////////

void CDlgSensPropChild::CreateSpin(
	CEdit *pEdit, 
	UINT EditIDD, 
	CSpinButtonCtrl *pSpin, 
	UINT SpinIDD, 
	CPoint TopLeft,
	CSize size)
{
	if (1)
	{
		CSize szControlSize(15,20);
		pSpin->Create(WS_CHILD | WS_VISIBLE | WS_BORDER | UDS_ARROWKEYS,
					  CRect(TopLeft,szControlSize),
					  this,
					  SpinIDD);
	}

	if (1)
	{	
		TopLeft.x += 20;
		pEdit->Create(WS_CHILD |WS_VISIBLE | WS_BORDER | ES_CENTER ,
					  CRect(TopLeft,size),
					  this,
					  EditIDD);
	}
			
	pEdit->SetFont(GetFont());
	pSpin->SetFont(GetFont());
}


void CDlgSensPropChild::_CreateSpin(
	CEdit *pEdit, 
	UINT EditIDD, 
	CSpinButtonCtrl *pSpin, 
	UINT SpinIDD, 
	CStatic *pStatic, 
	CPoint TopLeft, 
	CString szText)
{
	CSize szControlSize(15,20);
	
	pSpin->Create(WS_CHILD | WS_VISIBLE | WS_BORDER | UDS_ARROWKEYS,
				  CRect(TopLeft,szControlSize),
				  this,
				  SpinIDD);
	
	TopLeft.y -= SENS_STATIC_Y_SIZE;
	szControlSize.cx = SENS_STATIC_X_SIZE-20;
	szControlSize.cy = SENS_STATIC_Y_SIZE;
	
	pStatic->Create(szText,
				    WS_CHILD | WS_VISIBLE | SS_LEFT,
					CRect(TopLeft,szControlSize),
					this);

	TopLeft.y += SENS_STATIC_Y_SIZE;
	TopLeft.x += 20;	
	szControlSize.cx = SENS_EDIT_BUDDY_X_SIZE-10; 
	szControlSize.cy = SENS_EDIT_BUDDY_Y_SIZE;

	pEdit->Create(WS_CHILD |WS_VISIBLE | WS_BORDER | ES_CENTER ,
				  CRect(TopLeft,szControlSize),						
				  this,
				  EditIDD);		

	pEdit->SetFont(GetFont());	
	pStatic->SetFont(GetFont());
	pSpin->SetFont(GetFont());
}

void CDlgSensPropChild::_CreateSpin2(CEdit *pEdit, UINT EditIDD, CSpinButtonCtrl *pSpin, 
	UINT SpinIDD, CStatic *pStatic, CPoint TopLeft, CString szText) 
{
	{
		const CSize size(25,SENS_STATIC_Y_SIZE);
		pStatic->Create(szText,
					    WS_CHILD | WS_VISIBLE | SS_LEFT,
						CRect(TopLeft,size),
						this);
	}


	CSize szControlSize(15,20);
	{
		TopLeft.x += 26;		
		pSpin->Create(WS_CHILD | WS_VISIBLE | WS_BORDER | UDS_ARROWKEYS,
					  CRect(TopLeft,szControlSize),
					  this,
					  SpinIDD);
	}

	{
		//TopLeft.y += SENS_STATIC_Y_SIZE;
		TopLeft.x += 20;	
		szControlSize.cx = SENS_EDIT_BUDDY_X_SIZE-10; 
		szControlSize.cy = SENS_EDIT_BUDDY_Y_SIZE;

		pEdit->Create(WS_CHILD |WS_VISIBLE | WS_BORDER | ES_CENTER ,
					  CRect(TopLeft,szControlSize),						
					  this,
					  EditIDD);		
	}

	CFont* const pFont = GetFont();
	if (0 != pFont) {
		pEdit->SetFont(pFont);	
		pStatic->SetFont(pFont);
		pSpin->SetFont(pFont);
	}
}

void CDlgSensPropChild::UpdateControls(void)
{
	PostMessage(WM_UPDATE_CONTROLS);
}


void CDlgSensPropChild::CheckHysValBorders(BYTE *pValue,BOOL fDirection)
{
	// Oppassen:
	// Wanneer we CSpinButtonCtrl::SetRange( ) aanroepen dan is de fDirection omgekeerd.

	if (fDirection == FALSE) 
	{
		if (*pValue <= SENS_HYSVAL_MAXVALUE - SENS_HYSVAL_STEP) 
			*pValue += SENS_HYSVAL_STEP;
	}
	else 
	{
		if (*pValue >= SENS_HYSVAL_MINVALUE + SENS_HYSVAL_STEP)  
			*pValue -= SENS_HYSVAL_STEP;	
	}
}

void CDlgSensPropChild::CheckHysOffsetBorders(BYTE *pValue,BOOL fDirection)
{
	// Oppassen:
	// Wanneer we CSpinButtonCtrl::SetRange( ) aanroepen dan is de fDirection omgekeerd.

	if (fDirection == FALSE) 
	{
		if (*pValue <= SENS_HYSOFFSET_MAXVALUE - SENS_HYSOFFSET_STEP) 
			*pValue += SENS_HYSOFFSET_STEP;
	}
	else 
	{
		if (*pValue >= SENS_HYSOFFSET_MINVALUE + SENS_HYSOFFSET_STEP)  
			*pValue -= SENS_HYSOFFSET_STEP;	
	}
}



void CDlgSensPropChild::CheckOffsetMeasuredValBorders(short *psValue,BOOL fDirection)	/* TM20120402 */
{
	// Oppassen:
	// Wanneer we CSpinButtonCtrl::SetRange( ) aanroepen dan is de fDirection omgekeerd.

	if (fDirection == FALSE)			
	{
		if (*psValue <= (SENS_OFFSETMEASUREDVAL_MAX - SENS_OFFSETMEASUREDVAL_STEP))
			*psValue += SENS_OFFSETMEASUREDVAL_STEP;
	}
	else
	{
		if (*psValue >= SENS_OFFSETMEASUREDVAL_MIN + SENS_OFFSETMEASUREDVAL_STEP)  
			*psValue -= SENS_OFFSETMEASUREDVAL_STEP;	
	}
}

void CDlgSensPropChild::CheckSampleTimeBorders(BYTE* pValue,BOOL fDirection) /* TM20120402 */
{
	// Oppassen:
	// Wanneer we CSpinButtonCtrl::SetRange( ) aanroepen dan is de fDirection omgekeerd.

	if (fDirection == FALSE)			
	{
		if (*pValue <= SENS_INTERVAL_MAXVALUE - 1) 
			*pValue += 1;
	}
	else 
	{
		if (*pValue >= SENS_INTERVAL_MINVALUE + 1)  
			*pValue -= 1;	
	}
}

void CDlgSensPropChild::checkSetpointRangeValue(short* psValue,BOOL fDirection)
{
	enum { STEP = 5 };

	if (FALSE==fDirection)
	{
		if (*psValue<=TEMP_MAX-STEP)
			*psValue+=STEP;		
	}
	else 
	{
		if (*psValue>=STEP)
			*psValue-=STEP;
	}
}

void CDlgSensPropChild::OnUpDownHysValueHighSun(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	BOOL fDirection;
	
	if (pNMUpDown->iDelta > 0) fDirection = TRUE;
	else fDirection = FALSE;

	if (1)
	{
		const int OFFSET = HYST_OFFSET_HIGH;
		const int MODE = MODE_SUN;
		
		BYTE bTemp = m_bindingSensUnit.GetHysValues(OFFSET,MODE);
		CheckHysValBorders(&bTemp,fDirection);
		m_bindingSensUnit.SetHysValues(OFFSET,MODE,bTemp);
	}

	UpdateControls();
	*pResult = 0;
	m_fChangesMade = TRUE;
}

void CDlgSensPropChild::OnUpDownHysValueHighHSun(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	BOOL fDirection;
	
	if (pNMUpDown->iDelta > 0) fDirection = TRUE;
	else fDirection = FALSE;

	if (1)
	{
		const int OFFSET = HYST_OFFSET_HIGH;
		const int MODE = MODE_HSUN;
		
		BYTE bTemp = m_bindingSensUnit.GetHysValues(OFFSET,MODE);
		CheckHysValBorders(&bTemp,fDirection);
		m_bindingSensUnit.SetHysValues(OFFSET,MODE,bTemp);
	}
	
	UpdateControls();
	*pResult = 0;
	m_fChangesMade = TRUE;
}

void CDlgSensPropChild::OnUpDownHysValueHighMoon(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	BOOL fDirection;
	
	if (pNMUpDown->iDelta > 0) fDirection = TRUE;
	else fDirection = FALSE;

	if (1)
	{
		const int OFFSET = HYST_OFFSET_HIGH;
		const int MODE = MODE_MOON;
		
		BYTE bTemp = m_bindingSensUnit.GetHysValues(OFFSET,MODE);
		CheckHysValBorders(&bTemp,fDirection);
		m_bindingSensUnit.SetHysValues(OFFSET,MODE,bTemp);
	}

	UpdateControls();
	*pResult = 0;
	m_fChangesMade = TRUE;
}

void CDlgSensPropChild::OnUpDownHysValueHighHMoon(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	BOOL fDirection;
	
	if (pNMUpDown->iDelta > 0) fDirection = TRUE;
	else fDirection = FALSE;

	if (1)
	{
		const int OFFSET = HYST_OFFSET_HIGH;
		const int MODE = MODE_HMOON;
		
		BYTE bTemp = m_bindingSensUnit.GetHysValues(OFFSET,MODE);
		CheckHysValBorders(&bTemp,fDirection);
		m_bindingSensUnit.SetHysValues(OFFSET,MODE,bTemp);
	}

	UpdateControls();
	*pResult = 0;
	m_fChangesMade = TRUE;
}

void CDlgSensPropChild::OnUpDownHysValueLowSun(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	BOOL fDirection;
	
	if (pNMUpDown->iDelta > 0) fDirection = TRUE;
	else fDirection = FALSE;

	if (1)
	{
		const int OFFSET = HYST_OFFSET_LOW;
		const int MODE = MODE_SUN;
		
		BYTE bTemp = m_bindingSensUnit.GetHysValues(OFFSET,MODE);
		CheckHysValBorders(&bTemp,fDirection);
		m_bindingSensUnit.SetHysValues(OFFSET,MODE,bTemp);
	}

	UpdateControls();
	*pResult = 0;
	m_fChangesMade = TRUE;
}

void CDlgSensPropChild::OnUpDownHysValueLowHSun(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	BOOL fDirection;
	
	if (pNMUpDown->iDelta > 0) fDirection = TRUE;
	else fDirection = FALSE;

	if (1)
	{
		const int OFFSET = HYST_OFFSET_LOW;
		const int MODE = MODE_HSUN;
		
		BYTE bTemp = m_bindingSensUnit.GetHysValues(OFFSET,MODE);
		CheckHysValBorders(&bTemp,fDirection);
		m_bindingSensUnit.SetHysValues(OFFSET,MODE,bTemp);
	}

	UpdateControls();
	*pResult = 0;
	m_fChangesMade = TRUE;
}

void CDlgSensPropChild::OnUpDownHysValueLowMoon(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	BOOL fDirection;
	
	if (pNMUpDown->iDelta > 0) fDirection = TRUE;
	else fDirection = FALSE;

	if (1)
	{
		const int OFFSET = HYST_OFFSET_LOW;
		const int MODE = MODE_MOON;
		
		BYTE bTemp = m_bindingSensUnit.GetHysValues(OFFSET,MODE);
		CheckHysValBorders(&bTemp,fDirection);
		m_bindingSensUnit.SetHysValues(OFFSET,MODE,bTemp);
	}

	UpdateControls();
	*pResult = 0;
	m_fChangesMade = TRUE;
}

void CDlgSensPropChild::OnUpDownHysValueLowHMoon(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	BOOL fDirection;
	
	if (pNMUpDown->iDelta > 0) fDirection = TRUE;
	else fDirection = FALSE;

	if (1)
	{
		const int OFFSET = HYST_OFFSET_LOW;
		const int MODE = MODE_HMOON;
		
		BYTE bTemp = m_bindingSensUnit.GetHysValues(OFFSET,MODE);
		CheckHysValBorders(&bTemp,fDirection);
		m_bindingSensUnit.SetHysValues(OFFSET,MODE,bTemp);
	}

	UpdateControls();
	*pResult = 0;
	m_fChangesMade = TRUE;
}

//////

void CDlgSensPropChild::OnUpDownHysOffsetHigh(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	BOOL fDirection;
	
	if (pNMUpDown->iDelta > 0) fDirection = TRUE;
	else fDirection = FALSE;

	if (1)
	{
		const int OFFSET = HYST_OFFSET_HIGH;	
		
		BYTE bTemp = m_bindingSensUnit.GetHysOffsetValues(OFFSET);
		CheckHysOffsetBorders(&bTemp,fDirection);
		m_bindingSensUnit.SetHysOffsetValues(OFFSET,bTemp);
	}
	
	UpdateControls();
	*pResult = 0;
	m_fChangesMade = TRUE;
}

void CDlgSensPropChild::OnUpDownHysOffsetLow(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	BOOL fDirection;
	
	if (pNMUpDown->iDelta > 0) fDirection = TRUE;
	else fDirection = FALSE;

	if (1)
	{
		const int OFFSET = HYST_OFFSET_LOW;	
		
		BYTE bTemp = m_bindingSensUnit.GetHysOffsetValues(OFFSET);
		CheckHysOffsetBorders(&bTemp,fDirection);
		m_bindingSensUnit.SetHysOffsetValues(OFFSET,bTemp);
	}

	UpdateControls();
	*pResult = 0;
	m_fChangesMade = TRUE;
}

void CDlgSensPropChild::OnUpdownSampleInterval(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	BOOL fDirection;
	
	if (pNMUpDown->iDelta > 0) fDirection = TRUE;
	else fDirection = FALSE;

	if (1)
	{
		BYTE bValue = m_bindingSensUnit.GetSampleTime();
		CheckSampleTimeBorders(&bValue,fDirection);
		m_bindingSensUnit.SetSampleTime(bValue);
	}

	UpdateControls();
	*pResult = 0;
	m_fChangesMade = TRUE;
}

void CDlgSensPropChild::OnUpdownMeasuredval(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	BOOL fDirection;
	
	if (pNMUpDown->iDelta > 0) fDirection = TRUE;
	else fDirection = FALSE;

	if (1)
	{
		short sTemp = m_bindingSensUnit.GetMeasuredValueOffset();
		CheckOffsetMeasuredValBorders(&sTemp,fDirection);
		m_bindingSensUnit.SetMeasuredValueOffset(sTemp);
	}

	UpdateControls();
	*pResult = 0;
	m_fChangesMade = TRUE;
}

void CDlgSensPropChild::OnUpdownSetpointRangeMin(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_UPDOWN* const pNMUpDown = (NM_UPDOWN*)pNMHDR;
	const BOOL fDirection = pNMUpDown->iDelta>0 ? TRUE : FALSE;
	const short prevValue = m_bindingSensUnit.getSetpointRangeValueMin();
	short value = prevValue;

	checkSetpointRangeValue(&value,fDirection);

	if (prevValue != value)
	{
		m_bindingSensUnit.setSetpointRangeValueMin(value);
		UpdateControls();	
		m_fChangesMade = TRUE;
	}
	*pResult = 0;
}

void CDlgSensPropChild::OnUpdownSetpointRangeMax(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_UPDOWN* const pNMUpDown = (NM_UPDOWN*)pNMHDR;
	const BOOL fDirection = pNMUpDown->iDelta>0 ? TRUE : FALSE;
	const short prevValue = m_bindingSensUnit.getSetpointRangeValueMax();
	short value = prevValue;

	checkSetpointRangeValue(&value,fDirection);

	if (prevValue != value)
	{
		m_bindingSensUnit.setSetpointRangeValueMax(value);
		UpdateControls();	
		m_fChangesMade = TRUE;
	}
	*pResult = 0;
}
///////////////////////////////////////////////////////////////////////////////

// Zie BUG-0179: Bij het maken van de defaults wordt de laatste waarde gebruikt. 
// vb: Als er 2 sensoren van de smartbox aanwezig zijn.
//     Sensor1: 20sec, sensor2: 23sec.
//     Bij het maken van defaults voor sensor 1 wordt er in de GUI 
//     23sec getoond en wordt er een binding gemaakt voor de 20 seconden.
void CDlgSensPropChild::RestoreDefaults(void)
{
	InitControlsToDefaults();
	UpdateControls();

	m_fChangesMade = TRUE;
}

//////////////////////////////////////////////////////////////////////////////

void CDlgSensPropChild::InitControlsToDefaults(void)
{	
	m_bindingSensUnit.SetToDefaults();
	m_SpinSample.SetPos(m_bindingSensUnit.GetSampleTime());
}

void CDlgSensPropChild::OnButtonResetValues()
{
	for (int i=0;i<2;i++) 
	{
		for (int j=0;j<4;j++) 
		{
			m_bindingSensUnit.SetHysValues(i,j,0);
		}
		
		m_bindingSensUnit.SetHysOffsetValues(i,0);
	}

	UpdateControls();
	m_fChangesMade = TRUE;
}


//////////////////////////////////////////////////////////////////////////////
void CDlgSensPropChild::CreateDefaultButton(void)
{
	CButton* const pBtn = &m_BtnRestoreDefaults;

	// Andere waarde dan de default waarde...
	const CPoint TopLeft(BUTTON_RESTORE_DEFAULTS_OFFSET_X,getBtnRestoreDefaultsPosY());
	const CSize szControlSize(DEFAULT_BUTTON_X_SIZE,DEFAULT_BUTTON_Y_SIZE);

	if (0!=pBtn)
	{
		pBtn->Create("Restore Defaults",
					 WS_CHILD | WS_VISIBLE,
					 CRect(TopLeft,szControlSize),
					 this,
					 IDC_BUTTON_DEFAULTS);		
		
		pBtn->SetFont(GetFont());
	}
}

void CDlgSensPropChild::OnSelectSensProfile()
{		
	SetProfile(m_CmbSetProfile.GetItemData(m_CmbSetProfile.GetCurSel()));			
	UpdateControls();		
	m_BtnRestoreDefaults.SetFocus();
}

void CDlgSensPropChild::SetProfile(int profile)
{
	switch(profile)
	{
		case SENS_PROFILE_HEATING:
		{
			m_bindingSensUnit.SetHysValues(HYST_OFFSET_HIGH,MODE_SUN,0);
			m_bindingSensUnit.SetHysValues(HYST_OFFSET_LOW,MODE_SUN,0);
			m_bindingSensUnit.SetHysValues(HYST_OFFSET_HIGH,MODE_HSUN,0);
			m_bindingSensUnit.SetHysValues(HYST_OFFSET_LOW,MODE_HSUN,0);
			m_bindingSensUnit.SetHysValues(HYST_OFFSET_HIGH,MODE_MOON,0);
			m_bindingSensUnit.SetHysValues(HYST_OFFSET_LOW,MODE_MOON,0);
			m_bindingSensUnit.SetHysValues(HYST_OFFSET_HIGH,MODE_HMOON,0);
			m_bindingSensUnit.SetHysValues(HYST_OFFSET_LOW,MODE_HMOON,0);
			
			m_bindingSensUnit.SetHysOffsetValues(HYST_OFFSET_HIGH,0);
			m_bindingSensUnit.SetHysOffsetValues(HYST_OFFSET_LOW,5);

			m_fChangesMade = TRUE;

		} break;

		case SENS_PROFILE_COOLING:
		{
			m_bindingSensUnit.SetHysValues(HYST_OFFSET_HIGH,MODE_SUN,0);
			m_bindingSensUnit.SetHysValues(HYST_OFFSET_LOW,MODE_SUN,0);
			m_bindingSensUnit.SetHysValues(HYST_OFFSET_HIGH,MODE_HSUN,0);
			m_bindingSensUnit.SetHysValues(HYST_OFFSET_LOW,MODE_HSUN,0);
			m_bindingSensUnit.SetHysValues(HYST_OFFSET_HIGH,MODE_MOON,0);
			m_bindingSensUnit.SetHysValues(HYST_OFFSET_LOW,MODE_MOON,0);
			m_bindingSensUnit.SetHysValues(HYST_OFFSET_HIGH,MODE_HMOON,0);
			m_bindingSensUnit.SetHysValues(HYST_OFFSET_LOW,MODE_HMOON,0);
			
			m_bindingSensUnit.SetHysOffsetValues(HYST_OFFSET_HIGH,5);
			m_bindingSensUnit.SetHysOffsetValues(HYST_OFFSET_LOW,0);

			m_fChangesMade = TRUE;

		} break;
		
		case SENS_PROFILE_BOTH:
		{
			m_bindingSensUnit.SetHysValues(HYST_OFFSET_HIGH,MODE_SUN,0);
			m_bindingSensUnit.SetHysValues(HYST_OFFSET_LOW,MODE_SUN,0);
			m_bindingSensUnit.SetHysValues(HYST_OFFSET_HIGH,MODE_HSUN,50);
			m_bindingSensUnit.SetHysValues(HYST_OFFSET_LOW,MODE_HSUN,50);
			m_bindingSensUnit.SetHysValues(HYST_OFFSET_HIGH,MODE_MOON,100);
			m_bindingSensUnit.SetHysValues(HYST_OFFSET_LOW,MODE_MOON,100);
			m_bindingSensUnit.SetHysValues(HYST_OFFSET_HIGH,MODE_HMOON,150);
			m_bindingSensUnit.SetHysValues(HYST_OFFSET_LOW,MODE_HMOON,150);
			
			m_bindingSensUnit.SetHysOffsetValues(HYST_OFFSET_HIGH,20);
			m_bindingSensUnit.SetHysOffsetValues(HYST_OFFSET_LOW,5);

			m_fChangesMade = TRUE;

		}break;	
	}
}


// Aanpassingen versie 0x0C03:
// ifv de mode wordt het type van delegation toegekend.
void CDlgSensPropChild::DelegationAssignDelegate(int selected)
{
	DWORD dwItemData = 0;
	BYTE bNodeAddress = 0xFF;
	BYTE bUnitAddress = 0xFF;

	const int sel = (selected>=0 ? selected : 0);
	dwItemData = m_CmbDelegation.GetItemData(sel);
	ASSERT(m_CmbDelegation.GetCount()>0);

	bNodeAddress=(BYTE) ((dwItemData>>8)&0xFF);
	bUnitAddress=(BYTE) (dwItemData&0xFF);

	ASSERT(bNodeAddress!=0xFF);
	ASSERT(bUnitAddress!=0xFF);
		
	if ((0xFF!=bNodeAddress)&&(0xFF!=bUnitAddress))
	{
		const mode_t mode = DelegationGetMode();
		switch (mode)
		{
			case MODE_DELEGATE_ATTACH_SUPPORTED:
			case MODE_DELEGATOR_ATTACHED:
			case MODE_DELEGATE_ATTACHED:
			{	
				m_bindingSensUnit.SetDelegationType(CBindingPropertySensUnit::DELEGATION_TYPE_DELEGATE);
			} break;

			case MODE_DELEDATION_REQUESTMEASURE_VALUES_SUPPORTED:	// sedert versie 0x0C03
			case MODE_DELEDATION_REQUESTMEASURE_VALUES_ATTACHED:	// sedert versie 0x0C03
			{
				m_bindingSensUnit.SetDelegationType(CBindingPropertySensUnit::DELEGATION_TYPE_REQUESTVALUE);
			} break;

			default:
			{
				ASSERT(0);	// Kunnen hier niet komen...
			} break;
		} /* switch(mode) */

		m_bindingSensUnit.SetDelegationNodeAddress(bNodeAddress);
		m_bindingSensUnit.SetDelegationUnitAddress(bUnitAddress);
	}
}

void CDlgSensPropChild::OnCheckExternalSensor(void)
{
	if (m_CheckBtnDelegationExtSensor.GetCheck()==0)
	{
		// Moeten hier ook al de eerste uit de combobox selecteren...
		DelegationAssignDelegate(-1);

		m_fChangesMade = TRUE;
	}
	else
	{
		// Clear delegation.
		m_bindingSensUnit.SetDelegationType(CBindingPropertySensUnit::DELEGATION_TYPE_NONE);
		m_bindingSensUnit.SetDelegationNodeAddress(0xFF);
		m_bindingSensUnit.SetDelegationUnitAddress(0xFF);

		m_fChangesMade = TRUE;
	}
	UpdateControls();
}

void CDlgSensPropChild::OnSelectDelegate() {
	int sel = m_CmbDelegation.GetCurSel();
	if (sel>=0) {
		ASSERT(m_CheckBtnDelegationExtSensor.GetCheck()==1);
		DelegationAssignDelegate(sel);	
		m_fChangesMade = TRUE;
	}
}

/*********************************************************************************************************/
// Sedert V16.00: USE_SENSOR_PIDCONTROL	
static BOOL onSelectPIDUnit(CComboBox* const pComboBox,CBindingPropertySensUnit* pUnit,int workingsmode) {

	if (0 == pComboBox || 0 == pUnit) {
		return FALSE;
	}
	const int sel = pComboBox->GetCurSel();
	if (sel<0) {
		return FALSE;
	}

	// Todo...
	const DWORD dwItemData = pComboBox->GetItemData(sel);
	const BYTE bNodeAddress=(BYTE) ((dwItemData>>8)&0xFF);
	const BYTE bUnitAddress=(BYTE) (dwItemData&0xFF);
	pUnit->setPropertyPIDControlUnit(workingsmode,bNodeAddress,bUnitAddress);
	return TRUE;
}

void CDlgSensPropChild::OnSelectPidCoolingUnit(void) {
	const int workingsmode = SENS_PIDCONTROL_WORKINGMODE_COOLING;
	m_fChangesMade = onSelectPIDUnit(&m_CmbPIDControlUnit[workingsmode], &m_bindingSensUnit, workingsmode); 
	if (m_fChangesMade) {
		UpdateControls();
	}
}

void CDlgSensPropChild::OnSelectPidHeatingUnit(void) {
	const int workingsmode = SENS_PIDCONTROL_WORKINGMODE_HEATING;
	m_fChangesMade = onSelectPIDUnit(&m_CmbPIDControlUnit[workingsmode], &m_bindingSensUnit, workingsmode); 
	if (m_fChangesMade) {
		UpdateControls();
	}
}

static int CheckValues(int value,int min,int max,int delta) {
	int retValue = value + delta;
	if (retValue<min) {
		retValue = min;
	}
	else if (retValue>max) {
		retValue = max;
	}
	return retValue;
}

static void updatePIDControlParam(CBindingPropertySensUnit* pUnit,int mode,int param,int delta) {
	int value = pUnit->getPropertyPIDParam(mode,param);
	value = CheckValues(value,0,255,delta);
	pUnit->setPropertyPIDParam(mode,param,value);
}


void CDlgSensPropChild::OnUpDownPidParam(NMHDR* pNMHDR, LRESULT* pResult) {
	NM_UPDOWN* const pNMUpDown = (NM_UPDOWN*)pNMHDR;
	if (0 == pNMUpDown) {
		return ;
	}
	
	const int delta = (pNMUpDown->iDelta * -1);
	const int idFrom = pNMHDR->idFrom;
	switch(idFrom) {
		case IDC_UPDOWN_PIDCOOLING_KP: {
			updatePIDControlParam(&m_bindingSensUnit,SENS_PIDCONTROL_WORKINGMODE_COOLING,SENS_PIDCONTROL_PARAM_KP,delta);
		} break;
		case IDC_UPDOWN_PIDCOOLING_KI: {
			updatePIDControlParam(&m_bindingSensUnit,SENS_PIDCONTROL_WORKINGMODE_COOLING,SENS_PIDCONTROL_PARAM_KI,delta);
		} break;
		case IDC_UPDOWN_PIDCOOLING_KD: {
			updatePIDControlParam(&m_bindingSensUnit,SENS_PIDCONTROL_WORKINGMODE_COOLING,SENS_PIDCONTROL_PARAM_KD,delta);
		} break;
		case IDC_UPDOWN_PIDHEATING_KP: {
			updatePIDControlParam(&m_bindingSensUnit,SENS_PIDCONTROL_WORKINGMODE_HEATING,SENS_PIDCONTROL_PARAM_KP,delta);
		} break;
		case IDC_UPDOWN_PIDHEATING_KI: {
			updatePIDControlParam(&m_bindingSensUnit,SENS_PIDCONTROL_WORKINGMODE_HEATING,SENS_PIDCONTROL_PARAM_KI,delta);
		} break;
		case IDC_UPDOWN_PIDHEATING_KD: {
			updatePIDControlParam(&m_bindingSensUnit,SENS_PIDCONTROL_WORKINGMODE_HEATING,SENS_PIDCONTROL_PARAM_KD,delta);
		} break;
	}
	UpdateControls();
	*pResult = 0;
	m_fChangesMade = TRUE;
}