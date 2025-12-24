#include "stdafx.h"
#include "BindingSoftware.h"
#include "bindingunitaudiohc4.h"

#include "NodeMess.h"
#include "BindingConstants.h"
#include "bindingUIElement.h"	// ??
#include "bindingunitstate.h"

#include "App\Appbindings\appbindingproperty\BindingPropertyStrings.h"
#include "App\Appbindings\appbindingproperty\BindingPropertyUnit.h"	
#include "App\Appbindings\appbindingproperty\BindingPropertyUnitDefault.h"
#include "App\Appbindings\appbindingproperty\BindingPropertyUnitDefaultFactory.h"


using namespace DUOTECNO::BINDINGS::UI;


CBindingHC4AudioUnit::CBindingHC4AudioUnit(BYTE bBindingUnitType) :
Base(bBindingUnitType)
{ 	
	SetInOutType(UNIT_TYPE_OUTPUT);

	Init();		//TM 20040701
}

CBindingHC4AudioUnit::CBindingHC4AudioUnit(const CBindingUnit &BindingUnit) :
CBindingUnit(BindingUnit)
{
	Init();		//TM 20040701
}

CBindingHC4AudioUnit::~CBindingHC4AudioUnit()
{
}

void CBindingHC4AudioUnit::Init(void)			// TM 20040701
{	
	CBindingAudioHC4UnitState* const pUnitState =
		CBindingUnitState::Instance()->get_AudioHC4Unit();

	if (pUnitState)
	{
		Set_Function(pUnitState->get_Function());
		Set_Source(pUnitState->get_Source());			
		Set_Destination(pUnitState->get_Destination());
		Set_State(pUnitState->get_State());				
		Set_Config(pUnitState->get_Config());
		Set_Volume(pUnitState->get_Volume());
		Set_Keypress(pUnitState->get_Keypress());

#ifdef SKIP
		Set_Event(pUnitState->get_Event( ));				/* TM_AUDIOHC4_BINDING_INPUT */
		Set_InpMacro(pUnitState->get_InpMacro( ));			/* TM_AUDIOHC4_BINDING_INPUT */
		Set_ControlState(pUnitState->get_ControlState( ));	/* TM_AUDIOHC4_BINDING_INPUT */
#endif

	}
	else
	{
		Set_Function(AUDIO_MATRIX_OBJECT_METHOD_FSVOLUPDOWN);
		Set_Source(0);			
		Set_Destination(0);
		Set_State(0);				
		Set_Config(0);
		Set_Volume(25);
		Set_Keypress(0);

#ifdef SKIP
		Set_Event(EV_UNITCONTROLPULS);				/* TM_AUDIOHC4_BINDING_INPUT */
		Set_InpMacro(0);							/* TM_AUDIOHC4_BINDING_INPUT */
		Set_ControlState(1);						/* TM_AUDIOHC4_BINDING_INPUT */	
#endif

	}
}

BOOL CBindingHC4AudioUnit::UpgradeIoType()
{
	const BYTE bMessageCode = BindingData.Get_MessageCode();

#ifdef SKIP

	if ((bMessageCode == EV_UNITCONTROLTOGGLE) || 
		(bMessageCode == EV_UNITCONTROLPULSTOGGLE) ||
		(bMessageCode == EV_UNITCONTROLPULS)) 
	{
		SetInOutType(UNIT_TYPE_INPUT);
		return TRUE;
	}
	else

#endif

	if ( bMessageCode == FC_UNITAUDIOSET )
	{
		SetInOutType(UNIT_TYPE_OUTPUT);
		return TRUE;
	}
	return FALSE;
}

