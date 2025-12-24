#include "stdafx.h"
#include "BindingSoftware.h"
#include "bindingunitduoswitch.h"

#include "NodeMess.h"
#include "BindingConstants.h"
#include "bindingUIElement.h"	// ??

#include "App\Appbindings\appbindingproperty\BindingPropertyStrings.h"
#include "App\Appbindings\appbindingproperty\BindingPropertyUnit.h"	
#include "App\Appbindings\appbindingproperty\BindingPropertyUnitDefault.h"
#include "App\Appbindings\appbindingproperty\BindingPropertyUnitDefaultFactory.h"

using namespace DUOTECNO::BINDINGS::UI;

CBindingDuoSwitchUnit::CBindingDuoSwitchUnit(BYTE bBindingUnitType) :
Base(bBindingUnitType)
{
	SetInOutType(UNIT_TYPE_OUTPUT);

	Init();			// TM 20040701
}

CBindingDuoSwitchUnit::CBindingDuoSwitchUnit(const CBindingUnit &BindingUnit) :
CBindingUnit(BindingUnit)
{
	ASSERT( BindingUnit.GetNodeAddress() != 0xFF);

	Init();
}

CBindingDuoSwitchUnit::~CBindingDuoSwitchUnit()
{

}

void CBindingDuoSwitchUnit::Init(void)			// TM 20040701
{
	Set_Function(DUOSWITCH_OBJECT_METHOD_FSUPDOWN);
	
	const CBindingPropertyUnitDuoswitchDefault* pDefaults = 0;

	if (GetNodeAddress() == 0xFF)
	{
		pDefaults = static_cast<const CBindingPropertyUnitDuoswitchDefault* const>
				(CBindingPropertyUnitDefaultFactory::Instance()->
					Create(UNIT_TYPE_DUOSWITCH));	
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

		pDefaults = static_cast<const CBindingPropertyUnitDuoswitchDefault* const>
				(CBindingPropertyUnitDefaultFactory::Instance()->
					Create(UNIT_TYPE_DUOSWITCH, unitInfo));
	}
	
	ASSERT( pDefaults != 0 );
	
	if (pDefaults != 0)
	{
		Set_StopTime(pDefaults->GetStopTime());
		Set_SwitchTime(pDefaults->GetSwitchTime());
	}
}

BOOL CBindingDuoSwitchUnit::UpgradeIoType()
{
	const BYTE bMessageCode = BindingData.Get_MessageCode();
	if ( bMessageCode == FC_UNITDUOSWITCHSET )
	{
		SetInOutType(UNIT_TYPE_OUTPUT);
		return TRUE;
	}
	return FALSE;
}

BOOL CBindingDuoSwitchUnit::ConvertToBindingData(void)
{
	BindingData.Clear();
	BindingData.Set_MessageCode(FC_UNITDUOSWITCHSET);

	BYTE bMethodData = Get_Function();
	BindingData.Set_MethodData(bMethodData);

	// TM 20051108 : DuoSwitch Unit must always be an output unit !!!!
	if (GetInOutType() == UNIT_TYPE_INPUT)
	{
		AfxMessageBox("Duoswitch Unit must always be an output unit");
		return FALSE;
	}
	
	switch(bMethodData)
	{
		case DUOSWITCH_OBJECT_USE_SELECTOR_DATA:
		case DUOSWITCH_OBJECT_ATTR_STATE:
			//ABSORBE
		return FALSE;
		case DUOSWITCH_OBJECT_ATTR_UPSTOP:						// Length=2 ,	0x01,0/1
		case DUOSWITCH_OBJECT_ATTR_DOWNSTOP:					//				0x02,0/1
		{
			BindingData.Set_Length(2);
			BindingData.Set_Data(1,Get_Status() & 0x01);
		} return TRUE;
		case DUOSWITCH_OBJECT_METHOD_STOP:						// Length = 1,	0x03
		case DUOSWITCH_OBJECT_METHOD_UP:						//				0x04			
		case DUOSWITCH_OBJECT_METHOD_DOWN:						//				0x05
		case DUOSWITCH_OBJECT_METHOD_FSUPDOWN:					//				0x06
		{
			BindingData.Set_Length(1);
		} return TRUE;
		case DUOSWITCH_OBJECT_ATTR_CONFIG:						// Length =2,	0x08
		{
			BindingData.Set_Length(2);
			BindingData.Set_Data(1,Get_Config());
		} return TRUE;
		case DUOSWITCH_OBJECT_ATTR_STOPTIMER:					// Length =2,	0x09
		{
			BindingData.Set_Length(2);
			BindingData.Set_Data(1,Get_StopTime());
		} return TRUE;
		case DUOSWITCH_OBJECT_ATTR_SWITCHTIMER:					// Length =2,	0x0A
		{
			BindingData.Set_Length(2);
			BindingData.Set_Data(1,Get_SwitchTime());
		} return TRUE;
		default:
		{
			ASSERT(0);
		} return FALSE;
	}

	return FALSE;
}

