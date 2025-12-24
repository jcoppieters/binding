#include "stdafx.h"
#include "BindingSoftware.h"
#include "bindingunitcontrol.h"

#include "NodeMess.h"
#include "BindingConstants.h"
#include "bindingUIElement.h"	// ??

#include "App\Appbindings\appbindingproperty\BindingPropertyStrings.h"
#include "App\Appbindings\appbindingproperty\BindingPropertyUnit.h"	
#include "App\Appbindings\appbindingproperty\BindingPropertyUnitDefault.h"
#include "App\Appbindings\appbindingproperty\BindingPropertyUnitDefaultFactory.h"

using namespace DUOTECNO::BINDINGS::UI;


CBindingControlUnit::CBindingControlUnit(BYTE bBindingUnitType) :
Base(bBindingUnitType)
{
	SetInOutType(UNIT_TYPE_INPUT);

	Init();			//TM 20040701
}

CBindingControlUnit::CBindingControlUnit(const CBindingUnit &BindingUnit) :
CBindingUnit(BindingUnit)
{
	Init();
}

CBindingControlUnit::~CBindingControlUnit()
{

}


void CBindingControlUnit::Init(void)			// TM 20040701
{
	Set_Event(EV_UNITCONTROLPULS);
	Set_ControlState(0);
}

BOOL CBindingControlUnit::UpgradeIoType()
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


BOOL CBindingControlUnit::ConvertToBindingData(void)
{
	// TM 20051108 : PreConditions - Normally always checked by Links !!
	if (GetInOutType() == UNIT_TYPE_OUTPUT) 
	{
		AfxMessageBox("Control Unit must always be an input unit");
		return FALSE;
	}

	BindingData.Clear();
	BindingData.Set_MessageCode(Get_Event());
	BindingData.Set_BindingFlags((Get_ControlState()&0x01)<<7);
	return TRUE;
}

BOOL CBindingControlUnit::ConvertFromBindingData(void)
{
	Init();			//TM 20040701

	const BYTE bMessageCode = BindingData.Get_MessageCode();
	if ((bMessageCode == EV_UNITCONTROLTOGGLE) || 
		(bMessageCode == EV_UNITCONTROLPULSTOGGLE) ||
		(bMessageCode == EV_UNITCONTROLPULS)) {
		Set_Event(bMessageCode);
		return TRUE;
	}
	return FALSE;
}

// Changes TM,V16.46: 
// De string aanmaken ifv het bControlEvent ipv BindingData::Get_MessageCode()
// Nodig om de tekst weer te geven via de tooltips na verandering in de property dialogen.
CString CBindingControlUnit::ConvertFuntionToString(void)
{
	int i;
	CString strReturn;
	// const BYTE bControlEvent = BindingData.Get_MessageCode();
	const BYTE bMessageCode = this->CBindingControlUnit::Get_Event();		// Sedert V16.46: Nodig om de tekst weer te geven via de tooltips na verandering in de property dialogen.
	for (i=0;i<MAX_CONTROL_FUNCTIONS;i++)  {
		if (bControlEvent == ControlStrings[i].bMethodData) {
			strReturn.LoadString(ControlStrings[i].ulStringID);
			break;	
		}		
	}
	return strReturn;
}

