#include "stdafx.h"
#include "BindingSoftware.h"
#include "bindingunitaudiobasic.h"

#include "NodeMess.h"
#include "BindingConstants.h"

#include "bindingUIElement.h"	// ??

#include "App\Appbindings\appbindingproperty\BindingPropertyStrings.h"
#include "App\Appbindings\appbindingproperty\BindingPropertyUnit.h"	
#include "App\Appbindings\appbindingproperty\BindingPropertyUnitDefault.h"
#include "App\Appbindings\appbindingproperty\BindingPropertyUnitDefaultFactory.h"


using namespace DUOTECNO::BINDINGS::UI;




CBindingBasicAudioUnit::CBindingBasicAudioUnit(BYTE bBindingUnitType) :
Base(bBindingUnitType)
{ 
	SetInOutType(UNIT_TYPE_OUTPUT);

	Init();		//TM 20040701
}

CBindingBasicAudioUnit::CBindingBasicAudioUnit(const CBindingUnit &BindingUnit) :
CBindingUnit(BindingUnit)
{
	Init();
}

CBindingBasicAudioUnit::~CBindingBasicAudioUnit()
{

}

void CBindingBasicAudioUnit::Init(void)			// TM 20040701
{
	Set_Function(AUDIO_OBJECT_METHOD_FSVOLUPDOWN);
	Set_Source(0);			
	Set_State(0);				
	Set_Config(0);
	Set_SrcFunction(0);
	Set_DestFunction(0);
}

BOOL CBindingBasicAudioUnit::UpgradeIoType()
{
	const BYTE bMessageCode = BindingData.Get_MessageCode();
	if ( bMessageCode == FC_UNITAUDIOSET )
	{
		SetInOutType(UNIT_TYPE_OUTPUT);
		return TRUE;
	}
	return FALSE;
}

BOOL CBindingBasicAudioUnit::ConvertToBindingData(void)
{
	BindingData.Clear();
	BindingData.Set_MessageCode(FC_UNITAUDIOSET);

	const BYTE bMethodData = Get_Function();
	BindingData.Set_MethodData(bMethodData);

	// TM 20051108 : Basic AudioRoom must always be an output unit !!!!
	if (GetInOutType() == UNIT_TYPE_INPUT) 
	{
		AfxMessageBox("Audio unit must always be an output unit");
		return FALSE;
	}

	switch(bMethodData)
	{	
		case AUDIO_OBJECT_USE_SELECTOR_DATA :
		case AUDIO_OBJECT_ATTR_STATE:
		return FALSE;

		case AUDIO_OBJECT_ATTR_SOURCE_ON:
		{
			BindingData.Set_Length(2);
			BindingData.Set_Data(1,Get_Source());

		} return TRUE;

		case AUDIO_OBJECT_METHOD_SOURCEINC:
		case AUDIO_OBJECT_METHOD_SOURCEDEC:
		case AUDIO_OBJECT_METHOD_CSON:
		case AUDIO_OBJECT_METHOD_OFF:
		case AUDIO_OBJECT_METHOD_FSCSONOFF:
		case AUDIO_OBJECT_METHOD_VOLUP:
		case AUDIO_OBJECT_METHOD_VOLDOWN:
		case AUDIO_OBJECT_METHOD_FSVOLUPDOWN:
		case AUDIO_OBJECT_METHOD_PRESUP:
		case AUDIO_OBJECT_METHOD_PRESDOWN:
		{
			BindingData.Set_Length(1);

		} return TRUE;

		case AUDIO_OBJECT_ATTR_ONOFF:
		{
			BindingData.Set_Length(2);
			BindingData.Set_Data(1,Get_State() & 0x01);

		} return TRUE;

		case AUDIO_OBJECT_ATTR_CONFIG:
		{
			BindingData.Set_Length(2);
			BindingData.Set_Data(1,Get_Config());

		} return TRUE;

		case AUDIO_OBJECT_METHOD_SOURCE_FUNCTION:
		{
			BindingData.Set_Length(2);
			BindingData.Set_Data(1,Get_SrcFunction() & 0x01);

		} return TRUE;

		case AUDIO_OBJECT_METHOD_DESTINATION_FUNCTION:
		{
			BindingData.Set_Length(2);
			BindingData.Set_Data(1,Get_DestFunction() & 0x01);

		} return TRUE;
		
		default:
		{
			ASSERT(0);

		} return FALSE;
	}
	return FALSE;
}

