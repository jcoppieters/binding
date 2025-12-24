#include "stdafx.h"
#include "BindingSoftware.h"
#include "bindingunitsens.h"

#include "NodeMess.h"
#include "BindingConstants.h"
#include "bindingUIElement.h"	// ??

#include "App\Appbindings\appbindingproperty\BindingPropertyStrings.h"
#include "App\Appbindings\appbindingproperty\BindingPropertyUnit.h"	
#include "App\Appbindings\appbindingproperty\BindingPropertyUnitDefault.h"
#include "App\Appbindings\appbindingproperty\BindingPropertyUnitDefaultFactory.h"

using namespace DUOTECNO::BINDINGS::UI;


CBindingSensUnit::CBindingSensUnit(BYTE bBindingUnitType) :
Base(bBindingUnitType)
{ 
	SetInOutType(UNIT_TYPE_INPUT);
	Init();	
}

CBindingSensUnit::CBindingSensUnit(const CBindingUnit &BindingUnit) :
CBindingUnit(BindingUnit)
{
	Init();				
}

CBindingSensUnit::~CBindingSensUnit() { }

// Sedert V16.00: USE_SENSOR_PIDCONTROL

void CBindingSensUnit::setPIDWorkingMode(int workingMode) {
	m_pidWorkingMode = workingMode;
}
int CBindingSensUnit::getPIDWorkingMode( ) const {
	return m_pidWorkingMode;
}

void CBindingSensUnit::setPIDControlUnit(int workingsmode,int nodeAddress,int unitAddress) {
	if (workingsmode>=0 && workingsmode<PIDCONTROL_WORKINGMODE_MAX) {
		this->m_pidControl[workingsmode].m_bNodeAddress = nodeAddress;
		this->m_pidControl[workingsmode].m_bUnitAddress = unitAddress;
	}
}
BOOL CBindingSensUnit::getPIDControlUnit(int workingsmode,int *pNodeAddress,int *pUnitAddress) const {
	if (workingsmode>=0 && workingsmode<PIDCONTROL_WORKINGMODE_MAX) {
		*pNodeAddress = this->m_pidControl[workingsmode].m_bNodeAddress;
		*pUnitAddress = this->m_pidControl[workingsmode].m_bUnitAddress;
		return TRUE;
	}
	return FALSE;
}

void CBindingSensUnit::setPIDControlParams(int workingsmode, int kp,int ki,int kd) {
	if (workingsmode>=0 && workingsmode<PIDCONTROL_WORKINGMODE_MAX) {
		this->m_pidControl[workingsmode].m_params[PIDCONTROL_PARAM_KP] = kp;
		this->m_pidControl[workingsmode].m_params[PIDCONTROL_PARAM_KI] = ki;
		this->m_pidControl[workingsmode].m_params[PIDCONTROL_PARAM_KD] = kd;		
	}
}
BOOL CBindingSensUnit::getPIDControlParams(int workingsmode, int *kp,int *ki,int *kd) const {
	if (workingsmode>=0 && workingsmode<PIDCONTROL_WORKINGMODE_MAX) {
		*kp = this->m_pidControl[workingsmode].m_params[PIDCONTROL_PARAM_KP];
		*ki = this->m_pidControl[workingsmode].m_params[PIDCONTROL_PARAM_KI];
		*kd = this->m_pidControl[workingsmode].m_params[PIDCONTROL_PARAM_KD];
		return TRUE;
	}
	return FALSE;
}

void CBindingSensUnit::InitDelegation(void)
{
	// Sedert versie 0x0900
	SetDelegationType(CBindingSensUnit::DELEGATION_TYPE_NONE);
	SetDelegationNodeAddress(0xFF);
	SetDelegationUnitAddress(0xFF);
}

void CBindingSensUnit::InitSensorExtended(void)
{
	SetMeasuredValueOffset(SENS_OFFSETMEASUREDVAL_DEFAULT);	/* TM20120405 */
	Set_WorkingMode(SENS_WORKING_MODE_AUTO);	/* TM20120917 */
	Set_FanSpeed(SENS_FAN_SPEED_1);				/* TM20120917 */
	Set_SwingMode(0);							/* TM20120917 */	
	Set_Angle(0);								/* TM20120917 */
}

void CBindingSensUnit::InitSensorsOutput(void)
{
	Set_Function(SENS_OBJECT_ATTR_CONTROL_ONOFF);	
	Set_Setpoint(220);
	Set_Mode(SENS_MODE_SUN); 
	Set_State(1);

	setSetpointRangeValueMin(SENS_DEFAULT_SP_RANGE_MIN);
	setSetpointRangeValueMax(SENS_DEFAULT_SP_RANGE_MAX);
}

void CBindingSensUnit::InitSensorInput(void)
{
	Set_EventState(SENS_EVENT_ON);
	Set_Event(EV_HEATING);
}

