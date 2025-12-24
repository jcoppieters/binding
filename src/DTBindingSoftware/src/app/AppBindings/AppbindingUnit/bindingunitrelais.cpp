#include "stdafx.h"
#include "BindingSoftware.h"
#include "bindingunitrelais.h"

#include "NodeMess.h"
#include "BindingConstants.h"
#include "bindingUIElement.h"	// ??

#include "App\Appbindings\appbindingproperty\BindingPropertyStrings.h"
#include "App\Appbindings\appbindingproperty\BindingPropertyUnit.h"	
#include "App\Appbindings\appbindingproperty\BindingPropertyUnitDefault.h"
#include "App\Appbindings\appbindingproperty\BindingPropertyUnitDefaultFactory.h"

using namespace DUOTECNO::BINDINGS::UI;


CBindingRelaisUnit::CBindingRelaisUnit(BYTE bBindingUnitType) :
Base(bBindingUnitType)
{
	SetInOutType(UNIT_TYPE_OUTPUT);

	Init();
}

CBindingRelaisUnit::CBindingRelaisUnit(const CBindingUnit &BindingUnit) :
CBindingUnit(BindingUnit)
{
	ASSERT( BindingUnit.GetNodeAddress() != 0xFF);

	Init();
}

CBindingRelaisUnit::~CBindingRelaisUnit()
{

}


void CBindingRelaisUnit::Init(void)			
{
	Set_PirTime(0xFF);	

	Set_Function(SWITCH_OBJECT_METHOD_FSONOFF);
	Set_StopTime( 0 );
}

BOOL CBindingRelaisUnit::UpgradeIoType()
{
	const BYTE bMessageCode = BindingData.Get_MessageCode();
	if ( bMessageCode == FC_UNITSWITCHSET )
	{
		SetInOutType(UNIT_TYPE_OUTPUT);
		return TRUE;
	}
	return FALSE;
}


BOOL CBindingRelaisUnit::ConvertToBindingData(void)
{
	BindingData.Clear();
	BindingData.Set_MessageCode(FC_UNITSWITCHSET);

	const BYTE bMethodData = Get_Function();
	BindingData.Set_MethodData(bMethodData);

	// TM 20051108 : Switch Unit must always be an output unit !!!!
	if (GetInOutType() == UNIT_TYPE_INPUT) 
	{
		AfxMessageBox("This type of switch unit cannot be used as an input in a binding.\n"
					  "It can only be used as an output.");
		return FALSE;
	}

	switch(bMethodData)
	{
		case SWITCH_OBJECT_USE_SELECTOR_DATA :
		case SWITCH_OBJECT_ATTR_STATE:
			//ABSORBE
		return FALSE;

		case SWITCH_OBJECT_ATTR_ONOFF:		// Length=2,	0x01, 0/1
		{
			BindingData.Set_Length(2);
			BindingData.Set_Data(1,Get_Status() & 0x01);

		} return TRUE;

		case SWITCH_OBJECT_METHOD_FSONOFF:	// Length=1,	0x04
		case SWITCH_OBJECT_METHOD_OFF:		//				0x02
		case SWITCH_OBJECT_METHOD_ON:		//				0x03
		{
			BindingData.Set_Length(1);

		} return TRUE;

		case SWITCH_OBJECT_ATTR_CONFIG:		// Length=2,	0x06
		{
			BindingData.Set_Length(2);
			BindingData.Set_Data(1,Get_Config());

		} return TRUE;

		case SWITCH_OBJECT_METHOD_FSCVONOFFTIMEDPIR:	// Length =2	0x07,PirTime
		{
			BindingData.Set_Length(2);
			BindingData.Set_Data(1,Get_PirTime());	

		} return TRUE;
		
		case SWITCH_OBJECT_ATTR_STOPTIMER:				// Length =2	0x08,StopTime
		{
			BindingData.Set_Length(2);
			BindingData.Set_Data(1,Get_StopTime());

		} return TRUE;
 
		default:
		{
			ASSERT(0);

		} return FALSE;
	}
	return FALSE;
}

BOOL CBindingRelaisUnit::ConvertFromBindingData(void)
{
	Init();		//TM 20040701

	BYTE bMessageCode = BindingData.Get_MessageCode();
	if (bMessageCode != FC_UNITSWITCHSET) 
	{
		ASSERT(0);
		return FALSE;
	}

	const BYTE bMethodData = BindingData.Get_MethodData();
	Set_Function(bMethodData);

	const BYTE bLength = BindingData.Get_Length();
	switch(bMethodData) 
	{
		case SWITCH_OBJECT_USE_SELECTOR_DATA :
		case SWITCH_OBJECT_ATTR_STATE:
			//ABSORBE
		return FALSE;

		case SWITCH_OBJECT_ATTR_ONOFF:		// Length=2,	0x01, 0/1
		{
			if (bLength != 2) return FALSE;
			Set_Status(BindingData.Get_Data(1));

		} return TRUE;

		case SWITCH_OBJECT_METHOD_FSONOFF:	// Length=1,	0x04
		case SWITCH_OBJECT_METHOD_OFF:		//				0x02
		case SWITCH_OBJECT_METHOD_ON:		//				0x03
		{
			if (bLength != 1) return FALSE;			

		} return TRUE;

		case SWITCH_OBJECT_ATTR_CONFIG:		// Length=2,	0x06
		{
			if (bLength != 2) return FALSE;
			
			Set_Config(BindingData.Get_Data(1));

		} return TRUE;

		case SWITCH_OBJECT_METHOD_FSCVONOFFTIMEDPIR:	// Length =2	0x07,PirTime
		{
			if (bLength != 2) return FALSE;
			Set_PirTime(BindingData.Get_Data(1));

		} return TRUE;
		
		case SWITCH_OBJECT_ATTR_STOPTIMER:				// Length =2	0x08,StopTime
		{
			if (bLength !=2) return FALSE;
			Set_StopTime(BindingData.Get_Data(1));

		} return TRUE;
 
		default:
		{
			ASSERT(0);

		} return FALSE;
	}
	return FALSE;
}