BOOL CBindingHC4AudioUnit::ConvertToBindingData(void)
{
	BindingData.Clear();
	
	if (GetInOutType( ) == UNIT_TYPE_OUTPUT)
	{
		BindingData.Set_MessageCode(FC_UNITAUDIOSET);

		const BYTE bMethodData = Get_Function();
		BindingData.Set_MethodData(bMethodData);
		
		switch (bMethodData)
		{
			case AUDIO_MATRIX_OBJECT_ATTR_SOURCE_ON :
			{
				BindingData.Set_Length(3);
				BindingData.Set_Data(1,Get_Destination());
				BindingData.Set_Data(2,Get_Source());
				
				CBindingAudioHC4UnitState* const pUnitState =
					CBindingUnitState::Instance()->get_AudioHC4Unit();

				if (pUnitState)
				{
					pUnitState->set_Function(bMethodData);
					pUnitState->set_Destination(Get_Destination());
					pUnitState->set_Source(Get_Source());
				}	

			} return TRUE;

			case AUDIO_MATRIX_OBJECT_METHOD_CSON :
			case AUDIO_MATRIX_OBJECT_METHOD_OFF :
			case AUDIO_MATRIX_OBJECT_METHOD_FSCSONOFF :
			case AUDIO_MATRIX_OBJECT_METHOD_VOLUP :
			case AUDIO_MATRIX_OBJECT_METHOD_VOLDOWN :
			case AUDIO_MATRIX_OBJECT_METHOD_VOLSTOP :
			case AUDIO_MATRIX_OBJECT_METHOD_FSVOLUPDOWN :
			case AUDIO_MATRIX_OBJECT_METHOD_PLAY :
			case AUDIO_MATRIX_OBJECT_METHOD_STOP :
			case AUDIO_MATRIX_OBJECT_METHOD_PAUSE :
			case AUDIO_MATRIX_OBJECT_METHOD_FF :
			case AUDIO_MATRIX_OBJECT_METHOD_RR :
			case AUDIO_MATRIX_OBJECT_METHOD_FFWD :
			case AUDIO_MATRIX_OBJECT_METHOD_FRWD :
			case AUDIO_MATRIX_OBJECT_METHOD_MUTE_UNMUTE :
			case AUDIO_MATRIX_OBJECT_METHOD_SOURCEINC :
			case AUDIO_MATRIX_OBJECT_METHOD_SOURCEDEC :
			{
				BindingData.Set_Length(2);
				BindingData.Set_Data(1,Get_Destination());

				CBindingAudioHC4UnitState* const pUnitState =
					CBindingUnitState::Instance()->get_AudioHC4Unit();

				if (pUnitState)
				{
					pUnitState->set_Function(bMethodData);
					pUnitState->set_Destination(Get_Destination());				
				}	

			} return TRUE;

			case AUDIO_MATRIX_OBJECT_ATTR_ONOFF :
			{
				BindingData.Set_Length(3);
				BindingData.Set_Data(1,Get_Destination());
				BindingData.Set_Data(2,Get_State());

				CBindingAudioHC4UnitState* const pUnitState =
					CBindingUnitState::Instance()->get_AudioHC4Unit();

				if (pUnitState)
				{
					pUnitState->set_Function(bMethodData);
					pUnitState->set_Destination(Get_Destination());
					pUnitState->set_State(Get_State());
				}	

			} return TRUE;

			case AUDIO_MATRIX_OBJECT_ATTR_VOLUME :
			{
				BindingData.Set_Length(3);
				BindingData.Set_Data(1,Get_Destination());
				BindingData.Set_Data(2,Get_Volume());

				CBindingAudioHC4UnitState* const pUnitState =
					CBindingUnitState::Instance()->get_AudioHC4Unit();

				if (pUnitState)
				{
					pUnitState->set_Function(bMethodData);
					pUnitState->set_Destination(Get_Destination());
					pUnitState->set_Volume(Get_Volume());
				}	

			} return TRUE;

			case AUDIO_MATRIX_OBJECT_METHOD_EXTRAIR :
			{
				BindingData.Set_Length(3);
				BindingData.Set_Data(1,Get_Destination());
				BindingData.Set_Data(2,Get_Keypress());

				CBindingAudioHC4UnitState* const pUnitState =
					CBindingUnitState::Instance()->get_AudioHC4Unit();

				if (pUnitState)
				{
					pUnitState->set_Function(bMethodData);
					pUnitState->set_Destination(Get_Destination());
					pUnitState->set_Keypress(Get_Keypress());
				}	

			} return TRUE;

			default:
				ASSERT(0);
			return FALSE;
		}
	}
	else
	{
		AfxMessageBox("The general audio unit must always be an output unit");
		return FALSE;

		
#ifdef SKIP

		const BYTE bMethodData = Get_Event();
		if (bMethodData == EV_UNITCONTROLPULS ||
			bMethodData == EV_UNITCONTROLPULSTOGGLE ||
			bMethodData == EV_UNITCONTROLTOGGLE)
		{
			BindingData.Set_MessageCode(bMethodData);

			BYTE bTemp = Get_InpMacro() & 0x7F;
			if (bTemp >= CBindingHC4AudioUnit::MAX_AUDIO_HC4_INPUT_MACROS)		
			{
				bTemp = CBindingHC4AudioUnit::MAX_AUDIO_HC4_INPUT_MACROS-1;
			}
			if (Get_ControlState() & 0x01) 
			{	
				bTemp |= 0x80;
			}
			BindingData.Set_BindingFlags(bTemp);				

			CBindingUnitState::Instance()->get_AudioHC4Unit()->set_InpMacro(bTemp & 0x7F);		
			CBindingUnitState::Instance()->get_AudioHC4Unit()->set_Event(bMethodData);

			return TRUE;
		}

		ASSERT( 0 );
		return FALSE;
#endif

	}
	
	ASSERT(0);
	return FALSE;
}

