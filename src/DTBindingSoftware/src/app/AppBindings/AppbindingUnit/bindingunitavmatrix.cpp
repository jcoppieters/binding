#include "stdafx.h"
#include "BindingSoftware.h"
#include "bindingunitavmatrix.h"

#include "NodeMess.h"
#include "BindingConstants.h"
#include "bindingUIElement.h"	// ??
#include "bindingunitstate.h"

#include "App\Appbindings\appbindingproperty\BindingPropertyStrings.h"
#include "App\Appbindings\appbindingproperty\BindingPropertyUnit.h"	
#include "App\Appbindings\appbindingproperty\BindingPropertyUnitDefault.h"
#include "App\Appbindings\appbindingproperty\BindingPropertyUnitDefaultFactory.h"


using namespace DUOTECNO::BINDINGS::UI;


CBindingAVMatrixUnit::CBindingAVMatrixUnit(BYTE bBindingUnitType) :
Base(bBindingUnitType)
{
	SetInOutType(UNIT_TYPE_OUTPUT);

	Init();	
}

CBindingAVMatrixUnit::CBindingAVMatrixUnit(const CBindingUnit &BindingUnit) :
CBindingUnit(BindingUnit)
{
	Init();	
}

CBindingAVMatrixUnit::~CBindingAVMatrixUnit()
{

}

void CBindingAVMatrixUnit::Init(void)			
{		
	Set_Function(AVMATRIX_OBJECT_METHOD_MUTE_UNMUTE);
	Set_Volume(25);
	Set_Bass(25);
	Set_Treble(25);
	Set_Input(0);
	Set_Output(0);
	Set_Config(0);
	Set_State(0);
	Set_MacroCode(0);
	Set_DestMasterIdx(0);
	Set_DestSlaveIdx(0xFF);
}

BOOL CBindingAVMatrixUnit::UpgradeIoType()
{
	const BYTE bMessageCode = BindingData.Get_MessageCode();

	if (bMessageCode == FC_UNITAVMATRIXSET)
	{
		SetInOutType(UNIT_TYPE_OUTPUT);
		return TRUE;
	}
	return FALSE;
}

BOOL CBindingAVMatrixUnit::ConvertToBindingData(void)
{
	BindingData.Clear();
	BindingData.Set_MessageCode(FC_UNITAVMATRIXSET);

	const BYTE bMethodData = Get_Function();
	BindingData.Set_MethodData(bMethodData);

	// TM 20051108 : AVMatrix Unit must always be an output unit !!!!
	if (GetInOutType() == UNIT_TYPE_INPUT) 
	{
		AfxMessageBox("AvMatrix Unit must always be an output unit");
		return FALSE;
	}

	switch (bMethodData)
	{		
		case AVMATRIX_OBJECT_ATTR_CONNECTION :
		{
			BindingData.Set_Length(4);
			BindingData.Set_Data(1,Get_Output());
			BindingData.Set_Data(2,Get_Input());
			BindingData.Set_Data(3,Get_State() & 0x01);
		} return TRUE;

		case AVMATRIX_OBJECT_METHOD_CONNECT :
		case AVMATRIX_OBJECT_METHOD_DISCONNECT :
		{
			BindingData.Set_Length(3);
			BindingData.Set_Data(1,Get_Output());
			BindingData.Set_Data(2,Get_Input());		

		} return TRUE;

		case AVMATRIX_OBJECT_ATTR_VOLUME :
		{
			BindingData.Set_Length(3);
			BindingData.Set_Data(1,Get_Output());
			BindingData.Set_Data(2,Get_Volume());

		} return TRUE;

		case AVMATRIX_OBJECT_ATTR_BASS :
		{
			BindingData.Set_Length(3);
			BindingData.Set_Data(1,Get_Output());
			BindingData.Set_Data(2,Get_Bass());

		} return TRUE;

		case AVMATRIX_OBJECT_ATTR_TREBLE :
		{
			BindingData.Set_Length(3);
			BindingData.Set_Data(1,Get_Output());
			BindingData.Set_Data(2,Get_Treble());

		} return TRUE;

		case AVMATRIX_OBJECT_METHOD_VOLUME_INC :
		case AVMATRIX_OBJECT_METHOD_VOLUME_DEC :
		{
			BindingData.Set_Length(2);
			BindingData.Set_Data(1,Get_Output());

		} return TRUE;

		case AVMATRIX_OBJECT_METHOD_DISCONNECT_ALL :
		{
			BindingData.Set_Length(1);

		} return TRUE;

		case AVMATRIX_OBJECT_METHOD_VOLUME_UP:
		case AVMATRIX_OBJECT_METHOD_VOLUME_DOWN:
		case AVMATRIX_OBJECT_METHOD_VOLUME_STOP :
		case AVMATRIX_OBJECT_METHOD_MUTE_UNMUTE :
		{
			BindingData.Set_Length(2);
			BindingData.Set_Data(1,Get_Output());

		} return TRUE;

		case AVMATRIX_OBJECT_METHOD_MACRO:
		{
			const unsigned short usTemp = Get_MacroCode();

			BindingData.Set_Length(4);
			BindingData.Set_Data(1,Get_Output());			
			BindingData.Set_Data(2,usTemp/256);
			BindingData.Set_Data(3,usTemp%256);

		} return TRUE;

		case AVMATRIX_OBJECT_ATTR_ZONEGROUP_SET:
		case AVMATRIX_OBJECT_ATTR_ZONEGROUP_SLAVEADD:
		case AVMATRIX_OBJECT_ATTR_ZONEGROUP_SLAVEREMOVE:
		{
			BindingData.Set_Length(4);
			BindingData.Set_Data(1,Get_Output());
			BindingData.Set_Data(2,Get_DestMasterIdx());
			BindingData.Set_Data(3,Get_DestSlaveIdx());

		} return TRUE;

		default :
		{
			ASSERT(0);

		} return FALSE;

	}// end switch ...
}