CString CBindingRelaisUnit::ConvertFuntionToString(void)
{	
	CString strMethod;
	CString strData;

	const BYTE bMethodData = Get_Function();
	
	const CBindingStringEntries* const pStringEntries = 
		CBindingUnitStringEntries_Collection::Instance()->Get(SWITCH_STRINGS_EXTENDED);

	ASSERT(pStringEntries);

	pStringEntries->Find(bMethodData,strMethod);

	switch(bMethodData) 
	{
		case SWITCH_OBJECT_USE_SELECTOR_DATA :
		case SWITCH_OBJECT_ATTR_STATE:
		break;

		case SWITCH_OBJECT_ATTR_ONOFF:				
		{
			if ((Get_Status() & 0x01) == 0x01) strData.LoadString(IDS_ON);
			else strData.LoadString(IDS_OFF);		

		} break;

		case SWITCH_OBJECT_METHOD_FSONOFF:	// Length=1,	0x04
		case SWITCH_OBJECT_METHOD_OFF:		//				0x02
		case SWITCH_OBJECT_METHOD_ON:		//				0x03					
		{
			
		} break;

		case SWITCH_OBJECT_ATTR_CONFIG:		// Length=2,	0x06		
		{
			strData.Format("- %d",Get_Config());

		} break;

		case SWITCH_OBJECT_METHOD_FSCVONOFFTIMEDPIR:	// Length =2	0x07,PirTime		
		{
			BYTE bData1 = Get_PirTime();

			if (bData1==255) bData1 = 3;
			
			strData.Format("- %d min.",bData1);

		} break;
		
		case SWITCH_OBJECT_ATTR_STOPTIMER:				// Length =2	0x08,StopTime		
		{
			strData.Format("- %d",Get_StopTime());

		}break;
 
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

// CBindingRelaisUnitInput
// Oppassen: Dit is in feite dezelfde klasse als de CBindingDimUnitInput

CBindingRelaisUnitInput::CBindingRelaisUnitInput(BYTE bBindingUnitType) :
Base(bBindingUnitType),
m_bEvent(EV_UNITCONTROLPULSTOGGLE),
m_bInputState(1)
{
	Init( );
}

CBindingRelaisUnitInput::CBindingRelaisUnitInput(const CBindingUnit& pBindingUnit) :
Base(pBindingUnit),
m_bEvent(EV_UNITCONTROLPULSTOGGLE),
m_bInputState(1)
{
	Init( );
}

CBindingRelaisUnitInput::~CBindingRelaisUnitInput()
{

}

void CBindingRelaisUnitInput::Init(void)
{
	// TODO: Evt opvragen van unitState.

	m_bEvent = EV_UNITCONTROLPULSTOGGLE;
	m_bInputState = 1;

	this->Base::Init( );
}

BOOL CBindingRelaisUnitInput::UpgradeIoType()
{
	const BYTE bMessageCode = BindingData.Get_MessageCode();

	if (bMessageCode == EV_UNITCONTROLPULSTOGGLE)
	{
		SetInOutType(UNIT_TYPE_INPUT);
		return TRUE;
	}

	return this->Base::UpgradeIoType();
}

BOOL CBindingRelaisUnitInput::ConvertToBindingData(void)
{		
	BOOL fResult=FALSE;

	if (this->GetInOutType() == UNIT_TYPE_INPUT) 
	{
		BYTE bTemp=0;
		const BYTE bMessageCode = this->Get_Event();

		this->BindingData.Clear();		
		
		if (bMessageCode == EV_UNITCONTROLPULSTOGGLE)
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

BOOL CBindingRelaisUnitInput::ConvertFromBindingData(void)
{
	BOOL fResult=FALSE;

	if (this->GetInOutType() == UNIT_TYPE_INPUT) 
	{
		Init();			

		const BYTE bMessageCode = BindingData.Get_MessageCode();
		if (bMessageCode == EV_UNITCONTROLPULSTOGGLE)
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


CString CBindingRelaisUnitInput::ConvertFuntionToString(void)
{
	CString strMethod;

	if (this->GetInOutType() == UNIT_TYPE_INPUT) 	
	{				
		const CBindingStringEntries* const pStringEntries = 
			CBindingUnitStringEntries_Collection::Instance()->Get(SWITCH_STRINGS_INPUT);

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