BOOL CBindingBasicAudioUnit::ConvertFromBindingData(void)
{
	Init();		//TM 20040701
	
	const BYTE bMessageCode = BindingData.Get_MessageCode();
	if (bMessageCode != FC_UNITAUDIOSET) 
	{
		ASSERT(0);
		return FALSE;
	}

	const BYTE bMethodData = BindingData.Get_MethodData();
	Set_Function(bMethodData);

	const BYTE bLength = BindingData.Get_Length();
	switch(bMethodData) 
	{
		case AUDIO_OBJECT_USE_SELECTOR_DATA :
		case AUDIO_OBJECT_ATTR_STATE:
		return FALSE;

		case AUDIO_OBJECT_ATTR_SOURCE_ON:
		{
			if (bLength != 2) return FALSE;
			Set_Source(BindingData.Get_Data(1));

		} return TRUE;

		case AUDIO_OBJECT_METHOD_SOURCEINC:
		case AUDIO_OBJECT_METHOD_SOURCEDEC:
		case AUDIO_OBJECT_METHOD_CSON:
		case AUDIO_OBJECT_METHOD_OFF:
		case AUDIO_OBJECT_METHOD_FSCSONOFF:
		case AUDIO_OBJECT_METHOD_VOLUP:
		case AUDIO_OBJECT_METHOD_VOLDOWN:
		case AUDIO_OBJECT_METHOD_FSVOLUPDOWN:
		case AUDIO_OBJECT_METHOD_PRESUP:
		case AUDIO_OBJECT_METHOD_PRESDOWN:
		{
			if(bLength != 1) return FALSE;

		} return TRUE;

		case AUDIO_OBJECT_ATTR_ONOFF:
		{
			if (bLength != 2) return FALSE;

			Set_State(BindingData.Get_Data(1));

		} return TRUE;

		case AUDIO_OBJECT_ATTR_CONFIG:
		{
			if (bLength != 2) return FALSE;

			Set_Config(BindingData.Get_Data(1));

		} return TRUE;

		case AUDIO_OBJECT_METHOD_SOURCE_FUNCTION:
		{
			if (bLength != 2) return FALSE;

			Set_SrcFunction(BindingData.Get_Data(1));

		} return TRUE;

		case AUDIO_OBJECT_METHOD_DESTINATION_FUNCTION:
		{
			if (bLength != 2) return FALSE;

			Set_DestFunction(BindingData.Get_Data(1));

		} return TRUE;
		
		default:
		{
			ASSERT(0);

		} return FALSE;
	}

	return FALSE;
}

CString CBindingBasicAudioUnit::ConvertFuntionToString(void)
{
	CString strReturn;
	CString strFunction;
	CString strTemp;

	const BYTE bMethodData = Get_Function();

	const CBindingStringEntries* const pStringEntries = 
		CBindingUnitStringEntries_Collection::Instance()->Get(BOSELF48_STRINGS_EXTENDED);

	ASSERT(pStringEntries);

	pStringEntries->Find(bMethodData,strFunction);
	
	switch(bMethodData)
	{
		case AUDIO_OBJECT_ATTR_SOURCE_ON:				
		{
			strReturn.Format("%s - Source %d",strFunction,Get_Source());				

		} return strReturn;

		case AUDIO_OBJECT_METHOD_SOURCEINC:
		case AUDIO_OBJECT_METHOD_SOURCEDEC:
		case AUDIO_OBJECT_METHOD_CSON:
		case AUDIO_OBJECT_METHOD_OFF:
		case AUDIO_OBJECT_METHOD_FSCSONOFF:
		case AUDIO_OBJECT_METHOD_VOLUP:
		case AUDIO_OBJECT_METHOD_VOLDOWN:
		case AUDIO_OBJECT_METHOD_FSVOLUPDOWN:
		case AUDIO_OBJECT_METHOD_PRESUP:
		case AUDIO_OBJECT_METHOD_PRESDOWN:			
		break;

		case AUDIO_OBJECT_ATTR_ONOFF:
		{
			if (Get_State() & 0x01) strTemp.LoadString(IDS_ON);
			else strTemp.LoadString(IDS_OFF);

			strReturn.Format("%s - %s",strFunction,strTemp);

		} return strReturn;
		 			
		case AUDIO_OBJECT_METHOD_DESTINATION_FUNCTION:					
		{
			strTemp.LoadString(IDS_FUNCTION);
			strReturn.Format("%s - %s %d",strFunction,strTemp,
										  Get_DestFunction() & IRAUDIO_FUNCTION_MASK);

		} return strReturn;
							
		case AUDIO_OBJECT_METHOD_SOURCE_FUNCTION:
		{
			strTemp.LoadString(IDS_FUNCTION);
			strReturn.Format("%s - %s %d",strFunction,strTemp,
										  Get_SrcFunction() & IRAUDIO_FUNCTION_MASK);

		} return strReturn;

		case AUDIO_OBJECT_ATTR_TUNERFREQ:	
		case AUDIO_OBJECT_ATTR_DISC:
		case AUDIO_OBJECT_ATTR_STATE:
		case AUDIO_OBJECT_USE_SELECTOR_DATA:				
		case AUDIO_OBJECT_ATTR_CONFIG:
		default:				
		break;	
	}
	return strFunction;
}

