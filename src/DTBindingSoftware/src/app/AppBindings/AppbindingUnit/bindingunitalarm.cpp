#include "stdafx.h"
#include "BindingSoftware.h"
#include "bindingunitalarm.h"

#include "NodeMess.h"
#include "BindingConstants.h"
#include "bindingUIElement.h"	// ??
#include "bindingunitstate.h"

#include "App/AppCfg/AppCfgMM/ConfigMultiMediaClass.h"

#include "App\Appbindings\appbindingproperty\BindingPropertyStrings.h"
#include "App\Appbindings\appbindingproperty\BindingPropertyUnit.h"	
#include "App\Appbindings\appbindingproperty\BindingPropertyUnitDefault.h"
#include "App\Appbindings\appbindingproperty\BindingPropertyUnitDefaultFactory.h"

using namespace DUOTECNO::BINDINGS::UI;


CBindingAlarmUnit::CBindingAlarmUnit(BYTE bBindingUnitType,BYTE bMaxInputs) :
Base(bBindingUnitType),
m_bMaxInputs(bMaxInputs) 
{
	SetInOutType(UNIT_TYPE_INPUT);

	Init();	
}

CBindingAlarmUnit::CBindingAlarmUnit(const CBindingUnit &BindingUnit,BYTE bMaxInputs) :
CBindingUnit(BindingUnit),
m_bMaxInputs(bMaxInputs)
{
	Init();	
}

CBindingAlarmUnit::~CBindingAlarmUnit()
{

}


void CBindingAlarmUnit::Init(void)			// TM 20040701
{		
	Set_Function(ALARM_OBJECT_METHOD_ARM);
	Set_Event(ALARM_EVENT_ALARM_INPUT);

	Set_ControlState(0);
//	Set_ZoneNr(0);
	Set_Keypress(0);	
	//Set_GroupNr(0);
	Set_GroupNr(1);
	Set_ArmSet(0);

	// TM 20071119
	const int Zone = CBindingUnitState::Instance()
		->get_AlarmUnit()->get();

	Set_ZoneNr(Zone);

	const CBindingPropertyUnitAlarmDefault* const pDefaults = 
		static_cast<const CBindingPropertyUnitAlarmDefault* const>
			(CBindingPropertyUnitDefaultFactory::Instance()->
				Create(UNIT_TYPE_ALARM));

	if (pDefaults != 0) 
	{
		memcpy(&bRemoteCode,
			   pDefaults->GetRemoteCode(),
			   ALARM_REMOTECODE_BUFFER_MAX);

		Set_Config(pDefaults->GetAlarmType());
	}
}

BOOL CBindingAlarmUnit::UpgradeIoType()
{
	const BYTE bMessageCode = BindingData.Get_MessageCode();

	if ((bMessageCode == EV_UNITCONTROLTOGGLE) || 
		(bMessageCode == EV_UNITCONTROLPULSTOGGLE) ||
		(bMessageCode == EV_UNITCONTROLPULS)) 
	{
		SetInOutType(UNIT_TYPE_INPUT);
		return TRUE;
	}
	else if (bMessageCode == FC_UNITALARMSET)
	{
		SetInOutType(UNIT_TYPE_OUTPUT);
		return TRUE;
	}
	return FALSE;
}

BOOL CBindingAlarmUnit::ConvertToBindingData(void)
{	
	BindingData.Clear();
	
	if (GetInOutType() ==  UNIT_TYPE_INPUT) 
	{	
		// TM_CHANGES_BINDINGS_ALARM: Onderscheid maken tss het event.

		const BYTE bEvent=Get_Event();
		switch(bEvent)
		{
			case ALARM_EVENT_ALARM_INPUT:
			{		
				BindingData.Set_MessageCode(EV_UNITCONTROLPULSTOGGLE);

				BYTE bTemp = Get_ZoneNr() & 0x7F;

				if (bTemp >= m_bMaxInputs)		// TM20110509
				{
					bTemp = m_bMaxInputs-1;
				}
			
				if (Get_ControlState() & 0x01) 
				{
					bTemp |= 0x80;
				}
				
				BindingData.Set_BindingFlags(bTemp);
				
				CBindingUnitState::Instance()->get_AlarmUnit()->set(bTemp & 0x7F);
				
				return TRUE;

			} break;

			case ALARM_EVENT_ALARM_ARM:
			{
				BindingData.Set_MessageCode(EV_UNITCONTROLTOGGLE);

				BYTE bTemp = Get_GroupNr( );
				if (bTemp>=1 && bTemp<=4)
				{
					bTemp-=1;

					if (Get_ControlState() & 0x01) 
					{
						bTemp |= 0x80;
					}	

					BindingData.Set_BindingFlags(bTemp);
					return TRUE;
				}
				else
				{
					ASSERT(bTemp>=0 && bTemp<=4);
				}
				return FALSE;

			} break;

			case ALARM_EVENT_ALARM_BURGLARY:
			{
				BindingData.Set_MessageCode(EV_UNITCONTROLTOGGLE);

				BYTE bTemp = Get_GroupNr( );
				if (bTemp>=1 && bTemp<=4)
				{
					// Oppassen:
					// Bindingvlaggen 0-3 = ALARM_EVENT_ALARM_ARM
					// Bindingvlaggen 4-7 = ALARM_EVENT_ALARM_BURGLARY
					bTemp+=4;
					bTemp-=1;

					if (Get_ControlState() & 0x01) 
					{
						bTemp |= 0x80;
					}	

					BindingData.Set_BindingFlags(bTemp);
					return TRUE;
				}
				else
				{
					ASSERT(bTemp>=0 && bTemp<=4);
				}
				return FALSE;

			} break;

			default:
			{
				ASSERT(0);
			} break;
		}
	}
	else 
	{
		BindingData.Set_MessageCode(FC_UNITALARMSET);
		const BYTE bMethodData = Get_Function();

		BindingData.Set_MethodData(bMethodData);
		switch(bMethodData)
		{
			case ALARM_OBJECT_METHOD_KEYPAD:
			{
				BindingData.Set_Length(2);
				BindingData.Set_Data(1,Get_Keypress());

			} return TRUE;
		
			case ALARM_OBJECT_METHOD_ARM :
			{
				BindingData.Set_Length(3);
				BindingData.Set_Data(1,Get_GroupNr());
				BindingData.Set_Data(2,Get_ArmSet());

			} return TRUE;

			case ALARM_OBJECT_ATTR_CONFIG:
			{
				BindingData.Set_Length(2);
				BindingData.Set_Data(1,Get_Config());
			} return TRUE;

			default:
				ASSERT(0);
			return FALSE;				
		}	
	}
	return FALSE;
}