void CBindingSensUnit::InitSensorUsingDefaults(void)
{
	const CBindingPropertyUnitSensDefault* pDefaults = 0;

	if (GetNodeAddress() == 0xFF)
	{
		pDefaults = static_cast<const CBindingPropertyUnitSensDefault* const>
			(CBindingPropertyUnitDefaultFactory::Instance()->
				Create(UNIT_TYPE_SENS));
	}
	else
	{
		// TM 20130328:
		// Structuur gebruiken om de params mee te geven aan de factory.
		// Binding unit type meegeven omdat de unit mogelijk nog niet aanwezig is 
		// in de nodedatabase.
		const CBindingPropertyUnitDefaultFactory::CUnitInfo unitInfo =
		{
			this->GetNodeAddress(),
			this->GetUnitAddress(),
			this->GetBindingUnitType()			// Binding unit type.
		};

		pDefaults = static_cast<const CBindingPropertyUnitSensDefault* const>
			(CBindingPropertyUnitDefaultFactory::Instance()->
				Create(UNIT_TYPE_SENS, unitInfo));
	}
	
	ASSERT( pDefaults != 0 );	

	if (pDefaults != 0)
	{
		Set_SampleTime(pDefaults->GetSampleTime());

		for (int i=0;i<4;i++)
		{
			Set_HysLowValue(i,pDefaults->GetHysLowValue(i));
			Set_HysHighValue(i,pDefaults->GetHysHighValue(i));
		}		
		Set_HysLowOffsetValue(pDefaults->GetHysOffsetLowValue());
		Set_HysHighOffsetValue(pDefaults->GetHysOffsetHighValue());

		setSetpointRangeValueMin(pDefaults->getSetpointRangeMin());	
		setSetpointRangeValueMax(pDefaults->getSetpointRangeMax());


		// Sedert V16.00: USE_SENSOR_PIDCONTROL		
		this->setPIDWorkingMode(SENS_PIDCONTROL_WORKINGMODE_NONE);	// Default not used...
		this->setPIDControlUnit(PIDCONTROL_WORKINGMODE_COOLING,0xff,0xff);
		this->setPIDControlParams(PIDCONTROL_WORKINGMODE_COOLING, 
								  pDefaults->getPIDControlParam(PIDCONTROL_WORKINGMODE_COOLING,PIDCONTROL_PARAM_KP), 
								  pDefaults->getPIDControlParam(PIDCONTROL_WORKINGMODE_COOLING,PIDCONTROL_PARAM_KI),
								  pDefaults->getPIDControlParam(PIDCONTROL_WORKINGMODE_COOLING,PIDCONTROL_PARAM_KD));

		this->setPIDControlUnit(PIDCONTROL_WORKINGMODE_HEATING,0xff,0xff);
		this->setPIDControlParams(PIDCONTROL_WORKINGMODE_HEATING, 
								  pDefaults->getPIDControlParam(PIDCONTROL_WORKINGMODE_HEATING,PIDCONTROL_PARAM_KP), 
								  pDefaults->getPIDControlParam(PIDCONTROL_WORKINGMODE_HEATING,PIDCONTROL_PARAM_KI),
								  pDefaults->getPIDControlParam(PIDCONTROL_WORKINGMODE_HEATING,PIDCONTROL_PARAM_KD));
		
	} /* if (pDefaults != 0) */
}

void CBindingSensUnit::Init(void)		
{
	InitSensorInput( );
	InitSensorsOutput( );
	InitSensorExtended( );
	InitDelegation( );
	InitSensorUsingDefaults( );
}

BOOL CBindingSensUnit::UpgradeIoType()
{
	const BYTE bMessageCode = BindingData.Get_MessageCode();

	if ((bMessageCode == EV_UNITCONTROLTOGGLE) || 
		(bMessageCode == EV_UNITCONTROLPULSTOGGLE) ||
		(bMessageCode == EV_UNITCONTROLPULS)) 
	{
		SetInOutType(UNIT_TYPE_INPUT);
		return TRUE;
	}
	else if (bMessageCode == FC_UNITSENSSET)
	{
		SetInOutType(UNIT_TYPE_OUTPUT);
		return TRUE;
	}
	return FALSE;
}

