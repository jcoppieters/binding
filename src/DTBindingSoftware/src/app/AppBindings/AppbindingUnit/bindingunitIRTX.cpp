#include "stdafx.h"
#include "BindingSoftware.h"
#include "bindingunitirtx.h"
#include "bindingunitirtx_def.h"

#include "NodeMess.h"
#include "BindingConstants.h"

#include "bindingUIElement.h"	// ??
#include "bindingunitstate.h"


#include "App\Appbindings\appbindingproperty\BindingPropertyStrings.h"
#include "App\Appbindings\appbindingproperty\BindingPropertyUnit.h"	
#include "App\Appbindings\appbindingproperty\BindingPropertyUnitDefault.h"
#include "App\Appbindings\appbindingproperty\BindingPropertyUnitDefaultFactory.h"

using namespace DUOTECNO::BINDINGS::UI;



CBindingIRTXUnit::CBindingIRTXUnit(BYTE bBindingUnitType) :
Base(bBindingUnitType)
{ 
	SetInOutType(UNIT_TYPE_OUTPUT);

	Init();			
}

CBindingIRTXUnit::CBindingIRTXUnit(const CBindingUnit &BindingUnit) :
CBindingUnit(BindingUnit)
{
	Init();
}

CBindingIRTXUnit::~CBindingIRTXUnit()
{

}

void CBindingIRTXUnit::Init(void)			// TM 20040701
{
	Set_Function(IRTX_OBJECT_METHOD_PULS_IRTX);

	// TM 20071119
	const int IRCode = CBindingUnitState::Instance()
		->get_IRTXUnit()->get();

	Set_IRCode(IRCode);
}

BOOL CBindingIRTXUnit::UpgradeIoType()
{
	const BYTE bMessageCode = BindingData.Get_MessageCode();
	if ( bMessageCode == FC_UNITIRTXSET )
	{
		SetInOutType(UNIT_TYPE_OUTPUT);

		return TRUE;
	}
	return FALSE;
}

BOOL CBindingIRTXUnit::ConvertToBindingData(void)
{
	unsigned short usIRCode;

	BindingData.Clear();
	BindingData.Set_MessageCode(FC_UNITIRTXSET);

	BYTE bMethodData = Get_Function();
	BindingData.Set_MethodData(bMethodData);

	// TM 20051108 : IRTX Unit must always be an output unit !!!!
	if (GetInOutType() == UNIT_TYPE_INPUT) 
	{
		AfxMessageBox("IRTX Unit must always be an output unit");
		return FALSE;
	}

	switch(bMethodData)
	{
		case IRTX_OBJECT_USE_SELECTOR_DATA:
		case IRTX_OBJECT_ATTR_STATE:
			// ABSORBE
		return FALSE;

		case IRTX_OBJECT_METHOD_START_IRTX :
		case IRTX_OBJECT_METHOD_STOP_IRTX :
		case IRTX_OBJECT_METHOD_PULS_IRTX :
		{
			usIRCode = Get_IRCode();

			BindingData.Set_Length(3);
			BindingData.Set_Data(1,usIRCode / 256);
			BindingData.Set_Data(2,usIRCode % 256);

			// TM 20071119
			CBindingUnitState::Instance()->get_IRTXUnit()->set(usIRCode);

		} return TRUE;
			
		case IRTX_OBJECT_ATTR_CONFIG:
		{

			//	BindingData.Set_Length(2);
			//	BindingData.Set_Data(1,Get_Config());

		} return FALSE;

		default:
			ASSERT(0);
		return FALSE;
	}
	return FALSE;
}

BOOL CBindingIRTXUnit::ConvertFromBindingData(void)
{
	Init();			// TM 20040701

	const BYTE bMessageCode = BindingData.Get_MessageCode();
	if (bMessageCode != FC_UNITIRTXSET) 
	{
		ASSERT(0);
		return FALSE;
	}

	const BYTE bMethodData = BindingData.Get_MethodData();
	Set_Function(bMethodData);

	const BYTE bLength = BindingData.Get_Length();
	switch(bMethodData) 
	{
		case IRTX_OBJECT_USE_SELECTOR_DATA:
		case IRTX_OBJECT_ATTR_STATE:
			// ABSORBE
		return FALSE;

		case IRTX_OBJECT_METHOD_START_IRTX :
		case IRTX_OBJECT_METHOD_STOP_IRTX :
		case IRTX_OBJECT_METHOD_PULS_IRTX :
		{
			if (bLength != 3) return FALSE;

			unsigned short usIRCode;
			usIRCode = BindingData.Get_Data(1) * 256;
			usIRCode += BindingData.Get_Data(2);
			Set_IRCode(usIRCode);

		} return TRUE;
			
		case IRTX_OBJECT_ATTR_CONFIG:
		{
			if (bLength != 2) return FALSE;

		} return FALSE;

		default:
			ASSERT(0);
		return FALSE;
	}
	return FALSE;
}

CString CBindingIRTXUnit::ConvertFuntionToString(void)
{
	CString strMethod;
	CString strData;

	const BYTE bMethodData = BindingData.Get_MethodData();

	const CBindingStringEntries* const pStringEntries = 
		CBindingUnitStringEntries_Collection::Instance()->Get(IRTX_STRINGS_EXTENDED);

	ASSERT(pStringEntries);

	pStringEntries->Find(bMethodData,strMethod);

	switch(bMethodData) 
	{
		case IRTX_OBJECT_USE_SELECTOR_DATA:
		case IRTX_OBJECT_ATTR_STATE:
		break;

		case IRTX_OBJECT_METHOD_START_IRTX :
		case IRTX_OBJECT_METHOD_STOP_IRTX :
		case IRTX_OBJECT_METHOD_PULS_IRTX :		
		{
			unsigned short usIRCode;

			usIRCode = BindingData.Get_Data(1) * 256;
			usIRCode += BindingData.Get_Data(2);
			
			strData.Format("- %04d",usIRCode);		

		} break;
			
		case IRTX_OBJECT_ATTR_CONFIG:	
		break;

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