#include "stdafx.h"
#include "BindingSoftware.h"
#include "bindingunitvideomux.h"

#include "NodeMess.h"
#include "BindingConstants.h"
#include "bindingUIElement.h"	// ??

#include "App\Appbindings\appbindingproperty\BindingPropertyStrings.h"
#include "App\Appbindings\appbindingproperty\BindingPropertyUnit.h"	
#include "App\Appbindings\appbindingproperty\BindingPropertyUnitDefault.h"
#include "App\Appbindings\appbindingproperty\BindingPropertyUnitDefaultFactory.h"

using namespace DUOTECNO::BINDINGS::UI;


CBindingVideomuxUnit::CBindingVideomuxUnit(BYTE bBindingUnitType) :
Base(bBindingUnitType)
{	
	SetInOutType(UNIT_TYPE_OUTPUT);

	Init();	
}

CBindingVideomuxUnit::CBindingVideomuxUnit(const CBindingUnit &BindingUnit) :
CBindingUnit(BindingUnit)
{
	Init();	
}

CBindingVideomuxUnit::~CBindingVideomuxUnit()
{

}


void CBindingVideomuxUnit::Init(void)			// TM 20040701
{		
	Set_Function(VIDEOMUX_OBJECT_METHOD_CONNECT);
	Set_Input(0);
	Set_Output(0);
	Set_Config(0);
	Set_State(0);
	Set_Keypress(0);
	Set_ButtonNr(0);
	Set_DvmrFunction(0);
}

BOOL CBindingVideomuxUnit::UpgradeIoType()
{
	const BYTE bMessageCode = BindingData.Get_MessageCode();

	if (bMessageCode == FC_UNITVIDEOMUXSET)
	{
		SetInOutType(UNIT_TYPE_OUTPUT);
		return TRUE;
	}
	return FALSE;
}

BOOL CBindingVideomuxUnit::ConvertToBindingData(void)
{
	BindingData.Clear();
	BindingData.Set_MessageCode(FC_UNITVIDEOMUXSET);

	const BYTE bMethodData = Get_Function();
	BindingData.Set_MethodData(bMethodData);

	// TM 20051108 : Videomux Unit must always be an output unit !!!!
	if (GetInOutType() == UNIT_TYPE_INPUT) 
	{
		AfxMessageBox("Videomux Unit must always be an output unit");
		return FALSE;
	}

	switch (bMethodData)
	{		
		case VIDEOMUX_OBJECT_ATTR_CONNECTION :
		{
			BindingData.Set_Length(4);
			BindingData.Set_Data(1,Get_Output());
			BindingData.Set_Data(2,Get_Input());
			BindingData.Set_Data(3,Get_State() & 0x01);

		} return TRUE;

		case VIDEOMUX_OBJECT_METHOD_CONNECT :
		case VIDEOMUX_OBJECT_METHOD_DISCONNECT :
		{
			BindingData.Set_Length(3);
			BindingData.Set_Data(1,Get_Output());
			BindingData.Set_Data(2,Get_Input());

		} return TRUE;

		case VIDEOMUX_OBJECT_METHOD_DISCONNECT_ALL :
		{
			BindingData.Set_Length(1);

		} return TRUE;

		case VIDEOMUX_OBJECT_METHOD_KEYPRESS:
		{
			BindingData.Set_Length(2);
			BindingData.Set_Data(1,Get_Keypress());

		} return TRUE;

		case VIDEOMUX_OBJECT_METHOD_KEYPRESS_CAMERA :
		{
			BindingData.Set_Length(2);
			BindingData.Set_Data(1,Get_ButtonNr());

		} return TRUE;

		case VIDEOMUX_OBJECT_METHOD_DVMR :
		{
			BindingData.Set_Length(2);
			BindingData.Set_Data(1,Get_DvmrFunction());

		} return TRUE;

		default:
			ASSERT(0);
		return FALSE;

	}// end switch()

	return FALSE;
}

BOOL CBindingVideomuxUnit::ConvertFromBindingData(void)
{
	Init();			

	const BYTE bMessageCode = BindingData.Get_MessageCode();
	if (bMessageCode != FC_UNITVIDEOMUXSET) 
	{
		ASSERT(0);
		return FALSE;
	}

	const BYTE bMethodData = BindingData.Get_MethodData();
	Set_Function(bMethodData);
	
	const BYTE bLength = BindingData.Get_Length();

	switch (bMethodData)
	{		
		case VIDEOMUX_OBJECT_ATTR_CONNECTION :
		{
			if (bLength != 4) return FALSE;

			Set_Output(BindingData.Get_Data(1));
			Set_Input(BindingData.Get_Data(2));
			Set_State(BindingData.Get_Data(3) & 0x01);		
		} return TRUE;

		case VIDEOMUX_OBJECT_METHOD_CONNECT :
		case VIDEOMUX_OBJECT_METHOD_DISCONNECT :
		{
			if (bLength != 3) return FALSE;

			Set_Output(BindingData.Get_Data(1));
			Set_Input(BindingData.Get_Data(2));	
		} return TRUE;

		case VIDEOMUX_OBJECT_METHOD_DISCONNECT_ALL :
		{
			if (bLength != 1) return FALSE;

		} return TRUE;
		
		case VIDEOMUX_OBJECT_METHOD_KEYPRESS:
		{
			if (bLength != 2) return FALSE;

			Set_Keypress(BindingData.Get_Data(1));

		} return TRUE;

		case VIDEOMUX_OBJECT_METHOD_KEYPRESS_CAMERA :
		{
			if (bLength != 2) return FALSE;

			Set_ButtonNr(BindingData.Get_Data(1));		

		} return TRUE;

		case VIDEOMUX_OBJECT_METHOD_DVMR :
		{
			if (bLength != 2) return FALSE;

			Set_DvmrFunction(BindingData.Get_Data(1));			

		} return TRUE;

		default :
			ASSERT(0);
		return FALSE;
	}// end switch ...

	return FALSE;
}

CString CBindingVideomuxUnit::ConvertFuntionToString(void)
{
	CString strMethod;
	CString strData;

	const BYTE bMethodData = Get_Function();
	const CBindingStringEntries* const pStringEntries = 
		CBindingUnitStringEntries_Collection::Instance()->Get(VIDEOMUX_STRINGS_EXTENDED);

	ASSERT(pStringEntries);
	pStringEntries->Find(bMethodData,strMethod);

	switch(bMethodData) 
	{
		case VIDEOMUX_OBJECT_ATTR_CONNECTION :
		{
			strData.Format("- %d,%d,%d",Get_Output(),Get_Input(),Get_State());		

		} break;

		case VIDEOMUX_OBJECT_METHOD_CONNECT :
		case VIDEOMUX_OBJECT_METHOD_DISCONNECT :
		{
			strData.Format("- %d,%d",Get_Output(),Get_Input());	

		} break;

		case VIDEOMUX_OBJECT_METHOD_DISCONNECT_ALL :		
		break;

		case VIDEOMUX_OBJECT_METHOD_KEYPRESS:
		{
			strData.Format("- %d",Get_Keypress());

		} break;	//return TRUE;		// CHANGES_MVS_2008

		case VIDEOMUX_OBJECT_METHOD_KEYPRESS_CAMERA :
		{
			strData.Format("- %d",Get_ButtonNr());

		} break;	// return TRUE;		// CHANGES_MVS_2008

		case VIDEOMUX_OBJECT_METHOD_DVMR :
		{
			strData.Format("- %d",Get_DvmrFunction());

		} break;	// return TRUE;		// CHANGES_MVS_2008

		default :
			ASSERT(0);
		break;
	}// end switch ...

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