BOOL CBindingSensUnit::ConvertToBindingData(void)
{
	// unsigned short usSetPoint;
	
	BindingData.Clear();
	
	const BYTE bInOutType = GetInOutType();
	switch(bInOutType)
	{
		case UNIT_TYPE_INPUT:
		{
			switch(Get_Event())
			{
				case EV_HEATING:
				{
					// Zie BUG-0152:
					// Heb het event SENS_HEATING_OFF en SENS_HEATING_ON
					// omgewisseld.
					BindingData.Set_MessageCode(EV_UNITCONTROLTOGGLE);				

					if (Get_EventState() == SENS_EVENT_ON) 
						BindingData.Set_BindingFlags(SENS_HEATING_OFF);
					else 
						BindingData.Set_BindingFlags(SENS_HEATING_ON);

				} return TRUE;

				case EV_COOLING:
				{
					// Zie BUG-0152
					// Heb het event SENS_HEATING_OFF en SENS_HEATING_ON
					// omgewisseld.
					BindingData.Set_MessageCode(EV_UNITCONTROLPULSTOGGLE);

					if (Get_EventState() == SENS_EVENT_ON) 
						BindingData.Set_BindingFlags(SENS_COOLING_OFF);
					else 
						BindingData.Set_BindingFlags(SENS_COOLING_ON);

				} return TRUE;

				default:
					ASSERT(0);
				return FALSE;
			}
		} break;

		case UNIT_TYPE_OUTPUT:
		{
			BindingData.Clear();
			BindingData.Set_MessageCode(FC_UNITSENSSET);

			const BYTE bMethodData = Get_Function();
			BindingData.Set_MethodData(bMethodData);

			switch(bMethodData)
			{
				case SENS_OBJECT_USE_SELECTOR_DATA :
				case SENS_OBJECT_ATTR_CURRENT_MEASURE:
				case SENS_OBJECT_ATTR_STATE:
				{
					ASSERT(0);
				} return FALSE;

				case SENS_OBJECT_ATTR_WORKING_SET_POINT:
				{
					const unsigned short usSetPoint = Get_SetPoint();
					BindingData.Set_Length(4);
					BindingData.Set_Data(1,usSetpoint/256);
					BindingData.Set_Data(2,usSetpoint%256);
					BindingData.Set_Data(3,Get_Mode());					
				} return TRUE;

				case SENS_OBJECT_ATTR_HYST_VALUES:
				{
					BindingData.Set_Length(4);
					BindingData.Set_Data(1,Get_HysHighValue(Get_Mode()));
					BindingData.Set_Data(2,Get_HysLowValue(Get_Mode()));
					BindingData.Set_Data(3,Get_Mode());	
				} return TRUE;

				case SENS_OBJECT_ATTR_CONTROL_ONOFF:
				{
					BindingData.Set_Length(2);
					BindingData.Set_Data(1,Get_State() & 0x01);
				} return TRUE;

				case SENS_OBJECT_METHOD_FSINC_DEC:
				case SENS_OBJECT_METHOD_INC:
				case SENS_OBJECT_METHOD_DEC:
				case SENS_OBJECT_METHOD_FSONOFF:
				{
					BindingData.Set_Length(1);
				} return TRUE;

				case SENS_OBJECT_ATTR_SAMPLE_INTERVAL:
				{
					BindingData.Set_Length(2);
					BindingData.Set_Data(1,Get_SampleTime());
				} return TRUE;

				case SENS_OBJECT_ATTR_CONFIG:
					//BindingData.Set_Length(2);
					//BindingData.Set_Data(1,Get_Config());
				return TRUE;

				case SENS_OBJECT_ATTR_HYST_OFFSETS:
				{
					BindingData.Set_Length(3);
					BindingData.Set_Data(1,Get_HysHighOffsetValue());
					BindingData.Set_Data(2,Get_HysLowOffsetValue());
				} return TRUE;

				case SENS_OBJECT_ATTR_PRESET:
				{
					BindingData.Set_Length(2);
					BindingData.Set_Data(1,Get_Mode());
				} return TRUE;

				case SENS_OBJECT_ATTR_MEASUREDOFFSET:	
				{
					const short sOffsetValue = GetMeasuredValueOffset( );
					BindingData.Set_Length(3);
					BindingData.Set_Data(1,((sOffsetValue&0xFF00)>>8));
					BindingData.Set_Data(2,(sOffsetValue&0xFF));
				} return TRUE;

				case SENS_OBJECT_ATTR_WORKING_MODE:		
				{
					BindingData.Set_Length(2);
					BindingData.Set_Data(1,Get_WorkingMode());
				} return TRUE;

				case SENS_OBJECT_ATTR_FAN_SPEED:			
				{
					BindingData.Set_Length(2);
					BindingData.Set_Data(1,Get_FanSpeed());
				} return TRUE;

				case SENS_OBJECT_ATTR_SWING_ANGLE:		
				{
					const unsigned short angle = Get_Angle( );
					BindingData.Set_Length(3);
					BindingData.Set_Data(1,(angle & 0xFF00)>>8);
					BindingData.Set_Data(2,(angle & 0x00FF));
				} return TRUE;

				case SENS_OBJECT_ATTR_SWING_MODE:		
				{
					BindingData.Set_Length(2);
					BindingData.Set_Data(1,Get_SwingMode());
				} return TRUE;

				// sedert versie 0x0900
				case SENS_OBJECT_ATTR_DELEGATE:
				case SENS_OBJECT_ATTR_DELEGATOR:	
				case SENS_OBJECT_ATTR_CFG_MEASURESENSOR:	// Sedert versie 0x0C03
				{
					BindingData.Set_Length(3);
					BindingData.Set_Data(1,GetDelegationNodeAddress());
					BindingData.Set_Data(2,GetDelegationUnitAddress());
				} return TRUE;

				case SENS_OBJECT_ATTR_SP_RANGE:
				{
					const short min = getSetpointRangeValueMin();
					const short max = getSetpointRangeValueMax();

					BindingData.Set_Length(5);
					BindingData.Set_Data(1,min/256);
					BindingData.Set_Data(2,min%256);
					BindingData.Set_Data(3,max/256);
					BindingData.Set_Data(4,max%256);
				} return TRUE;
				
				case SENS_OBJECT_ATTR_PIDCONTROLUNIT: 
				{
					// Sedert V16.02: USE_SENSOR_PIDCONTROL
					const int workingMode = this->getPIDWorkingMode();
					if (SENS_PIDCONTROL_WORKINGMODE_COOLING == workingMode || 
						SENS_PIDCONTROL_WORKINGMODE_HEATING == workingMode) 
					{
						int nodeAddress = 0xff;
						int unitAddress = 0xff;
						
						if (this->getPIDControlUnit(workingMode,&nodeAddress,&unitAddress)) {
							// const BYTE bUnitType = (0xff == nodeAddress && 0xff == unitAddress ? 0xff : UNIT_TYPE_DIM);
							const BYTE bUnitType = UNIT_TYPE_DIM;		// Oppassen; Moet altijd dit type zijn !!!

							BindingData.Set_Length(5);
							BindingData.Set_Data(1,workingMode);
							BindingData.Set_Data(2,bUnitType);
							BindingData.Set_Data(3,nodeAddress);
							BindingData.Set_Data(4,unitAddress);
							return TRUE;
						}
					}
				} return FALSE;		

				// zie USE_SENSOR_PIDCONTROL
				case SENS_OBJECT_ATTR_PIDCONTROLPARAMS:
				{
					const int workingMode = this->getPIDWorkingMode();
					if (SENS_PIDCONTROL_WORKINGMODE_COOLING == workingMode || 
						SENS_PIDCONTROL_WORKINGMODE_HEATING == workingMode) 
					{
						int kp = 0;
						int ki = 0;
						int kd = 0;
						if (this->getPIDControlParams(workingMode,&kp,&ki,&kd)) {						
							BindingData.Set_Length(5);
							BindingData.Set_Data(1,workingMode);
							BindingData.Set_Data(2,kp);
							BindingData.Set_Data(3,ki);
							BindingData.Set_Data(4,kd);
							return TRUE;
						}
					}
				} return FALSE;

				default:
				{
					ASSERT(0);
				} return FALSE;
			}
		} break;

		default:
			ASSERT(0);
		return FALSE;
	}
	return FALSE;
}