BOOL CBindingAlarmUnit::ConvertFromBindingData(void)
{
	Init();	

	const BYTE bMessageCode = BindingData.Get_MessageCode();
	
	if (GetInOutType() == UNIT_TYPE_INPUT) 
	{ 		
		const BYTE bBindingFlags = BindingData.Get_BindingFlags();

		// TM_CHANGES_BINDINGS_ALARM
		// Meerdere events:
		//  - EV_UNITCONTROLPULSTOGGLE -> ALARM_EVENT_ALARM_INPUT + ZoneNR 0-127
		//  - EV_UNITCONTROLTOGGLE -> ALARM_EVENT_ALARM_ARM + group 0-3
		//  - EV_UNITCONTROLTOGGLE -> ALARM_EVENT_ALARM_BURGLARY + group 0-3

		switch(bMessageCode)
		{
			case EV_UNITCONTROLPULSTOGGLE:
			{									
				Set_Event(ALARM_EVENT_ALARM_INPUT);

				Set_ControlState((bBindingFlags&0x80) ? 1 : 0);

				BYTE bTemp = bBindingFlags & 0x7F;
				if (bTemp >= m_bMaxInputs)		
				{
					bTemp = m_bMaxInputs-1;
				}
				Set_ZoneNr(bTemp);

				return TRUE;

			} break;

			case EV_UNITCONTROLTOGGLE:
			{				
				BYTE bTemp = (bBindingFlags & 0x7F);

				if (bTemp>=0&&bTemp<=3)
				{
					// Bindings voor Status Armed.

					// Het event
					Set_Event(ALARM_EVENT_ALARM_ARM);

					// group nr
					Set_GroupNr(bTemp+1);

					// toestand:
					Set_ControlState((bBindingFlags&0x80) ? 1 : 0);
					
				}
				else if (bTemp>=4&&bTemp<=7)
				{
					// Binding voor status Burglary

					// Het event
					Set_Event(ALARM_EVENT_ALARM_BURGLARY);

					// group nr
					Set_GroupNr((bTemp+1)-4);

					// toestand:
					Set_ControlState((bBindingFlags&0x80) ? 1 : 0);
				}
				else
				{				
					return FALSE;
				}
				return TRUE;

			} break;

			default:
			{
				ASSERT(bMessageCode==EV_UNITCONTROLPULSTOGGLE ||
					   bMessageCode==EV_UNITCONTROLTOGGLE);
			} break;
		}		 
	}
	else 
	{
		if (bMessageCode != FC_UNITALARMSET) 
		{
			ASSERT(0);
			return FALSE;
		}

		const BYTE bMethodData = BindingData.Get_MethodData();
		Set_Function(bMethodData);
		
		const BYTE bLength = BindingData.Get_Length();

		switch (bMethodData)
		{		
			case ALARM_OBJECT_METHOD_KEYPAD:
			{
				if (bLength != 2) return FALSE;
				Set_Keypress(BindingData.Get_Data(1));						

			} return TRUE;

			case ALARM_OBJECT_METHOD_ARM :
			{
				if (bLength != 3) return FALSE;
				Set_GroupNr(BindingData.Get_Data(1));
				Set_ArmSet(BindingData.Get_Data(2));		

			} return TRUE;

			case ALARM_OBJECT_ATTR_CONFIG:
			{
				if (bLength != 2) return FALSE;
				Set_Config(BindingData.Get_Data(1));

			} return TRUE;

			case ALARM_OBJECT_ATTR_REMOTECODE:
			{
				BYTE bIndex = BindingData.Get_Data(1);

				for (int i=2;i<bLength;i++,bIndex++)
				{
					Set_RemoteCode(bIndex,BindingData.Get_Data(i));
				}
			} return TRUE;

			default:
				ASSERT(0);
			return FALSE;
		}
	}
	ASSERT(0);
	return FALSE;
}


