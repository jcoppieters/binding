#include "stdafx.h"
#include "BindingSoftware.h"
#include "bindingunitrc5rx.h"
#include "bindingunitrc5rx_def.h"

#include "NodeMess.h"
#include "BindingConstants.h"
#include "bindingUIElement.h"	// ??
#include "bindingunitstate.h"

#include "App\Appbindings\appbindingproperty\BindingPropertyStrings.h"
#include "App\Appbindings\appbindingproperty\BindingPropertyUnit.h"	
#include "App\Appbindings\appbindingproperty\BindingPropertyUnitDefault.h"
#include "App\Appbindings\appbindingproperty\BindingPropertyUnitDefaultFactory.h"

using namespace DUOTECNO::BINDINGS::UI;


CBindingRC5RXUnit::CBindingRC5RXUnit(BYTE bBindingUnitType) :
Base(bBindingUnitType)
{
	SetInOutType(UNIT_TYPE_INPUT);

	Init();			//TM 20040701
}

CBindingRC5RXUnit::CBindingRC5RXUnit(const CBindingUnit &BindingUnit) :
CBindingUnit(BindingUnit)
{
	Init();	
}

CBindingRC5RXUnit::~CBindingRC5RXUnit()
{

}

void CBindingRC5RXUnit::Init(void)			// TM 20040701
{

	// TM 20071119
	CBindingRC5RXUnitState* const pUnitState = CBindingUnitState::Instance()
		->get_RC5RXUnit();

	if (pUnitState)
	{
		Set_Event(pUnitState->get_Event());	
		Set_IRCode(pUnitState->get_Code());
	}
	else
	{
		Set_Event(RC5RX_EVENT_PULS);	
		Set_IRCode(0);
	}

	Set_State(0);
}

BOOL CBindingRC5RXUnit::UpgradeIoType()
{
	const BYTE bMessageCode = BindingData.Get_MessageCode();

	if ((bMessageCode == EV_UNITCONTROLTOGGLE) || 
		(bMessageCode == EV_UNITCONTROLPULSTOGGLE) ||
		(bMessageCode == EV_UNITCONTROLPULS)) 
	{
		SetInOutType(UNIT_TYPE_INPUT);
		return TRUE;
	}
	return FALSE;
}

BOOL CBindingRC5RXUnit::ConvertToBindingData(void)
{	
	// TM 20051108 : PreConditions - Normally always checked by Links !!
	if (GetInOutType() == UNIT_TYPE_OUTPUT) 
	{
		AfxMessageBox("RC5RX Unit must always be an input unit");
		return FALSE;
	}

	BindingData.Clear();

	switch(Get_Event())
	{
		case RC5RX_EVENT_PULS :		
		{
			BindingData.Set_MessageCode(EV_UNITCONTROLPULS);

			// TM 20071119
			CBindingUnitState::Instance()->get_RC5RXUnit()->set_Event(Get_Event());

		} break;

		case RC5RX_EVENT_PULSTOGGLE :
		{
			BindingData.Set_MessageCode(EV_UNITCONTROLPULSTOGGLE);

			// TM 20071119
			CBindingUnitState::Instance()->get_RC5RXUnit()->set_Event(Get_Event());

		} break;

		case RC5RX_EVENT_TOGGLE :
		{
			BindingData.Set_MessageCode(EV_UNITCONTROLTOGGLE);

			// TM 20071119
			CBindingUnitState::Instance()->get_RC5RXUnit()->set_Event(Get_Event());

		} break;

		default:
		{
			ASSERT(0);

		} return FALSE;
	}

	const BYTE bFlags = (Get_State() & 0x01) << 7;
	const BYTE bIRCode = Get_IRCode() & 0x7F;

	BindingData.Set_BindingFlags(bFlags | bIRCode);
	
	// TM 20071119
	CBindingUnitState::Instance()->get_RC5RXUnit()->set_Code(bIRCode);

	return TRUE;
}

BOOL CBindingRC5RXUnit::ConvertFromBindingData(void)
{
	Init();			//TM 20040701

	const BYTE bMessageCode = BindingData.Get_MessageCode();
	const BYTE bBindingFlags = BindingData.Get_BindingFlags();

	if ((bMessageCode == EV_UNITCONTROLTOGGLE) || 
		(bMessageCode == EV_UNITCONTROLPULSTOGGLE) ||
		(bMessageCode == EV_UNITCONTROLPULS)) 
	{
		Set_Event(bMessageCode);
		Set_IRCode(bBindingFlags & 0x7F);
		return TRUE;
	}
	return FALSE;	
}

CString CBindingRC5RXUnit::ConvertFuntionToString(void)
{
	int i;
	CString strReturn;
	CString strFunction;
	
	strReturn.Empty();
	strFunction.Empty();

	const BYTE bRC5RXEvent = Get_Event();
	for (i=0;i<MAX_RC5RX_FUNCTIONS;i++) 
	{
		if (bRC5RXEvent == RC5RXStrings[i].bMethodData) 
		{
			strFunction.LoadString(RC5RXStrings[i].ulStringID);	
			break;	
		}
	}

	if (i>=MAX_RC5RX_FUNCTIONS) return strReturn;

	strReturn.Format("%s - %d",strFunction,Get_IRCode());	
	return strReturn;
}