BOOL CBindingSensUnit::ConvertFromBindingData(void)
{
	Init();		//TM 20040701

	const BYTE bMessageCode = BindingData.Get_MessageCode();
	BYTE bBindingFlags;
	
	if (GetInOutType() == UNIT_TYPE_INPUT) 
	{ 		
		switch(bMessageCode) 
		{
			case EV_UNITCONTROLTOGGLE:
			{
				// Zie ook BUG-0152.
				// Binding vlaggen voor heating zijn 0 of 1:
				// Is altijd SENS_EVENT_OFF

				Set_Event(EV_HEATING);
				bBindingFlags = BindingData.Get_BindingFlags();

				if (bBindingFlags & 0x80) 
					Set_EventState(SENS_EVENT_ON);
				else 
					Set_EventState(SENS_EVENT_OFF);

			} return TRUE;

			case EV_UNITCONTROLPULSTOGGLE:
			{
				// Zie ook BUG-0152.
				// Binding vlaggen voor cooling zijn 2 of 3
				// Is altijd SENS_EVENT_OFF

				Set_Event(EV_COOLING);
				bBindingFlags = BindingData.Get_BindingFlags();

				if (bBindingFlags & 0x80) 
					Set_EventState(SENS_EVENT_ON);
				else 
					Set_EventState(SENS_EVENT_OFF);

			} return TRUE;

			default:
				ASSERT(0);
			return FALSE;		
		}		
		return FALSE;
	}
	else 
	{
		if (bMessageCode != FC_UNITSENSSET) 
		{
			ASSERT(0);
			return FALSE;
		}

		const BYTE bMethodData = BindingData.Get_MethodData();
		Set_Function(bMethodData);
		
		const BYTE bLength = BindingData.Get_Length();

		switch(bMethodData) 
		{
			case SENS_OBJECT_USE_SELECTOR_DATA :
			case SENS_OBJECT_ATTR_CURRENT_MEASURE:
			case SENS_OBJECT_ATTR_STATE:
				ASSERT(0);
			return FALSE;

			case SENS_OBJECT_ATTR_WORKING_SET_POINT:
			{
				unsigned short usSetpoint;

				if (bLength != 4) return FALSE;
				usSetpoint = BindingData.Get_Data(1) * 256;
				usSetpoint += BindingData.Get_Data(2);
				Set_Setpoint(usSetpoint);
				Set_Mode(BindingData.Get_Data(3));
			} return TRUE;

			case SENS_OBJECT_ATTR_HYST_VALUES:
			{
				if (bLength !=4) return FALSE;			
				Set_Mode(BindingData.Get_Data(3));

				Set_HysHighValue(Get_Mode(),BindingData.Get_Data(1));
				Set_HysLowValue(Get_Mode(),BindingData.Get_Data(2));
				//Set_Mode(BindingData.Get_Data(3));

			} return TRUE;

			case SENS_OBJECT_ATTR_CONTROL_ONOFF:
			{
				if (bLength != 2) return FALSE;
				Set_State(BindingData.Get_Data(1));

			} return TRUE;

			case SENS_OBJECT_METHOD_FSINC_DEC:
			case SENS_OBJECT_METHOD_INC:
			case SENS_OBJECT_METHOD_DEC:
			case SENS_OBJECT_METHOD_FSONOFF:
			{
				if (bLength != 1) return FALSE;

			} return TRUE;

			case SENS_OBJECT_ATTR_SAMPLE_INTERVAL:
			{
				if (bLength != 2) return FALSE;
				Set_SampleTime(BindingData.Get_Data(1));

			} return TRUE;

			case SENS_OBJECT_ATTR_CONFIG:				
			return FALSE;

			case SENS_OBJECT_ATTR_HYST_OFFSETS:
			{
				if (bLength != 3) return FALSE;
				Set_HysHighOffsetValue(BindingData.Get_Data(1));
				Set_HysLowOffsetValue(BindingData.Get_Data(2));

			} return TRUE;

			case SENS_OBJECT_ATTR_PRESET:
			{
				if (bLength != 2) return FALSE;
				Set_Mode(BindingData.Get_Data(1));

			} return TRUE;

			case SENS_OBJECT_ATTR_MEASUREDOFFSET:		/* TM20120405 */
			{
				if (bLength != 3) return FALSE;
				short sOffsetValue = (BindingData.Get_Data(1) << 8) +
									  BindingData.Get_Data(2);

				SetMeasuredValueOffset(sOffsetValue);

			} return TRUE;

			case SENS_OBJECT_ATTR_WORKING_MODE:	/* TM20120917 */ 
			{
				if (bLength != 2) return FALSE;
				Set_WorkingMode(BindingData.Get_Data(1));

			} return TRUE;

			case SENS_OBJECT_ATTR_FAN_SPEED:	/* TM20120917 */ 
			{
				if (bLength != 2) return FALSE;
				Set_FanSpeed(BindingData.Get_Data(1));

			} return TRUE;

			case SENS_OBJECT_ATTR_SWING_ANGLE:	/* TM20120917 */ 
			{
				if (bLength != 3) return FALSE;
				unsigned short angle = (BindingData.Get_Data(1) << 8) +
									   BindingData.Get_Data(2);
				Set_Angle(angle);

			} return TRUE;

			case SENS_OBJECT_ATTR_SWING_MODE:	/* TM20120917 */ 
			{
				if (bLength != 2) return FALSE;
				Set_SwingMode(BindingData.Get_Data(1));

			} return TRUE;

			// sedert versie 0x0900
			case SENS_OBJECT_ATTR_DELEGATE:
			{
				if (bLength != 3) return FALSE;

				SetDelegationType(CBindingSensUnit::DELEGATION_TYPE_DELEGATE);
				SetDelegationNodeAddress(BindingData.Get_Data(1));
				SetDelegationUnitAddress(BindingData.Get_Data(2));

			} return TRUE;
			
			// sedert versie 0x0900
			case SENS_OBJECT_ATTR_DELEGATOR:
			{
				if (bLength != 3) return FALSE;

				SetDelegationType(CBindingSensUnit::DELEGATION_TYPE_DELEGATOR);
				SetDelegationNodeAddress(BindingData.Get_Data(1));
				SetDelegationUnitAddress(BindingData.Get_Data(2));

			} return TRUE;
			
			case SENS_OBJECT_ATTR_CFG_MEASURESENSOR:		// Sedert versie 0x0C03
			{
				if (bLength != 3) return FALSE;

				SetDelegationType(CBindingSensUnit::DELEGATION_TYPE_REQUESTVALUE);
				SetDelegationNodeAddress(BindingData.Get_Data(1));
				SetDelegationUnitAddress(BindingData.Get_Data(2));
			} return TRUE;

			case SENS_OBJECT_ATTR_SP_RANGE:
			{
				if (bLength != 5) return FALSE;

				const short min = ((BindingData.Get_Data(1)<<8)+(BindingData.Get_Data(2)));
				const short max = ((BindingData.Get_Data(3)<<8)+(BindingData.Get_Data(4)));				
				setSetpointRangeValueMin(min);
				setSetpointRangeValueMax(max);
			
			} return TRUE;

			// Sedert V16.00: USE_SENSOR_PIDCONTROL
			case SENS_OBJECT_ATTR_PIDCONTROLUNIT: {
			
				if (bLength != 5) return FALSE;

				const BYTE workingsmode = BindingData.Get_Data(1);
				if (workingsmode == PIDCONTROL_WORKINGMODE_COOLING || 
					workingsmode == PIDCONTROL_WORKINGMODE_HEATING) {									    
					// const BYTE unitType = BindingData.Get_Data(2);
					const BYTE nodeAddress = BindingData.Get_Data(3);
					const BYTE unitAddress = BindingData.Get_Data(4);

					setPIDWorkingMode(workingsmode);
					setPIDControlUnit(workingsmode,nodeAddress,unitAddress);												  
				}
				else {
					setPIDWorkingMode(SENS_PIDCONTROL_WORKINGMODE_NONE);	
				}
			} return TRUE;

			// Sedert V16.00: USE_SENSOR_PIDCONTROL
			case SENS_OBJECT_ATTR_PIDCONTROLPARAMS: {

				if (bLength != 5) return FALSE;

				const BYTE workingsmode = BindingData.Get_Data(1);
				if (workingsmode == PIDCONTROL_WORKINGMODE_COOLING || 
					workingsmode == PIDCONTROL_WORKINGMODE_HEATING) {		
					const BYTE kp = BindingData.Get_Data(2);
					const BYTE ki = BindingData.Get_Data(3);
					const BYTE kd = BindingData.Get_Data(4);

					setPIDWorkingMode(workingsmode);
					setPIDControlParams(workingsmode,kp,ki,kd);													
				}
				else {
					setPIDWorkingMode(SENS_PIDCONTROL_WORKINGMODE_NONE);	
				}
			} return TRUE;

			default:
			{
				ASSERT(0);
			} return FALSE;
		}
	}
	return FALSE;
}