BOOL CBindingHC4AudioUnit::ConvertFromBindingData(void)
{
	Init();			
	
	if (GetInOutType( ) == UNIT_TYPE_OUTPUT)
	{
		const BYTE bMessageCode = BindingData.Get_MessageCode();
		if (bMessageCode != FC_UNITAUDIOSET) 
		{
			ASSERT(0);
			return FALSE;
		}

		const BYTE bMethodData = BindingData.Get_MethodData();
		Set_Function(bMethodData);

		const BYTE bLength = BindingData.Get_Length();

		switch (bMethodData)
		{
			case AUDIO_MATRIX_OBJECT_ATTR_SOURCE_ON :
			{
				if(bLength != 3) return FALSE;
				
				Set_Destination(BindingData.Get_Data(1));
				Set_Source(BindingData.Get_Data(2));		

			} return TRUE;


			case AUDIO_MATRIX_OBJECT_METHOD_CSON :
			case AUDIO_MATRIX_OBJECT_METHOD_OFF :
			case AUDIO_MATRIX_OBJECT_METHOD_FSCSONOFF :
			case AUDIO_MATRIX_OBJECT_METHOD_VOLUP :
			case AUDIO_MATRIX_OBJECT_METHOD_VOLDOWN :
			case AUDIO_MATRIX_OBJECT_METHOD_VOLSTOP :
			case AUDIO_MATRIX_OBJECT_METHOD_FSVOLUPDOWN :
			case AUDIO_MATRIX_OBJECT_METHOD_PLAY :
			case AUDIO_MATRIX_OBJECT_METHOD_STOP :
			case AUDIO_MATRIX_OBJECT_METHOD_PAUSE :
			case AUDIO_MATRIX_OBJECT_METHOD_FF :
			case AUDIO_MATRIX_OBJECT_METHOD_RR :
			case AUDIO_MATRIX_OBJECT_METHOD_FFWD :
			case AUDIO_MATRIX_OBJECT_METHOD_FRWD :
			case AUDIO_MATRIX_OBJECT_METHOD_MUTE_UNMUTE :
			case AUDIO_MATRIX_OBJECT_METHOD_SOURCEINC :
			case AUDIO_MATRIX_OBJECT_METHOD_SOURCEDEC :
			{
				if (bLength !=2) return FALSE;

				Set_Destination(BindingData.Get_Data(1));		

			} return TRUE;

			case AUDIO_MATRIX_OBJECT_ATTR_ONOFF :
			{
				if (bLength !=3) return FALSE;

				Set_Destination(BindingData.Get_Data(1));
				Set_State(BindingData.Get_Data(2));		

			} return TRUE;

			case AUDIO_MATRIX_OBJECT_ATTR_VOLUME :
			{
				if (bLength !=3) return FALSE;

				Set_Destination(BindingData.Get_Data(1));
				Set_Volume(BindingData.Get_Data(2));		

			} return TRUE;

			case AUDIO_MATRIX_OBJECT_METHOD_EXTRAIR :
			{
				if (bLength !=3) return FALSE;

				Set_Destination(BindingData.Get_Data(1));
				Set_Keypress(BindingData.Get_Data(2));	

			} return TRUE;

			default:
			{
				ASSERT(0);

			} return FALSE;
		}
	}
	else
	{	
		/**
		 *	TODO: Melding geven !!!!!!!!!!!!!!!!!!!!
		 */
/*
		const BYTE bMessageCode = BindingData.Get_MessageCode();
		if ((bMessageCode == EV_UNITCONTROLTOGGLE) || 
			(bMessageCode == EV_UNITCONTROLPULSTOGGLE) ||
			(bMessageCode == EV_UNITCONTROLPULS)) 
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
			if (bTemp >= CBindingHC4AudioUnit::MAX_AUDIO_HC4_INPUT_MACROS)		// TM20110509
			{
				bTemp = CBindingHC4AudioUnit::MAX_AUDIO_HC4_INPUT_MACROS-1;
			}
			Set_InpMacro(bTemp);

			return TRUE;
		}
		ASSERT(0);
		return FALSE;
*/

	}
	ASSERT(0);
	return FALSE;
}