BOOL CBindingAVMatrixUnit::ConvertFromBindingData(void)
{
	Init();			

	const BYTE bMessageCode = BindingData.Get_MessageCode();

	if (bMessageCode != FC_UNITAVMATRIXSET) 
	{
		ASSERT(0);
		return FALSE;
	}

	const BYTE bMethodData = BindingData.Get_MethodData();
	Set_Function(bMethodData);

	const BYTE bLength = BindingData.Get_Length();

	switch (bMethodData)
	{		
		case AVMATRIX_OBJECT_ATTR_CONNECTION :
		{
			if (bLength != 4) return FALSE;

			Set_Output(BindingData.Get_Data(1));
			Set_Input(BindingData.Get_Data(2));
			Set_State(BindingData.Get_Data(3) & 0x01);		

		} return TRUE;

		case AVMATRIX_OBJECT_METHOD_CONNECT :
		case AVMATRIX_OBJECT_METHOD_DISCONNECT :
		{
			if (bLength != 3) return FALSE;

			Set_Output(BindingData.Get_Data(1));
			Set_Input(BindingData.Get_Data(2));	

		} return TRUE;

		case AVMATRIX_OBJECT_ATTR_VOLUME :
		{
			if (bLength != 3) return FALSE;

			Set_Output(BindingData.Get_Data(1));
			Set_Volume(BindingData.Get_Data(2));

		} return TRUE;

		case AVMATRIX_OBJECT_ATTR_BASS :
		{
			if (bLength != 3) return FALSE;

			Set_Output(BindingData.Get_Data(1));
			Set_Bass(BindingData.Get_Data(2));		

		} return TRUE;

		case AVMATRIX_OBJECT_ATTR_TREBLE :
		{
			if (bLength != 3) return FALSE;

			Set_Output(BindingData.Get_Data(1));
			Set_Treble(BindingData.Get_Data(2));	

		} return TRUE;

		case AVMATRIX_OBJECT_METHOD_VOLUME_INC :
		case AVMATRIX_OBJECT_METHOD_VOLUME_DEC :
		{
			if (bLength != 2) return FALSE;

			Set_Output(BindingData.Get_Data(1));
		} return TRUE;

		case AVMATRIX_OBJECT_METHOD_DISCONNECT_ALL :
		{
			if (bLength != 1) return FALSE;
		} return TRUE;

		case AVMATRIX_OBJECT_METHOD_VOLUME_UP:
		case AVMATRIX_OBJECT_METHOD_VOLUME_DOWN:
		case AVMATRIX_OBJECT_METHOD_VOLUME_STOP :
		case AVMATRIX_OBJECT_METHOD_MUTE_UNMUTE :
		{
			if (bLength != 2) return FALSE;

			Set_Output(BindingData.Get_Data(1));
		} return TRUE;

		case AVMATRIX_OBJECT_METHOD_MACRO:
		{
			if (bLength != 4) return FALSE;

			Set_Output(BindingData.Get_Data(1));		
			Set_MacroCode((BindingData.Get_Data(2)*256)+BindingData.Get_Data(3));		
		} return TRUE;

		case AVMATRIX_OBJECT_ATTR_ZONEGROUP_SET:
		case AVMATRIX_OBJECT_ATTR_ZONEGROUP_SLAVEADD:
		case AVMATRIX_OBJECT_ATTR_ZONEGROUP_SLAVEREMOVE:
		{
			if (bLength != 4) return FALSE;

			this->Set_Output(BindingData.Get_Data(1));
			this->Set_DestMasterIdx(BindingData.Get_Data(2));
			this->Set_DestSlaveIdx(BindingData.Get_Data(3));
		} return TRUE;

		case AVMATRIX_OBJECT_ATTR_CONFIG:
		{
			if (bLength != 2) return FALSE;
			this->Set_Config(BindingData.Get_Data(1));
		} return TRUE;
		default:
		{
			ASSERT(0);
		} return FALSE;

	}// end switch ...

	return FALSE;
}