CString CBindingSensUnit::ConvertFuntionToString(void)
{
	CString strReturn;
	CString strFunction;

	if (GetInOutType() == UNIT_TYPE_INPUT) 
	{	
		CString strFunction;

		const BYTE bMethodData = Get_Event();		
		const CBindingStringEntries* const pStringEntries = 
			CBindingUnitStringEntries_Collection::Instance()->Get(SENS_STRINGS_INPUT);

		ASSERT(pStringEntries);
		pStringEntries->Find(bMethodData,strFunction);
		return strFunction;	
	}
	else 
	{			
		// UNIT_TYPE_OUTPUT
		const BYTE bMethodData = Get_Function();

		const CBindingStringEntries* const pStringEntries = 
			CBindingUnitStringEntries_Collection::Instance()->Get(SENS_STRINGS_EXTENDED);

		ASSERT(pStringEntries);

		pStringEntries->Find(bMethodData,strFunction);

		switch(bMethodData)
		{
			case SENS_OBJECT_METHOD_FSONOFF:
			case SENS_OBJECT_METHOD_INC:
			case SENS_OBJECT_METHOD_DEC:
			case SENS_OBJECT_METHOD_FSINC_DEC:			
			break;

			case SENS_OBJECT_ATTR_CONTROL_ONOFF :			
			{
				CString strTemp;
				
				if ((Get_State() & 0x01)==0x01) strTemp.LoadString(IDS_ON);
				else strTemp.LoadString(IDS_OFF);
				strReturn.Format("%s - %s",strFunction,strTemp);
				return strReturn;
			
			} break;

			case SENS_OBJECT_ATTR_PRESET:	
			{
				CString strTemp;	
				switch(Get_Mode() & 0x03) 
				{
					case SENS_MODE_SUN: strTemp.LoadString(IDS_SENS_SUN); break;
					case SENS_MODE_HALFSUN: strTemp.LoadString(IDS_SENS_HALFSUN); break;
					case SENS_MODE_MOON: strTemp.LoadString(IDS_SENS_MOON); break;
					case SENS_MODE_HALFMOON: strTemp.LoadString(IDS_SENS_HALFMOON); break;	
				}
				strReturn.Format("%s -%s",strFunction,strTemp);
			} return strReturn;

			case SENS_OBJECT_ATTR_WORKING_SET_POINT:
			{
				CString strTemp;
				CString strTemp2;
				
				strTemp2.Format("%02d,%01d°C",Get_SetPoint()/10,Get_SetPoint()%10);
				
				switch(Get_Mode() & 0x03) 
				{
					case SENS_MODE_SUN: strTemp.LoadString(IDS_SENS_SUN); break;
					case SENS_MODE_HALFSUN: strTemp.LoadString(IDS_SENS_HALFSUN); break;
					case SENS_MODE_MOON: strTemp.LoadString(IDS_SENS_MOON); break;
					case SENS_MODE_HALFMOON: strTemp.LoadString(IDS_SENS_HALFMOON); break;	
				}

				strReturn.Format("%s - %s - %s",strFunction,strTemp,strTemp2);

			} return strReturn;

			
			case SENS_OBJECT_ATTR_SAMPLE_INTERVAL:
			{
				CString strTemp("");

				if (FALSE==CBindingSensUnit_Utils::getStrSampleTime(
						*this,
						&strTemp))
				{
					ASSERT(0);
				}
				strReturn.Format("%s - %s",strFunction,strTemp);

			} return strReturn;

			case SENS_OBJECT_ATTR_WORKING_MODE:		/* TM20120917 */ 
			{
				CString strTemp("");
				const BYTE bTempWorkingMode = Get_WorkingMode( );

				switch (bTempWorkingMode)
				{
					case SENS_WORKING_MODE_AUTO:
					{
						strTemp.LoadString(IDS_SENS_WORKINGMODE_AUTO);
					} break;
					case SENS_WORKING_MODE_HEATING:
					{
						strTemp.LoadString(IDS_SENS_WORKINGMODE_HEATING);
					} break;
					case SENS_WORKING_MODE_COOLING:
					{
						strTemp.LoadString(IDS_SENS_WORKINGMODE_COOLING);
					} break;
					case SENS_WORKING_MODE_DRY:
					{
						strTemp.LoadString(IDS_SENS_WORKINGMODE_DRY);
					} break;
					case SENS_WORKING_MODE_FAN:
					{
						strTemp.LoadString(IDS_SENS_WORKINGMODE_FAN);
					} break;
					case SENS_WORKING_MODE_OFF:
					{
						strTemp.LoadString(IDS_SENS_WORKINGMODE_OFF);
					} break;
				}

				strReturn.Format("%s - %s",strFunction,strTemp);

			} return strReturn;

			case SENS_OBJECT_ATTR_FAN_SPEED:			/* TM20120917 */ 
			{
				CString strTemp;

				const BYTE fanSpeed = Get_FanSpeed( );
				if (fanSpeed == 0xFF)
				{
					strTemp = "Auto";				
				}
				else if (fanSpeed >= SENS_FAN_SPEED_1 && fanSpeed <= SENS_FAN_SPEED_6)
				{
					const unsigned long ulResourceID[ ] =
					{
						IDS_SENS_FANSPEED_1,
						IDS_SENS_FANSPEED_2,
						IDS_SENS_FANSPEED_3,
						IDS_SENS_FANSPEED_4,
						IDS_SENS_FANSPEED_5,
						IDS_SENS_FANSPEED_6
					};
					
					strTemp.LoadString(ulResourceID[fanSpeed-SENS_FAN_SPEED_1]);
				}
				else
				{
					strTemp.Format("Speed %d",fanSpeed+1);
				}
				strReturn.Format("%s - %s",strFunction,strTemp);

			} return strReturn;

			case SENS_OBJECT_ATTR_SWING_ANGLE:		
			{				
				const unsigned short swingAngle = Get_Angle( );				
				strReturn.Format("%s - %d°",strFunction,swingAngle);

			} return strReturn;

			case SENS_OBJECT_ATTR_SWING_MODE:		
			{
				// TM,20210111: Uitbreiding van de swing modes.
				// Vroeger was enkel on/off mogelijk.
				CString szTemp;
				const int swingMode = Get_SwingMode();
				if (0 == swingMode) {
					CString s;
					s.LoadString(IDS_OFF);
					szTemp.Format("%d - (%s)",swingMode,s);
				}
				else if (1 == swingMode) {
					CString s;
					s.LoadString(IDS_ON);
					szTemp.Format("%d - (%s)",swingMode,s);
				}
				else {
					szTemp.Format("%d",swingMode);
				}
				strReturn.Format("%s - %s",strFunction,szTemp);

			} return strReturn;

			case SENS_OBJECT_ATTR_SP_RANGE:
			{
				CString strTemp;
				strTemp.LoadString(IDS_SENS_SETPOINT_RANGE);
			
				strReturn.Format("%s - min=%d,max=%d",
								 strTemp,
								 getSetpointRangeValueMin(),
								 getSetpointRangeValueMax());

			} return strReturn;


			case SENS_OBJECT_ATTR_PIDCONTROLUNIT: {
					
				const int workingMode = this->getPIDWorkingMode();
				if (SENS_PIDCONTROL_WORKINGMODE_COOLING == workingMode || 
					SENS_PIDCONTROL_WORKINGMODE_HEATING == workingMode) 
				{
					int nodeAddress = 0xff;
					int unitAddress = 0xff;						
					if (this->getPIDControlUnit(workingMode,&nodeAddress,&unitAddress)) {
						CString mode = (SENS_PIDCONTROL_WORKINGMODE_COOLING == workingMode ? "Cooling" : "Heating");
						strReturn.Format("PID %s - Unit=0x%02x,0x%02x",mode,nodeAddress,unitAddress);
						return strReturn;
					}
				}
												  
			} break;

			case SENS_OBJECT_ATTR_PIDCONTROLPARAMS: {				
				const int workingMode = this->getPIDWorkingMode();
				if (SENS_PIDCONTROL_WORKINGMODE_COOLING == workingMode || 
					SENS_PIDCONTROL_WORKINGMODE_HEATING == workingMode) 
				{
					int kp = 0;
					int ki = 0;
					int kd = 0;
					if (this->getPIDControlParams(workingMode,&kp,&ki,&kd)) {
						CString mode = (SENS_PIDCONTROL_WORKINGMODE_COOLING == workingMode ? "Cooling" : "Heating");
						strReturn.Format("PID %s - params=%d,%d,%d",mode,kp,ki,kd);
						return strReturn;
					}
				}												
			} break;
		}
	}
	strReturn.Format("%s",strFunction);
	return strReturn;
}