////////////////////////// TEMP /////////////////////////////////////
namespace 
{
	const char* _szKeypresses[ 25 ] =
	{
		"1",
		"2",
		"3",
		"4",
		"5",
		"6",
		"7",
		"8",
		"9",
		"0",
		"Up",
		"Down",
		"Left",
		"Right",
		"Enter",
		"+10",
		"Free 1",
		"Free 2",
		"Free 3",
		"Free 4",
		"Free 5",
		"Up",
		"Down",
		"Up2",
		"Down2"	
	};
};

CString CBindingHC4AudioUnit::ConvertFuntionToString(void)
{
	CString strReturn;
	CString strFunction;

	if (GetInOutType( ) == UNIT_TYPE_OUTPUT)
	{
		const BYTE bMethodData = Get_Function();
		const CBindingStringEntries* const pStringEntries = 
			CBindingUnitStringEntries_Collection::Instance()->Get(AUDIOHC4_STRINGS_EXTENDED);

		ASSERT(pStringEntries);
		pStringEntries->Find(bMethodData,strFunction);

		switch(bMethodData)
		{
			case AUDIO_OBJECT_ATTR_SOURCE_ON:		
			{
				strReturn.Format("%s - Dest %d,%d",
								strFunction,
								Get_Destination(),
								Get_Source());				

			} return strReturn;

			case AUDIO_MATRIX_OBJECT_METHOD_CSON :
			case AUDIO_MATRIX_OBJECT_METHOD_OFF :
			case AUDIO_MATRIX_OBJECT_METHOD_FSCSONOFF :
			case AUDIO_MATRIX_OBJECT_METHOD_VOLUP :
			case AUDIO_MATRIX_OBJECT_METHOD_VOLDOWN :
			case AUDIO_MATRIX_OBJECT_METHOD_VOLSTOP :
			case AUDIO_MATRIX_OBJECT_METHOD_FSVOLUPDOWN :
			case AUDIO_MATRIX_OBJECT_METHOD_PLAY :
			case AUDIO_MATRIX_OBJECT_METHOD_STOP :
			case AUDIO_MATRIX_OBJECT_METHOD_PAUSE :
			case AUDIO_MATRIX_OBJECT_METHOD_FF :
			case AUDIO_MATRIX_OBJECT_METHOD_RR :
			case AUDIO_MATRIX_OBJECT_METHOD_FFWD :
			case AUDIO_MATRIX_OBJECT_METHOD_FRWD :
			case AUDIO_MATRIX_OBJECT_METHOD_MUTE_UNMUTE :
			case AUDIO_MATRIX_OBJECT_METHOD_SOURCEINC :
			case AUDIO_MATRIX_OBJECT_METHOD_SOURCEDEC :
			{
				strReturn.Format("%s - Dest %d",
								 strFunction,
								 Get_Destination());
			
			} return strReturn;

			case AUDIO_MATRIX_OBJECT_ATTR_ONOFF :
			{
				CString strTemp;

				if (Get_State() & 0x01) strTemp.LoadString(IDS_ON);
				else strTemp.LoadString(IDS_OFF);

				strReturn.Format("%s - Dest %d,%s",
								 strFunction,
								 Get_Destination(),
								 strTemp);

			} return strReturn;

			case AUDIO_MATRIX_OBJECT_ATTR_VOLUME :
			{
				strReturn.Format("%s - Dest %d,%d",
								 strFunction,
								 Get_Destination(),
								 Get_Volume());		

			} return strReturn;

			case AUDIO_MATRIX_OBJECT_METHOD_EXTRAIR :
			{
				const BYTE bTemp = Get_Keypress();

				if (bTemp < 25)
				{
					strReturn.Format("%s - Dest %d,%s",
									 strFunction,
									 Get_Destination(),
									 _szKeypresses[bTemp]);
				}
				else
				{
					strReturn.Format("%s - Dest %d,%d",strFunction,
									 Get_Destination(),
									 bTemp);			
				}

			} return strReturn;

			default:
			{
				ASSERT(0);

			} return strFunction;					
		}
	}
	else
	{

#ifdef SKIP

		const BYTE bMethodData = Get_Event();
		
		if (bMethodData == EV_UNITCONTROLPULS ||
			bMethodData == EV_UNITCONTROLPULSTOGGLE ||
			bMethodData == EV_UNITCONTROLTOGGLE)
		{
			const CBindingStringEntries* const pStringEntries = 
				CBindingUnitStringEntries_Collection::Instance()->Get(AUDIOHC4_INPUTSTRINGS_BASIC);

			ASSERT(pStringEntries);
			if (pStringEntries->Find(bMethodData,strFunction))
			{
				strReturn.Format("%s - Macro %d",strFunction, Get_InpMacro( ));
				return strReturn;	
			}
		}

#endif

	}

	return strFunction;
}