CString CBindingAVMatrixUnit::ConvertFuntionToString(void)
{
	CString strMethod;
	CString strData;

	const BYTE bMethodData = Get_Function();
	const CBindingStringEntries* const pStringEntries = 
		CBindingUnitStringEntries_Collection::Instance()->Get(AVMATRIX_STRINGS_EXTENDED);

	ASSERT(pStringEntries);
	pStringEntries->Find(bMethodData,strMethod);

	switch(bMethodData) 
	{
		case AVMATRIX_OBJECT_ATTR_CONNECTION :
		{
			strData.Format("- %d,%d,%d",Get_Output(),Get_Input(),Get_State());		

		} break;

		case AVMATRIX_OBJECT_METHOD_CONNECT :
		case AVMATRIX_OBJECT_METHOD_DISCONNECT :
		{
			strData.Format("- %d,%d",Get_Output(),Get_Input());	

		} break;

		case AVMATRIX_OBJECT_ATTR_VOLUME :
		{
			strData.Format("- %d,%d",Get_Output(),Get_Volume());	

		} break;

		case AVMATRIX_OBJECT_ATTR_BASS :
		{
			strData.Format("- %d,%d",Get_Output(),Get_Bass());	

		} break;

		case AVMATRIX_OBJECT_ATTR_TREBLE :
		{
			strData.Format("- %d,%d",Get_Output(),Get_Treble());		

		} break;

		case AVMATRIX_OBJECT_METHOD_VOLUME_INC :
		case AVMATRIX_OBJECT_METHOD_VOLUME_DEC :
		{
			strData.Format("- %d",Get_Output());

		} break;

		case AVMATRIX_OBJECT_METHOD_DISCONNECT_ALL :		
		break;

		case AVMATRIX_OBJECT_METHOD_VOLUME_UP:
		case AVMATRIX_OBJECT_METHOD_VOLUME_DOWN:
		case AVMATRIX_OBJECT_METHOD_VOLUME_STOP :
		case AVMATRIX_OBJECT_METHOD_MUTE_UNMUTE :
		{
			strData.Format("- %d",Get_Output());

		} break;

		case AVMATRIX_OBJECT_METHOD_MACRO:
		{
			strData.Format("- %d,%i",Get_Output(),Get_MacroCode());

		} break;

		case AVMATRIX_OBJECT_ATTR_ZONEGROUP_SET:
		case AVMATRIX_OBJECT_ATTR_ZONEGROUP_SLAVEADD:
		case AVMATRIX_OBJECT_ATTR_ZONEGROUP_SLAVEREMOVE:
		{
			strData.Format(_T("- %d,%d,%d"),
							 Get_Output(),
							 Get_DestMasterIdx(),
							 Get_DestSlaveIdx());
		} break;

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

////////////////////////////////////////////////////////////////////////
/////////			AVMATRIX INPUT UNIT						////////////
////////////////////////////////////////////////////////////////////////

CBindingAVMatrixUnitInput::CBindingAVMatrixUnitInput(BYTE bBindingUnitType) :	/* TM_AVMATRIX_INPUT */
Base(bBindingUnitType)
{	
	Init();	
}

CBindingAVMatrixUnitInput::CBindingAVMatrixUnitInput(const CBindingUnit &BindingUnit) :
Base(BindingUnit)
{
	Init();	
}

CBindingAVMatrixUnitInput::~CBindingAVMatrixUnitInput()
{
}

void CBindingAVMatrixUnitInput::Init(void)			
{		
	CBindingAVMatrixUnitState* const pUnitState =
		CBindingUnitState::Instance()->get_AvMatrixUnit();

	if (pUnitState)
	{	
		Set_Event(pUnitState->get_Event());
		Set_InpMacro(pUnitState->get_InpMacro());
		Set_ControlState(pUnitState->get_ControlState());	
	}
	else
	{
		ASSERT( pUnitState );

		Set_Event(EV_UNITCONTROLPULS);
		Set_InpMacro(0);
		Set_ControlState(1);
	}

	this->Base::Init( );
}

BOOL CBindingAVMatrixUnitInput::UpgradeIoType()
{
	const BYTE bMessageCode = BindingData.Get_MessageCode();

	if ((bMessageCode == EV_UNITCONTROLTOGGLE) || 
		(bMessageCode == EV_UNITCONTROLPULSTOGGLE) ||
		(bMessageCode == EV_UNITCONTROLPULS)) 
	{
		SetInOutType(UNIT_TYPE_INPUT);
		return TRUE;
	}

	return this->Base::UpgradeIoType();
}

BOOL CBindingAVMatrixUnitInput::ConvertToBindingData(void)
{		
	if (this->GetInOutType() == UNIT_TYPE_INPUT) 
	{
		ASSERT( this->GetBindingUnitType( ) == BINDING_UNIT_TYPE_AVMATRIX_INPUT);

		this->BindingData.Clear();		

		const BYTE bMethodData = this->Get_Event();
		if (bMethodData == EV_UNITCONTROLPULS) 
		{
			BindingData.Set_MessageCode(bMethodData);

			BYTE bTemp = Get_InpMacro() & 0x7F;
			if (bTemp >= CBindingAVMatrixUnitInput::MAX_INPUT_FUNCTIONS)		
			{
				bTemp = CBindingAVMatrixUnitInput::MAX_INPUT_FUNCTIONS-1;
			}
			if (Get_ControlState() & 0x01) 
			{	
				bTemp |= 0x80;
			}
			this->BindingData.Set_BindingFlags(bTemp);				

			CBindingUnitState::Instance()->get_AvMatrixUnit()->set_InpMacro(bTemp & 0x7F);		
			CBindingUnitState::Instance()->get_AvMatrixUnit()->set_Event(bMethodData);

			return TRUE;
		}

		ASSERT( 0 );
		return FALSE;
		
	}
	else
	{
		return this->Base::ConvertToBindingData( );
	}

	return FALSE;
}

BOOL CBindingAVMatrixUnitInput::ConvertFromBindingData(void)
{
	if (this->GetInOutType() == UNIT_TYPE_INPUT) 
	{
		ASSERT( this->GetBindingUnitType( ) == BINDING_UNIT_TYPE_AVMATRIX_INPUT);

		Init();			

		const BYTE bMessageCode = BindingData.Get_MessageCode();
		if (bMessageCode == EV_UNITCONTROLPULS) 
		{
			const BYTE bBindingFlags = BindingData.Get_BindingFlags();
			if (bBindingFlags & 0x80)
			{
				Set_ControlState(1);
			}
			else 
			{
				Set_ControlState(0);
			}

			BYTE bTemp = bBindingFlags & 0x7F;
			if (bTemp >= CBindingAVMatrixUnitInput::MAX_INPUT_FUNCTIONS)	
			{
				bTemp = CBindingAVMatrixUnitInput::MAX_INPUT_FUNCTIONS-1;
			}
			Set_InpMacro(bTemp);

			return TRUE;
		}
		ASSERT(0);
		return FALSE;
	}
	else
	{
		return this->Base::ConvertFromBindingData( );
	}
	ASSERT(0);
	return FALSE;
}

CString CBindingAVMatrixUnitInput::ConvertFuntionToString(void)
{	
	if (this->GetInOutType() == UNIT_TYPE_INPUT) 	
	{
		ASSERT( this->GetBindingUnitType( ) == BINDING_UNIT_TYPE_AVMATRIX_INPUT);

		const BYTE bMethodData = Get_Event();
		
		if (bMethodData == EV_UNITCONTROLPULS) 
		{
			CString strReturn;
			CString strFunction;

			const CBindingStringEntries* const pStringEntries = 
				CBindingUnitStringEntries_Collection::Instance()->Get(AVMATRIX_INPUTSTRINGS_BASIC);

			ASSERT(pStringEntries);
			if (pStringEntries->Find(bMethodData,strFunction))
			{
				strReturn.Format("%s - Macro %d",strFunction, Get_InpMacro( ));
				return strReturn;	
			}
		}
	}
	else
	{
		return this->Base::ConvertFuntionToString( );
	}
	return "";
}