// Sedert versie 0x0D00.

CBindingSensUnivUnit::CBindingSensUnivUnit(BYTE bBindingUnitType) :
Base(bBindingUnitType)
{
	// niks doen, wordt gedaan in de basisklasse.
}

CBindingSensUnivUnit::CBindingSensUnivUnit(
	const CBindingUnit& pBindingUnit) :
Base(pBindingUnit)
{
	// niks doen, wordt gedaan in de basisklasse.
}

CBindingSensUnivUnit::~CBindingSensUnivUnit( )
{
	// niks doen
}

BOOL CBindingSensUnivUnit::ConvertToBindingData(void)
{
	BOOL fResult=FALSE;

	if (this->GetInOutType() == UNIT_TYPE_INPUT) 
	{
		BYTE bEvent = Get_Event();
	
		this->BindingData.Clear();	

		switch(bEvent)
		{
			case SENSUNIV_EVENT_INC:
			{
				BindingData.Set_MessageCode(EV_UNITCONTROLTOGGLE);		

				if (Get_EventState() == SENS_EVENT_ON) 
					BindingData.Set_BindingFlags(SENSUNIV_INCREMENT_OFF);
				else 
					BindingData.Set_BindingFlags(SENSUNIV_INCREMENT_ON);

				fResult=TRUE;

			} break; /* SENSUNIV_EVENT_INC */

			case SENSUNIV_EVENT_DEC:
			{
				BindingData.Set_MessageCode(EV_UNITCONTROLPULSTOGGLE);		

				if (Get_EventState() == SENS_EVENT_ON) 
					BindingData.Set_BindingFlags(SENSUNIV_DECREMENT_OFF);
				else 
					BindingData.Set_BindingFlags(SENSUNIV_DECREMENT_ON);

				fResult=TRUE;

			} break; /* SENSUNIV_EVENT_DEC */

			case SENSUNIV_EVENT_INC_PULS:
			{
				BindingData.Set_MessageCode(EV_UNITCONTROLPULS);	
				BindingData.Set_BindingFlags(SENSUNIV_INCREMENT_ON);
				fResult=TRUE;
			} break; /* SENSUNIV_EVENT_INC_PULS */
			case SENSUNIV_EVENT_DEC_PULS:
			{
				BindingData.Set_MessageCode(EV_UNITCONTROLPULS);
				BindingData.Set_BindingFlags(SENSUNIV_DECREMENT_ON);		
				fResult=TRUE;
			} break; /* SENSUNIV_EVENT_DEC_PULS */
		} /* switch(bEvent) */
	}
	else
	{
		fResult=this->Base::ConvertToBindingData();
	}
	return fResult;
}