//////////////////////////////////////////////////////////////////
///////		     HC4 INPUT AUDIO UNIT					//////////
//////////////////////////////////////////////////////////////////
CBindingHC4AudioUnit_Input::CBindingHC4AudioUnit_Input(BYTE bBindingUnitType) :
Base(bBindingUnitType)
{ 	
	SetInOutType(UNIT_TYPE_OUTPUT);
	this->CBindingHC4AudioUnit_Input::Init();		
}

CBindingHC4AudioUnit_Input::CBindingHC4AudioUnit_Input(const CBindingUnit &BindingUnit) :
Base(BindingUnit)
{
	Init();		
}

CBindingHC4AudioUnit_Input::~CBindingHC4AudioUnit_Input()
{
}

void CBindingHC4AudioUnit_Input::Init(void)			
{	
	CBindingAudioHC4UnitState* const pUnitState =
		CBindingUnitState::Instance()->get_AudioHC4Unit();

	if (pUnitState)
	{	
		Set_Event(pUnitState->get_Event( ));				/* TM_AUDIOHC4_BINDING_INPUT */
		Set_InpMacro(pUnitState->get_InpMacro( ));			/* TM_AUDIOHC4_BINDING_INPUT */
		Set_ControlState(pUnitState->get_ControlState( ));	/* TM_AUDIOHC4_BINDING_INPUT */
	}
	else
	{		
		Set_Event(EV_UNITCONTROLPULS);				/* TM_AUDIOHC4_BINDING_INPUT */
		Set_InpMacro(0);							/* TM_AUDIOHC4_BINDING_INPUT */
		Set_ControlState(1);						/* TM_AUDIOHC4_BINDING_INPUT */	
	}

	this->Base::Init( );
}

BOOL CBindingHC4AudioUnit_Input::UpgradeIoType()
{
	const BYTE bMessageCode = BindingData.Get_MessageCode();

	if ((bMessageCode == EV_UNITCONTROLTOGGLE) || 
		(bMessageCode == EV_UNITCONTROLPULSTOGGLE) ||
		(bMessageCode == EV_UNITCONTROLPULS)) 
	{
		SetInOutType(UNIT_TYPE_INPUT);
		return TRUE;
	}
	else
	{
		return this->Base::UpgradeIoType( );
	}
	return FALSE;
}

BOOL CBindingHC4AudioUnit_Input::ConvertToBindingData(void)
{
	if (this->GetInOutType( ) == UNIT_TYPE_OUTPUT)
	{
		return this->Base::ConvertToBindingData();
	}
	else
	{
		BindingData.Clear();
			
		const BYTE bMethodData = Get_Event();
		if (bMethodData == EV_UNITCONTROLPULS ||
			bMethodData == EV_UNITCONTROLPULSTOGGLE ||
			bMethodData == EV_UNITCONTROLTOGGLE)
		{
			BindingData.Set_MessageCode(bMethodData);

			BYTE bTemp = Get_InpMacro() & 0x7F;
			if (bTemp >= CBindingHC4AudioUnit_Input::MAX_AUDIO_HC4_INPUT_MACROS)		
			{
				bTemp = CBindingHC4AudioUnit_Input::MAX_AUDIO_HC4_INPUT_MACROS-1;
			}
			if (Get_ControlState() & 0x01) 
			{	
				bTemp |= 0x80;
			}
			BindingData.Set_BindingFlags(bTemp);				

			CBindingUnitState::Instance()->get_AudioHC4Unit()->set_InpMacro(bTemp & 0x7F);		
			CBindingUnitState::Instance()->get_AudioHC4Unit()->set_Event(bMethodData);

			return TRUE;
		}

		ASSERT( 0 );
		return FALSE;
	}
	
	ASSERT(0);
	return FALSE;
}

