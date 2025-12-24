#include "stdafx.h"
#include "BindingSoftware.h"
#include "bindingunitvirtual.h"

#include "NodeMess.h"
#include "BindingConstants.h"
#include "bindingUIElement.h"	// ??

#include "App\Appbindings\appbindingproperty\BindingPropertyStrings.h"
#include "App\Appbindings\appbindingproperty\BindingPropertyUnit.h"	
#include "App\Appbindings\appbindingproperty\BindingPropertyUnitDefault.h"
#include "App\Appbindings\appbindingproperty\BindingPropertyUnitDefaultFactory.h"

using namespace DUOTECNO::BINDINGS::UI;


CBindingVirtualUnit::CBindingVirtualUnit(BYTE bBindingUnitType) :
Base(bBindingUnitType)
{ 
	SetInOutType(UNIT_TYPE_INPUT);

	Init();			//20040701
}

CBindingVirtualUnit::CBindingVirtualUnit(const CBindingUnit &BindingUnit) :
CBindingUnit(BindingUnit)
{
	Init();
}

CBindingVirtualUnit::~CBindingVirtualUnit()
{

}


void CBindingVirtualUnit::Init(void)			// TM 20040701
{
	Set_Event(EV_UNITCONTROLPULS);
	Set_ControlState(0);
	Set_Function(VIRTUAL_OBJECT_METHOD_PULS);
}

BOOL CBindingVirtualUnit::UpgradeIoType()
{
	const BYTE bMessageCode = BindingData.Get_MessageCode();

	if ((bMessageCode == EV_UNITCONTROLTOGGLE) || 
		(bMessageCode == EV_UNITCONTROLPULSTOGGLE) ||
		(bMessageCode == EV_UNITCONTROLPULS)) 
	{
		SetInOutType(UNIT_TYPE_INPUT);
		return TRUE;
	}
	else if (bMessageCode == FC_UNITLCDVIRTUALSET)
	{
		SetInOutType(UNIT_TYPE_OUTPUT);
		return TRUE;
	}
	return FALSE;
}

BOOL CBindingVirtualUnit::ConvertToBindingData(void)
{
	BYTE bMethodData;
	BindingData.Clear();

	switch(GetInOutType())
	{
		case UNIT_TYPE_INPUT:
		{
			BindingData.Set_MessageCode(Get_Event());
			BindingData.Set_BindingFlags((Get_ControlState()&0x01)<<7);

		} return TRUE;

		case UNIT_TYPE_OUTPUT:
		{
			BindingData.Set_MessageCode(FC_UNITLCDVIRTUALSET);
			bMethodData = Get_Function();
			BindingData.Set_MethodData(bMethodData);

			switch(bMethodData)
			{
				case VIRTUAL_OBJECT_METHOD_PULS:
				{
					BindingData.Set_Length(1);

				} return TRUE;

				case VIRTUAL_OBJECT_METHOD_TOGGLE:
				case VIRTUAL_OBJECT_METHOD_PULSTOGGLE:
				{
					BindingData.Set_Length(2);
					BindingData.Set_Data(1,Get_ControlState() & 0x01);

				} return TRUE;

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

BOOL CBindingVirtualUnit::ConvertFromBindingData(void)
{
	Init();		//TM 20040701

	const BYTE bMessageCode = BindingData.Get_MessageCode();
	
	if (GetInOutType() == UNIT_TYPE_INPUT) 
	{ 
		if ((bMessageCode == EV_UNITCONTROLTOGGLE) || 
			(bMessageCode == EV_UNITCONTROLPULSTOGGLE) ||
			(bMessageCode == EV_UNITCONTROLPULS)) 
		{
			Set_Event(bMessageCode);
			return TRUE;
		}
		return FALSE;
	}
	else 
	{
		if (bMessageCode != FC_UNITLCDVIRTUALSET) 
		{
			ASSERT(0);
			return FALSE;
		}

		const BYTE bMethodData = BindingData.Get_MethodData();
		Set_Function(bMethodData);
		
		const BYTE bLength = BindingData.Get_Length();
		switch(bMethodData) 
		{
			case VIRTUAL_OBJECT_METHOD_PULS:
			{
				if (bLength !=1) return FALSE;

			} return TRUE;

			case VIRTUAL_OBJECT_METHOD_TOGGLE:
			case VIRTUAL_OBJECT_METHOD_PULSTOGGLE:
			{
				if (bLength != 2) return FALSE;

				Set_ControlState(BindingData.Get_Data(1));

			} return TRUE;

			default:
			{
				ASSERT(0);

			} return FALSE;		
		}
	}
	return FALSE;
}


// Changes TM,V16.46: 
// Nodig om de tekst weer te geven via de tooltips na verandering in de property dialogen.
// Wanneer de virtual als input staat dan gaan we string aanmaken ifv het bControlEvent ipv BindingData::Get_MessageCode()
CString CBindingVirtualUnit::ConvertFuntionToString(void)
{
	BYTE i;
	CString strReturn;
	CString strFunction;
	CString strTemp;

	if (GetInOutType() == UNIT_TYPE_INPUT) { 
		// const BYTE bMessageCode = BindingData.Get_MessageCode();		
		const BYTE bMessageCode = this->CBindingVirtualUnit::Get_Event();	// Sedert V16.46: Nodig om de tekst weer te geven via de tooltips na verandering in de property dialogen.
		for (i=0;i<MAX_CONTROL_FUNCTIONS;i++) {
			if (bMessageCode == ControlStrings[i].bMethodData) {
				strReturn.LoadString(ControlStrings[i].ulStringID);
				return strReturn;
			}		
		}
	}
	else {		
		const BYTE bMethodData = Get_Function();
		for (i=0;i<MAX_VIRTUALOUTPUT_FUNCTIONS;i++) {
			if (bMethodData == VirtualOutputStrings[i].bMethodData) {
				strFunction.LoadString(VirtualOutputStrings[i].ulStringID);
				break;	
			}
		}

		if (i>=MAX_VIRTUALOUTPUT_FUNCTIONS) 
			return strReturn;

		switch(bMethodData) {
			case VIRTUAL_OBJECT_METHOD_PULS:
			break;			
			case VIRTUAL_OBJECT_METHOD_PULSTOGGLE:
			case VIRTUAL_OBJECT_METHOD_TOGGLE: {
				if ((Get_ControlState() & 0x01) == 0x01) strTemp.LoadString(IDS_ON);
				else strTemp.LoadString(IDS_OFF);

				strReturn.Format("%s - %s",strFunction,strTemp);

			} return strReturn;
		}			
	}
	strReturn.Format("%s",strFunction);
	return strReturn;
}