BOOL CBindingDuoSwitchUnit::ConvertFromBindingData(void)
{
	Init();			// TM 20040701

	BYTE bMessageCode = BindingData.Get_MessageCode();
	
	if (bMessageCode != FC_UNITDUOSWITCHSET) 
	{
		ASSERT(0);
		return FALSE;
	}

	BYTE bMethodData = BindingData.Get_MethodData();
	Set_Function(bMethodData);
	
	BYTE bLength = BindingData.Get_Length();

	switch(bMethodData) 
	{
		case DUOSWITCH_OBJECT_USE_SELECTOR_DATA:
		case DUOSWITCH_OBJECT_ATTR_STATE:
			//ABSORBE
		return FALSE;

		case DUOSWITCH_OBJECT_ATTR_UPSTOP:						// Length=2 ,	0x01,0/1
		case DUOSWITCH_OBJECT_ATTR_DOWNSTOP:					//				0x02,0/1
		{
			if (bLength != 2) return FALSE;
			Set_Status(BindingData.Get_Data(1));			
		} return TRUE;

		case DUOSWITCH_OBJECT_METHOD_STOP:						// Length = 1,	0x03
		case DUOSWITCH_OBJECT_METHOD_UP:						//				0x04			
		case DUOSWITCH_OBJECT_METHOD_DOWN:						//				0x05
		case DUOSWITCH_OBJECT_METHOD_FSUPDOWN:					//				0x06
		{
			if (bLength != 1) return FALSE;		
		} return TRUE;

		case DUOSWITCH_OBJECT_ATTR_CONFIG:						// Length =2,	0x08
		{
			if (bLength != 2) return FALSE;
			Set_Config(BindingData.Get_Data(1));
		} return TRUE;

		case DUOSWITCH_OBJECT_ATTR_STOPTIMER:					// Length =2,	0x09
		{
			if (bLength != 2) return FALSE;
			Set_StopTime(BindingData.Get_Data(1));
		} return TRUE;

		case DUOSWITCH_OBJECT_ATTR_SWITCHTIMER:					// Length =2,	0x0A
		{
			if (bLength != 2) return FALSE;

			Set_SwitchTime(BindingData.Get_Data(1));
		} return TRUE;
		default:
		{
			ASSERT(0);
		} return FALSE;
	}
	return FALSE;
}

CString CBindingDuoSwitchUnit::ConvertFuntionToString(void)
{
	CString strMethod;
	CString strData;

	const BYTE bMethodData = Get_Function();
	const CBindingStringEntries* const pStringEntries = 
		CBindingUnitStringEntries_Collection::Instance()->Get(DUOSWITCH_STRINGS_EXTENDED);

	ASSERT(pStringEntries);

	pStringEntries->Find(bMethodData,strMethod);

	switch(bMethodData) 
	{
		case DUOSWITCH_OBJECT_USE_SELECTOR_DATA:
		case DUOSWITCH_OBJECT_ATTR_STATE:
			//ABSORBE
		break;
		case DUOSWITCH_OBJECT_ATTR_UPSTOP:						// Length=2 ,	0x01,0/1
		case DUOSWITCH_OBJECT_ATTR_DOWNSTOP:					//				0x02,0/1
		{
			strData.Format("- %d",Get_Status());
		} break;

		case DUOSWITCH_OBJECT_METHOD_STOP:						// Length = 1,	0x03
		case DUOSWITCH_OBJECT_METHOD_UP:						//				0x04			
		case DUOSWITCH_OBJECT_METHOD_DOWN:						//				0x05
		case DUOSWITCH_OBJECT_METHOD_FSUPDOWN:					//				0x06		
		{
		} break;
		case DUOSWITCH_OBJECT_ATTR_CONFIG:						// Length =2,	0x08		
		{
			strData.Format("- %d",Get_Config());
		} break;
		case DUOSWITCH_OBJECT_ATTR_STOPTIMER:					// Length =2,	0x09	
		{
			strData.Format("- %d",Get_StopTime());
		} break;
		case DUOSWITCH_OBJECT_ATTR_SWITCHTIMER:					// Length =2,	0x0A			
		{
			strData.Format("- %d",Get_SwitchTime());
		} break;
		default:
			ASSERT(0);
		break;
	}	

	CString strReturn;

	if (!strData.IsEmpty())
	{
		strReturn.Format("%s%s",strMethod,strData);	
	}
	else
	{
		strReturn = strMethod;
	}

	return strReturn;
}