BOOL CBindingHC4AudioUnit_Input::ConvertFromBindingData(void)
{					
	if (GetInOutType( ) == UNIT_TYPE_OUTPUT)
	{
		return this->Base::ConvertFromBindingData( );
	}
	else
	{	
		Init();

		const BYTE bMessageCode = BindingData.Get_MessageCode();
		if ((bMessageCode == EV_UNITCONTROLTOGGLE) || 
			(bMessageCode == EV_UNITCONTROLPULSTOGGLE) ||
			(bMessageCode == EV_UNITCONTROLPULS)) 
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
			if (bTemp >= CBindingHC4AudioUnit_Input::MAX_AUDIO_HC4_INPUT_MACROS)		
			{
				bTemp = CBindingHC4AudioUnit_Input::MAX_AUDIO_HC4_INPUT_MACROS-1;
			}
			Set_InpMacro(bTemp);

			return TRUE;
		}
		ASSERT(0);
		return FALSE;
	}
	ASSERT(0);
	return FALSE;
}

CString CBindingHC4AudioUnit_Input::ConvertFuntionToString(void)
{	
	CString strFunction;

	if (GetInOutType( ) == UNIT_TYPE_OUTPUT)
	{
		return this->Base::ConvertFuntionToString( );
	}
	else
	{
		const BYTE bMethodData = Get_Event();
		
		if (bMethodData == EV_UNITCONTROLPULS ||
			bMethodData == EV_UNITCONTROLPULSTOGGLE ||
			bMethodData == EV_UNITCONTROLTOGGLE)
		{
			const CBindingStringEntries* const pStringEntries = 
				CBindingUnitStringEntries_Collection::Instance()->Get(AUDIOHC4_INPUTSTRINGS_BASIC);

			ASSERT(pStringEntries);
			if (pStringEntries->Find(bMethodData,strFunction))
			{
				CString strReturn;

				strReturn.Format("%s - Macro %d",strFunction, Get_InpMacro( ));
				return strReturn;	
			}
		}
	}
	return strFunction;
}