BOOL CBindingSensUnivUnit::ConvertFromBindingData(void)
{
	BOOL fResult=FALSE;

	if (this->GetInOutType() == UNIT_TYPE_INPUT) 
	{
		const BYTE bMessageCode = BindingData.Get_MessageCode();
		BYTE bBindingFlags = 0;

		Init();			

		switch(bMessageCode)
		{
			case EV_UNITCONTROLTOGGLE:
			{
				Set_Event(SENSUNIV_EVENT_INC);
				bBindingFlags = BindingData.Get_BindingFlags();

// wat doen met de binding vlaggen...
				if (bBindingFlags & 0x80) 
					Set_EventState(SENS_EVENT_ON);
				else 
					Set_EventState(SENS_EVENT_OFF);

				fResult=TRUE;

			} break;

			case EV_UNITCONTROLPULSTOGGLE:
			{
				Set_Event(SENSUNIV_EVENT_DEC);
				bBindingFlags = BindingData.Get_BindingFlags();

// wat doen met de binding vlaggen...
				if (bBindingFlags & 0x80) 
					Set_EventState(SENS_EVENT_ON);
				else 
					Set_EventState(SENS_EVENT_OFF);

				fResult=TRUE;
			} break;

			case EV_UNITCONTROLPULS:
			{
				bBindingFlags = BindingData.Get_BindingFlags();

				if ((bBindingFlags&0x7F)==SENSUNIV_INCREMENT_ON)
				{
					Set_Event(SENSUNIV_EVENT_INC_PULS);
				}
				else if ((bBindingFlags&0x7F)==SENSUNIV_DECREMENT_ON)
				{
					Set_Event(SENSUNIV_EVENT_DEC_PULS);
				}
				else
				{
					ASSERT(0);
				}

// wat doen met de binding vlaggen...
				if (bBindingFlags & 0x80) 
					Set_EventState(SENS_EVENT_ON);
				else 
					Set_EventState(SENS_EVENT_OFF);

				fResult=TRUE;

			} break;

			default:
			{
				ASSERT(0);
			} break;

		} /* switch(bMessageCode) */
	}
	else
	{
		fResult=this->Base::ConvertFromBindingData();
	}
	return fResult;
}