/***************************************************************************************/

// CBindingDuoSwitchUnitInput

CBindingDuoSwitchUnitInput::CBindingDuoSwitchUnitInput(BYTE bBindingUnitType) :
Base(bBindingUnitType),
m_bEvent(EV_UNITCONTROLPULSTOGGLE),
m_bInputState(1)
{
	Init( );
}

CBindingDuoSwitchUnitInput::CBindingDuoSwitchUnitInput(const CBindingUnit& pBindingUnit) :
Base(pBindingUnit),
m_bEvent(EV_UNITCONTROLPULSTOGGLE),
m_bInputState(1)
{
	Init( );
}

CBindingDuoSwitchUnitInput::~CBindingDuoSwitchUnitInput()
{

}

void CBindingDuoSwitchUnitInput::Init(void)
{
	// TODO: Evt opvragen van unitState.

	m_bEvent = EV_UNITCONTROLPULSTOGGLE;
	m_bInputState = 1;

	this->Base::Init( );
}

BOOL CBindingDuoSwitchUnitInput::UpgradeIoType()
{
	const BYTE bMessageCode = BindingData.Get_MessageCode();

	if ((bMessageCode == EV_UNITCONTROLPULSTOGGLE)||
		(bMessageCode == EV_UNITCONTROLTOGGLE))
	{
		SetInOutType(UNIT_TYPE_INPUT);
		return TRUE;
	}

	return this->Base::UpgradeIoType();
}

BOOL CBindingDuoSwitchUnitInput::ConvertToBindingData(void)
{		
	BOOL fResult=FALSE;

	if (this->GetInOutType() == UNIT_TYPE_INPUT) 
	{
		BYTE bTemp=0;
		const BYTE bMessageCode = this->Get_Event();

		this->BindingData.Clear();		
		
		if ((bMessageCode== EV_UNITCONTROLPULSTOGGLE)||
			(bMessageCode== EV_UNITCONTROLTOGGLE))
		{
			if (this->Get_InputState() != 0)
			{
				bTemp|=0x80;
			}

			this->BindingData.Set_MessageCode(bMessageCode);
			this->BindingData.Set_BindingFlags(bTemp);	

			// TODO: Evt de toestand opslaan in unitState.

			fResult=TRUE;
		}		
		else
		{
			ASSERT(0);
			fResult=FALSE;
		}
	}
	else
	{
		fResult=this->Base::ConvertToBindingData();
	}
	return fResult;
}

BOOL CBindingDuoSwitchUnitInput::ConvertFromBindingData(void)
{
	BOOL fResult=FALSE;

	if (this->GetInOutType() == UNIT_TYPE_INPUT) 
	{
		Init();			

		const BYTE bMessageCode = BindingData.Get_MessageCode();

		if ((bMessageCode== EV_UNITCONTROLPULSTOGGLE)||
			(bMessageCode== EV_UNITCONTROLTOGGLE))
		{
			int state = 0;
			const BYTE bBindingFlags = BindingData.Get_BindingFlags();

			if (bBindingFlags & 0x80)
			{
				state=1;
			}
			
			this->Set_Event(bMessageCode);
			this->Set_InputState(state);
			fResult=TRUE;
		}
		else
		{
			fResult=FALSE;
			ASSERT(0);
		}
	}
	else
	{
		fResult=this->Base::ConvertFromBindingData();
	}
	return fResult;
}


CString CBindingDuoSwitchUnitInput::ConvertFuntionToString(void)
{
CString strMethod;

	if (this->GetInOutType() == UNIT_TYPE_INPUT) 	
	{				
		const CBindingStringEntries* const pStringEntries = 
			CBindingUnitStringEntries_Collection::Instance()->Get(DUOSWITCH_STRINGS_INPUT);

		if(0!=pStringEntries)
		{			
			const BYTE bMethodData = this->Get_Event();

			if (TRUE==pStringEntries->Find(bMethodData,strMethod))
			{
				return strMethod;
			}
		}
	}
	else
	{
		return this->Base::ConvertFuntionToString();
	}
	return "";
}