//////////////////////////////////////////////////////////////////
/////////		     HC4 BNO AUDIO UNIT					//////////
//////////////////////////////////////////////////////////////////
const CBindingHC4AudioBnoUnit::STRUCT_AVMATRIX_MACRO_BEO4_EVENT 
	CBindingHC4AudioBnoUnit::tableMacroBeo4Events[ MAX_AUDIO_HC4_BNO_INPUT_MACROS ] =
{
	{ "Light Digit 0", 0x00, 0x00, 0x00 },							/* 0 */
	{ "Light Digit 1", 0x01, 0x00, 0x01 },							/* 1 */
	{ "Light Digit 2", 0x02, 0x00, 0x02 },							/* 2 */
	{ "Light Digit 3", 0x03, 0x00, 0x03 },							/* 3 */
	{ "Light Digit 4", 0x04, 0x00, 0x04 },							/* 4 */
	{ "Light Digit 5", 0x05, 0x00, 0x05 },							/* 5 */
	{ "Light Digit 6", 0x06, 0x00, 0x06 },							/* 6 */
	{ "Light Digit 7", 0x07, 0x00, 0x07 },							/* 7 */
	{ "Light Digit 8", 0x08, 0x00, 0x08 },							/* 8 */
	{ "Light Digit 9", 0x09, 0x00, 0x09 },							/* 9 */		
	{ "Light Go/Play", 0x0a, 0x00, 0x35 },							/* 10 */
	{ "Light Stop", 0x0b, 0x00, 0x36 },								/* 11 */
	{ "Light Cntl C Rewind/Green", 0x0c, 0x00, 0x76 },				/* 12 */
	{ "Light Cntl C Wind/Yellow", 0x0d, 0x00, 0x77 },				/* 13 */
	{ "Light Cntl C Step Up/Blue", 0x0e, 0x00, 0x78 },				/* 14 */
	{ "Light Cntl C Step Dw/Red", 0x0f, 0x00, 0x79 },				/* 15 */
	{ "Light Step Up", 0x10, 0x00, 0x1e },							/* 16 */
	{ "Light Step Down", 0x11, 0x00, 0x1f },						/* 17 */
	{ "Light Rewind", 0x12, 0x00, 0x32 },							/* 18 */
	{ "Light Wind", 0x13, 0x00, 0x34 },								/* 19 */
	{ "Light Function 1", 0x14, 0x00, 0x0f },						/* 20 */
	{ "Light Function 2", 0x15, 0x00, 0x10 },						/* 21 */
	{ "Light Function 3", 0x16, 0x00, 0x11 },						/* 22 */
	{ "Light Function 4", 0x17, 0x00, 0x12 },						/* 23 */
	{ "Light Function 5", 0x18, 0x00, 0x19 },						/* 24 */
	{ "Light Function 6", 0x19, 0x00, 0x1a },						/* 25 */
	{ "Light Function 7", 0x1a, 0x00, 0x21 },						/* 26 */
	{ "Light Function 8", 0x1b, 0x00, 0x22 },						/* 27 */
	{ "Light Function 9", 0x1c, 0x00, 0x23 },						/* 28 */
	{ "Light Function 10", 0x1d, 0x00, 0x24 },						/* 29 */
	{ "Light Function 11", 0x1e, 0x00, 0x25 },						/* 30 */
	{ "Light Function 12", 0x1f, 0x00, 0x26 },						/* 31 */
	{ "Light Function 13", 0x20, 0x00, 0x27 },						/* 32 */
	{ "Light Function 14", 0x21, 0x00, 0x39 },						/* 33 */
	{ "Light Function 15", 0x22, 0x00, 0x3a },						/* 34 */
	{ "Light Function 16", 0x23, 0x00, 0x3b },						/* 35 */
	{ "Light Function 17", 0x24, 0x00, 0x3c },						/* 36 */
	{ "Light Function 18", 0x25, 0x00, 0x3d },						/* 37 */
	{ "Light Function 19", 0x26, 0x00, 0x3e },						/* 38 */
	{ "Light Function 20", 0x27, 0x00, 0x4b },						/* 39 */
	{ "Light Function 21", 0x28, 0x00, 0x4c },						/* 40 */
	{ "Light Function 22", 0x29, 0x00, 0x50 },						/* 41 */
	{ "Light Function 23", 0x2a, 0x00, 0x51 },						/* 42 */
	{ "Light Function 24", 0x2b, 0x00, 0x7d },						/* 43 */
	{ "Light Function 25", 0x2c, 0x00, 0xa5 },						/* 44 */
	{ "Light Function 26", 0x2d, 0x00, 0xa6 },						/* 45 */
	{ "Light Function 27", 0x2e, 0x00, 0xa9 },						/* 46 */
	{ "Light Function 28", 0x2f, 0x00, 0xaa },						/* 47 */
	{ "Light Function 29", 0x30, 0x00, 0xdd },						/* 48 */
	{ "Light Function 30", 0x31, 0x00, 0xde },						/* 49 */
	{ "Control Digit 0", 0x32, 0x01, 0x00 },						/* 50 */
	{ "Control Digit 1", 0x33, 0x01, 0x01 },						/* 51 */
	{ "Control Digit 2", 0x34, 0x01, 0x02 },						/* 52 */
	{ "Control Digit 3", 0x35, 0x01, 0x03 },						/* 53 */
	{ "Control Digit 4", 0x36, 0x01, 0x04 },						/* 54 */
	{ "Control Digit 5", 0x37, 0x01, 0x05 },						/* 55 */
	{ "Control Digit 6", 0x38, 0x01, 0x06 },						/* 56 */
	{ "Control Digit 7", 0x39, 0x01, 0x07 },						/* 57 */
	{ "Control Digit 8", 0x3a, 0x01, 0x08 },						/* 58 */
	{ "Control Digit 9", 0x3b, 0x01, 0x09 },						/* 59 */
	{ "Control Go/Play", 0x3c, 0x01, 0x35 },						/* 60 */
	{ "Control Stop", 0x3d, 0x01, 0x36 },							/* 61 */
	{ "Control Cntl C Rewind/Green", 0x3e, 0x01, 0x76 },			/* 62 */
	{ "Control Cntl C Wind/Yellow", 0x3f, 0x01, 0x77 },				/* 63 */
	{ "Control Cntl C Step Up/Blue", 0x40, 0x01, 0x78 },			/* 64 */
	{ "Control Cntl C Step Dw/Red", 0x41, 0x01, 0x79 },				/* 65 */
	{ "Control Step Up", 0x42, 0x01, 0x1e },						/* 66 */
	{ "Control Step Down", 0x43, 0x01, 0x1f },						/* 67 */
	{ "Control Rewind", 0x44, 0x01, 0x32 },							/* 68 */
	{ "Control Wind", 0x45, 0x01, 0x34 },							/* 69 */
	{ "Control Function 1", 0x46, 0x01, 0x0f },						/* 70 */
	{ "Control Function 2", 0x47, 0x01, 0x10 },						/* 71 */
	{ "Control Function 3", 0x48, 0x01, 0x11 },						/* 72 */
	{ "Control Function 4", 0x49, 0x01, 0x12 },						/* 73 */
	{ "Control Function 5", 0x4a, 0x01, 0x19 },						/* 74 */
	{ "Control Function 6", 0x4b, 0x01, 0x1a },						/* 75 */
	{ "Control Function 7", 0x4c, 0x01, 0x21 },						/* 76 */
	{ "Control Function 8", 0x4d, 0x01, 0x22 },						/* 77 */
	{ "Control Function 9", 0x4e, 0x01, 0x23 },						/* 78 */
	{ "Control Function 10", 0x4f, 0x01, 0x24 },					/* 79 */
	{ "Control Function 11", 0x50, 0x01, 0x25 },					/* 80 */
	{ "Control Function 12", 0x51, 0x01, 0x26 },					/* 81 */
	{ "Control Function 13", 0x52, 0x01, 0x27 },					/* 82 */
	{ "Control Function 14", 0x53, 0x01, 0x39 },					/* 83 */
	{ "Control Function 15", 0x54, 0x01, 0x3a },					/* 84 */
	{ "Control Function 16", 0x55, 0x01, 0x3b },					/* 85 */
	{ "Control Function 17", 0x56, 0x01, 0x3c },					/* 86 */
	{ "Control Function 18", 0x57, 0x01, 0x3d },					/* 87 */
	{ "Control Function 19", 0x58, 0x01, 0x3e },					/* 88 */
	{ "Control Function 20", 0x59, 0x01, 0x4b },					/* 89 */
	{ "Control Function 21", 0x5a, 0x01, 0x4c },					/* 90 */
	{ "Control Function 22", 0x5b, 0x01, 0x50 },					/* 91 */
	{ "Control Function 23", 0x5c, 0x01, 0x51 },					/* 92 */
	{ "Control Function 24", 0x5d, 0x01, 0x7d },					/* 93 */
	{ "Control Function 25", 0x5e, 0x01, 0xa5 },					/* 94 */
	{ "Control Function 26", 0x5f, 0x01, 0xa6 },					/* 95 */
	{ "Control Function 27", 0x60, 0x01, 0xa9 },					/* 96 */
	{ "Control Function 28", 0x61, 0x01, 0xaa },					/* 97 */
	{ "Control Function 29", 0x62, 0x01, 0xdd },					/* 98 */
	{ "Control Function 30", 0x63, 0x01, 0xde },					/* 99 */
};