CString CBindingSensUnivUnit::ConvertFuntionToString(void)
{
	CString strMethod;

	if (this->GetInOutType() == UNIT_TYPE_INPUT) 	
	{			
		const BYTE bEvent = this->Get_Event();	
		const CBindingStringEntries* const pStringEntries = 
			CBindingUnitStringEntries_Collection::Instance()->Get(SENSUNIV_STRINGS_INPUT);

		ASSERT(pStringEntries);
		pStringEntries->Find(bEvent,strMethod);
		return strMethod;	
	}
	else
	{
		return this->Base::ConvertFuntionToString();
	}
	return "";
}

BOOL CBindingSensUnivUnit::UpgradeIoType()
{
	const BYTE bMessageCode = BindingData.Get_MessageCode();

	if (bMessageCode == EV_UNITCONTROLPULSTOGGLE ||
		bMessageCode == EV_UNITCONTROLTOGGLE ||
		bMessageCode == EV_UNITCONTROLPULS)
	{
		SetInOutType(UNIT_TYPE_INPUT);
		return TRUE;
	}
	return this->Base::UpgradeIoType();
}


void CBindingSensUnivUnit::InitSensorInput(void)
{
	Set_EventState(SENS_EVENT_ON);
	Set_Event(SENSUNIV_EVENT_INC);
}

void CBindingSensUnivUnit::Init(void)
{	
	this->CBindingSensUnivUnit::InitSensorInput();
	InitSensorsOutput( );
	InitSensorExtended( );
	InitDelegation( );
	InitSensorUsingDefaults( );
}




// Utility klasse 
BOOL CBindingSensUnit_Utils::getStrSampleTime(
	const CBindingSensUnit& ref,
	CString* pString)
{
	BOOL fResult=TRUE;
	const BYTE bType = ref.GetBindingUnitType();	
	int value = ref.Get_SampleTime();
	CString s;

	REQUIRE(pString);

	switch(bType)
	{			
		case BINDING_UNIT_TYPE_SENS:
		{															
			s.Format("%d sec",value);
			*pString = s;
		} break;

		case BINDING_UNIT_TYPE_SENSUNIV:
		{														
			if (value>=SENSUNIV_SAMPLETIME_MINUTE)
			{
				value-=(SENSUNIV_SAMPLETIME_MINUTE-1);
				s.Format("%d min",value);
			}
			else 
			{
				s.Format("%d sec",value);
			}
			*pString = s;
		} break;

		default:
		{
			fResult=FALSE;
		} break;
	};
	return fResult;
}