// TM_CHANGES_BINDINGS_ALARM
// Andere implementatie.
CString CBindingAlarmUnit::ConvertFuntionToString(void)
{
	CString strReturn;
	CString strFunction;
	CString strTemp;

	if (GetInOutType() == UNIT_TYPE_INPUT) 
	{ 		
		const BYTE bEvent = Get_Event();

		const CBindingStringEntries* const pStringEntries = 
			CBindingUnitStringEntries_Collection::Instance()->Get(ALARM_INPUTSTRINGS_EXTENDED);

		ASSERT(pStringEntries);

		if (FALSE==pStringEntries->Find(bEvent,strFunction))
		{
			ASSERT(0);
			return "";
		}

		switch(bEvent)
		{
			case ALARM_EVENT_ALARM_INPUT:
			{
				// TM_CHANGES_ALARM_ARITECH + TM_CHANGES_ALARM_ANB + TM_CHANGES_ALARM_TEXECOM +
				// TM_CHANGES_ALARM_INTEGRA
				// ifv het alarm type wordt er een andere zone naam getoond.

				const BYTE bTempUnitType = GetBindingUnitType( );
				CConfigAlarmZoneNameUtil::TypeAlarmCentrale type = CConfigAlarmZoneNameUtil::TYPE_ALARM_GALAXY;		

				if (bTempUnitType == BINDING_UNIT_TYPE_ALARM_ARITECH)
				{
					type = CConfigAlarmZoneNameUtil::TYPE_ALARM_ARITECH;
				}
				else if (bTempUnitType == BINDING_UNIT_TYPE_ALARM_ANB)
				{
					type = CConfigAlarmZoneNameUtil::TYPE_ALARM_ANB;
				}
				else if (bTempUnitType == BINDING_UNIT_TYPE_ALARM_TEXECOM)
				{
					type = CConfigAlarmZoneNameUtil::TYPE_ALARM_TEXECOM;
				}
				else if (bTempUnitType == BINDING_UNIT_TYPE_ALARM_INTEGRA)
				{
					type = CConfigAlarmZoneNameUtil::TYPE_ALARM_INTEGRA;
				}

				BYTE bTemp = Get_ZoneNr();
				bTemp &= 0x7F;	
				ASSERT(bTemp < m_bMaxInputs);

				char szZoneName[16];
				if (!CConfigAlarmZoneNameUtil::GetAlarmZoneName(
						type,			// TM_CHANGES_ALARM_ARITECH: ifv het type
						bTemp,
						16,
						szZoneName))
				{
					sprintf(szZoneName,"AlarmZone ?");
				}

				strReturn.Format("%s - %s",strFunction,szZoneName);						
				return strReturn;
			} break;

			case ALARM_EVENT_ALARM_ARM:
			{				
				const BYTE bTemp = Get_GroupNr();

				ASSERT(bTemp>=1 && bTemp<=4);
					
				strReturn.Format("Status %s Group %d",
								 (Get_ControlState()==0 ? "disarmed" : "armed"),
								 bTemp);
				return strReturn;
			} break;

			case ALARM_EVENT_ALARM_BURGLARY:
			{
				const BYTE bTemp = Get_GroupNr();

				ASSERT(bTemp>=1 && bTemp<=4);
					
				strReturn.Format("Status %s Group %d",
								 (Get_ControlState()==0 ? "no burglary" : "burglary"),
								 bTemp);
				return strReturn;
			} break;

			default:
			{
				ASSERT(0);
			} break;
		}
	}
	else 
	{
		const BYTE bMethodData = Get_Function();
	
		const CBindingStringEntries* const pStringEntries = 
			CBindingUnitStringEntries_Collection::Instance()->Get(ALARM_OUTPUTSTRING);

		ASSERT(pStringEntries);

		if (FALSE==pStringEntries->Find(bMethodData,strFunction))
		{
			ASSERT(0);
			return "";
		}

		switch(bMethodData)
		{
			case ALARM_OBJECT_METHOD_KEYPAD :
			{
				strTemp.Format("%d",Get_Keypress());				
			} break;

			case ALARM_OBJECT_METHOD_ARM :
			{
				strTemp.Format("%d,%d",Get_GroupNr(),Get_ArmSet());			
			} break;

			default:
			{
				ASSERT(0);
			} return strReturn;
		}
		strReturn.Format("%s - %s",strFunction,strTemp);
	}
	return strReturn;
}


/*
BYTE CBindingAlarmUnit::Get_MaxInputs( ) const 
{				
	BYTE bResult = ALARM_GALAXY_MAX_INPUTS;
	if (GetBindingUnitType( ) == BINDING_UNIT_TYPE_ALARM_ARITECH)
	{
		bResult = ALARM_ARITECH_MAX_INPUTS;
	}
	return bResult;	
}
*/