CBindingHC4AudioBnoUnit::CBindingHC4AudioBnoUnit(BYTE bBindingUnitType) :
Base(bBindingUnitType)
{ 	
}

CBindingHC4AudioBnoUnit::CBindingHC4AudioBnoUnit(const CBindingUnit &BindingUnit) :
Base(BindingUnit)
{	
}

CBindingHC4AudioBnoUnit::~CBindingHC4AudioBnoUnit()
{
}

/*
BYTE CBindingHC4AudioBnoUnit::Get_Event(void) const
{
	BYTE bTemp = this->Base::Get_Event( );
	if (bEvent != EV_UNITCONTROLPULS)
	{
		bTemp = EV_UNITCONTROLPULS;
	}	
	return bTemp;
}

BYTE CBindingHC4AudioBnoUnit::Get_InpMacro(void) const
{	
	BYTE bTemp = this->Base::Get_InpMacro( );
	
	if (bTemp >= MAX_AUDIO_HC4_BNO_INPUT_MACROS)
	{
		bTemp = MAX_AUDIO_HC4_BNO_INPUT_MACROS - 1;
	}	
	return bTemp;
}

void CBindingHC4AudioBnoUnit::Set_InpMacro(BYTE Macro)
{
	ASSERT( Macro < MAX_AUDIO_HC4_BNO_INPUT_MACROS );

	if (Macro >= MAX_AUDIO_HC4_BNO_INPUT_MACROS)
		Macro = MAX_AUDIO_HC4_BNO_INPUT_MACROS - 1;

	this->CBindingHC4AudioUnit::Set_InpMacro(Macro);
}

void CBindingHC4AudioBnoUnit::Set_Event(BYTE Event)
{
	ASSERT( Event == EV_UNITCONTROLPULS );

	if (Event != EV_UNITCONTROLPULS)
	{
		Event = EV_UNITCONTROLPULS;
	}
	this->CBindingHC4AudioUnit::Set_Event( Event );
}
*/

